#include "IConnectionBluetooth.h"

#if defined TARGET_OS_Windows
#define WIN32_LEAN_AND_MEAN
#define UNICODE
#include <Windows.h>
#include <CommCtrl.h>
#include <codecvt>
#include <WinSock2.h>
#include <ws2bth.h>
#include <bluetoothapis.h>
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	#include <sys/time.h>
	#include <sys/types.h>
	#include <sys/select.h>
	#include <sys/stat.h>
	#include <sys/ioctl.h>

	#include <unistd.h>
	#include <fcntl.h>
	#include <termios.h>
#else
	#error "Unsupported platform"
#endif

#include <assert.h>
#include <iostream>
#include <vector>
#include <algorithm>

namespace Socket
{
	class CConnectionBluetooth final : public IConnectionBluetooth
	{
	public:

#if defined TARGET_OS_Windows
		CConnectionBluetooth() : m_oSocket(INVALID_SOCKET) { }
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		CConnectionBluetooth() : m_sLastError() { }
#endif
		
#if defined TARGET_OS_Windows
		bool initialize()
		{
			WSADATA l_oWSAData;

			// Ask for Winsock version.
			if (_WINSOCK2API_::WSAStartup(MAKEWORD(m_ui8WinSocketMajorVersion, m_ui8WinSocketMinorVersion), &l_oWSAData) != 0)
			{
				m_sLastError = "Failed to start Winsock " + std::to_string(m_ui8WinSocketMajorVersion) + "." + std::to_string(m_ui8WinSocketMinorVersion) + ": " + this->getLastErrorFormated();
				return false;
			}

			// Confirm that the WinSock DLL supports version requested.
			// Note that if the DLL supports versions greater than the version requested, in addition to the version requested, it will still return the version requested in wVersion.
			if (LOBYTE(l_oWSAData.wVersion) != m_ui8WinSocketMajorVersion || HIBYTE(l_oWSAData.wVersion) != m_ui8WinSocketMinorVersion)
			{
				m_sLastError = "Could not find a usable version of Winsock.dll.";
				_WINSOCK2API_::WSACleanup();
				return false;
			}
			return true;
		}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		bool initialize() { return false; }
#endif

		bool open() override { return false; }

		bool close() override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Bluetooth device is not connected.";
				return false;
			}

#if defined TARGET_OS_Windows

			bool l_bIsSuccess = true;
			if (m_oSocket != INVALID_SOCKET)
			{
				// shutdown the connection since no more data will be sent or received
				if (_WINSOCK2API_::shutdown(m_oSocket, SD_BOTH) == SOCKET_ERROR)
				{
					m_sLastError = "Failed to shutdown the bluetooth socket:" + this->getLastErrorFormated();
					l_bIsSuccess = false;
				}

				if (_WINSOCK2API_::closesocket(m_oSocket) == SOCKET_ERROR)
				{
					m_sLastError = "Failed to close the bluetooth socket:" + this->getLastErrorFormated();
					l_bIsSuccess = false;
				}

				if (_WINSOCK2API_::WSACleanup() == SOCKET_ERROR)
				{
					m_sLastError = "Failed to cleanup the bluetooth socket:" + this->getLastErrorFormated();
					l_bIsSuccess = false;
				}

				m_oSocket = INVALID_SOCKET;
			}

			return l_bIsSuccess;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return false;
#endif
		}

		bool isReadyToSend(const uint32_t /*timeOut*/) const override { return this->isConnected(); }

		bool isReadyToReceive(const uint32_t /*timeOut*/) const override
		{
			if (!this->isConnected()) { return false; }

#if defined TARGET_OS_Windows

			unsigned long nPendingBytes = 0;

			if (_WINSOCK2API_::ioctlsocket(m_oSocket, FIONREAD, &nPendingBytes) == SOCKET_ERROR)
			{
				//m_sLastError = "Failed to get the pending bytes count: " + this->getLastErrorFormated();
				return false;
			}

			return nPendingBytes != 0;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return false;
#endif
		}

		uint32_t getPendingByteCount() override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Bluetooth device not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows

			unsigned long nPendingBytes = 0;

			if (_WINSOCK2API_::ioctlsocket(m_oSocket, FIONREAD, &nPendingBytes) == SOCKET_ERROR)
			{
				m_sLastError = "Failed to get the pending bytes count: " + this->getLastErrorFormated();
				return 0;
			}

			return nPendingBytes;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return 0;
#endif
		}

		uint32_t sendBuffer(const void* buffer, const uint32_t ui32BufferSize) override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Bluetooth device is not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows

			const int nBytesSent = _WINSOCK2API_::send(m_oSocket, (char *)buffer, ui32BufferSize, 0);

			if (nBytesSent == SOCKET_ERROR)
			{
				m_sLastError = "Failed to write on the bluetooth port: " + getLastErrorFormated();
				this->close();
				return 0;
			}

			return uint32_t(nBytesSent);

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return 0;
#endif
		}

		uint32_t receiveBuffer(void* buffer, const uint32_t ui32BufferSize) override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Bluetooth device is not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows


			const int nBytesReceived = _WINSOCK2API_::recv(m_oSocket, static_cast<char *>(buffer), ui32BufferSize, 0);

			if (nBytesReceived == SOCKET_ERROR)
			{
				m_sLastError = "Failed to receive data from bluetooth: " + getLastErrorFormated();

				this->close();
				return 0;
			}

			return uint32_t(nBytesReceived);

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return 0;
#endif
		}

		bool sendBufferBlocking(const void* buffer, const uint32_t ui32BufferSize) override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Bluetooth device is not connected.";
				return false;
			}

			const char* p   = reinterpret_cast<const char*>(buffer);
			uint32_t l_ui32BytesLeft = ui32BufferSize;

			while (l_ui32BytesLeft != 0 && this->isConnected())
			{
				l_ui32BytesLeft -= this->sendBuffer(p + ui32BufferSize - l_ui32BytesLeft, l_ui32BytesLeft);

				if (this->isErrorRaised()) { return false; }
			}

			return l_ui32BytesLeft == 0;
		}

		bool receiveBufferBlocking(void* buffer, const uint32_t ui32BufferSize) override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Bluetooth device is not connected.";
				return false;
			}

			char* p         = reinterpret_cast<char*>(buffer);
			uint32_t l_ui32BytesLeft = ui32BufferSize;

			while (l_ui32BytesLeft != 0 && this->isConnected())
			{
				l_ui32BytesLeft -= this->receiveBuffer(p + ui32BufferSize - l_ui32BytesLeft, l_ui32BytesLeft);

				if (this->isErrorRaised()) { return false; }
			}

			return l_ui32BytesLeft == 0;
		}

		bool isConnected() const override
		{
#if defined TARGET_OS_Windows

			return m_oSocket != INVALID_SOCKET;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return false;
#endif
		}

		void release() override { delete this; }

		bool connect(unsigned long long u64BluetoothAddress) override
		{
			m_sLastError.clear();

			if (this->isConnected())
			{
				m_sLastError = "Bluetooth device is already connected";
				return false;
			}

#if defined TARGET_OS_Windows

			if (!this->initialize()) { return false; }

			m_oSocket = _WINSOCK2API_::socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);

			if (m_oSocket == INVALID_SOCKET)
			{
				m_sLastError = "Failed to create bluetooth socket: " + getLastErrorFormated();
				_WINSOCK2API_::WSACleanup();
				return false;
			}

			SOCKADDR_BTH l_oSockAddressBlutoothServer;
			l_oSockAddressBlutoothServer.btAddr         = u64BluetoothAddress;
			l_oSockAddressBlutoothServer.addressFamily  = AF_BTH;
			l_oSockAddressBlutoothServer.serviceClassId = RFCOMM_PROTOCOL_UUID;
			l_oSockAddressBlutoothServer.port           = BT_PORT_ANY;

			if (_WINSOCK2API_::connect(m_oSocket, reinterpret_cast<SOCKADDR*>(&l_oSockAddressBlutoothServer), sizeof(SOCKADDR_BTH)) == SOCKET_ERROR)
			{
				m_sLastError = "Failed to connect the socket to the bluetooth address [" + std::to_string(l_oSockAddressBlutoothServer.btAddr) + "]: " + getLastErrorFormated();

				_WINSOCK2API_::closesocket(m_oSocket); // Returned code not checked.
				_WINSOCK2API_::WSACleanup(); // Returned code not checked.

				m_oSocket = INVALID_SOCKET;
				return false;
			}

			return true;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return false;
#endif
		}

		bool isErrorRaised() { return !m_sLastError.empty(); }

		const char* getLastError() const override { return m_sLastError.c_str(); }

		std::string getLastErrorFormated()
		{
#if defined TARGET_OS_Windows

			LPTSTR l_sErrorText;
			DWORD l_ui64Error = GetLastError();

			size_t ui32Size = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | // use system message tables to retrieve error text
											FORMAT_MESSAGE_ALLOCATE_BUFFER |  // allocate buffer on local heap for error text
											FORMAT_MESSAGE_IGNORE_INSERTS, // Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters
											nullptr, // unused with FORMAT_MESSAGE_FROM_SYSTEM
											l_ui64Error, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), 
											LPTSTR(&l_sErrorText), // output
											0, // minimum size for output buffer
											nullptr);

			// Converts std::wstring to std::string and returns it. 
			std::wstring l_sErrorMessage(l_sErrorText, ui32Size);
			LocalFree(l_sErrorText);
			std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> l_oConverter;
			return l_oConverter.to_bytes(l_sErrorMessage);

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return "";
#endif
		}

		void clearError() override { m_sLastError.clear(); }

		bool listPairedBluetoothDevices(unsigned int* pairedBluetoothDevicesCount, char** strarray, unsigned long long** bluetoothAddresses) override
		{
			std::vector<std::string> bluetoothDevicesName;
			std::vector<unsigned long long> bluetoothDevicesAddress;

#if defined TARGET_OS_Windows

			HANDLE l_pHandle;
			WSAQUERYSET l_oWSAQuerySet;

			memset((void *)&l_oWSAQuerySet, 0, sizeof(l_oWSAQuerySet));
			l_oWSAQuerySet.dwSize      = sizeof(l_oWSAQuerySet);
			l_oWSAQuerySet.dwNameSpace = NS_BTH;
			l_oWSAQuerySet.lpcsaBuffer = nullptr;

			if (_WINSOCK2API_::WSALookupServiceBegin(&l_oWSAQuerySet, LUP_CONTAINERS | LUP_RETURN_NAME | LUP_RETURN_ADDR, &l_pHandle) == SOCKET_ERROR)
			{
				m_sLastError = "Failed to start the Bluetooth lookup service: " + getLastErrorFormated();
				return false;
			}

			char l_vBuffer[5000];
			const LPWSAQUERYSET l_sWSAQuerySet = LPWSAQUERYSET(l_vBuffer);
			DWORD l_ui32dwSize           = sizeof(l_vBuffer);

			memset((void *)l_sWSAQuerySet, 0, sizeof(WSAQUERYSET));
			l_sWSAQuerySet->dwSize      = sizeof(WSAQUERYSET);
			l_sWSAQuerySet->dwNameSpace = NS_BTH;
			l_sWSAQuerySet->lpBlob      = nullptr;

			bool l_bLookup = true;

			while (l_bLookup)
			{
				// Check next bluetooth device
				const int l_i32Result = _WINSOCK2API_::WSALookupServiceNext(l_pHandle, LUP_RETURN_NAME | LUP_RETURN_ADDR, &l_ui32dwSize, l_sWSAQuerySet);

				if (l_i32Result == SOCKET_ERROR)
				{
					// If it is a "real" error, we trace it and return false.
					if (_WINSOCK2API_::WSAGetLastError() != WSA_E_NO_MORE)
					{
						m_sLastError = "Lookup service next operation failed: " + getLastErrorFormated();
						return false;
					}

					// Else, it is because there is no more Bluetooth devices available.
					l_bLookup = false;
					break;
				}
				// Get bluetooth MAC address and name
				bluetoothDevicesAddress.push_back(reinterpret_cast<SOCKADDR_BTH *>(l_sWSAQuerySet->lpcsaBuffer->RemoteAddr.lpSockaddr)->btAddr);

				std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converterX;
				bluetoothDevicesName.push_back(converterX.to_bytes(l_sWSAQuerySet->lpszServiceInstanceName));
			}

			if (_WINSOCK2API_::WSALookupServiceEnd(l_pHandle) == SOCKET_ERROR)
			{
				m_sLastError = "Failed to stop the Bluetooth lookup service: " + getLastErrorFormated();
				return false;
			}

			*pairedBluetoothDevicesCount = static_cast<unsigned int>(bluetoothDevicesAddress.size());
			strarray                     = new char*[*pairedBluetoothDevicesCount];

			for (unsigned int i = 0; i < *pairedBluetoothDevicesCount; ++i)
			{
				strarray[i] = new char[bluetoothDevicesName[i].size() + 1];
				std::strcpy(strarray[i], bluetoothDevicesName[i].c_str());
			}

			bluetoothAddresses = new unsigned long long*[*pairedBluetoothDevicesCount];

			return true;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return false;
#endif
		}

		std::string m_sLastError;

#if defined TARGET_OS_Windows

		SOCKET m_oSocket;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

#endif
	};

	IConnectionBluetooth* createConnectionBluetooth() { return new CConnectionBluetooth(); }
} // namespace Socket
