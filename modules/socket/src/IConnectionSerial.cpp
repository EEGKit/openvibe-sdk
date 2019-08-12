#include "IConnectionSerial.h"

#if defined TARGET_OS_Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
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

#if defined TARGET_OS_Linux
// Good resources at http://www.tldp.org/HOWTO/html_single/Serial-HOWTO/
#endif

namespace Socket
{
	class CConnectionSerial : public IConnectionSerial
	{
	public:

		CConnectionSerial()
			:
#if defined TARGET_OS_Windows
			  m_pFile(nullptr)
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			m_iFile(0)
#endif
		{ }

		bool open() override
		{
			// Should never be used
			return false;
		}

		bool close() override
		{
#if defined TARGET_OS_Windows

			if (m_pFile != nullptr)
			{
				if (!CloseHandle(m_pFile))
				{
					m_sLastError = "Failed to close the serial port:" + this->getLastErrorFormated();
					m_pFile      = nullptr;
					return false;
				}

				m_pFile = nullptr;
			}

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			if(m_iFile != -1)
			{
				::close(m_iFile);
				m_iFile=-1;
			}

#endif

			return true;
		}

		bool isReadyToSend(const uint32_t ui32TimeOut) const override
		{
			if (!this->isConnected()) { return false; }

#if defined TARGET_OS_Windows

			return true;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			fd_set  l_oOutputFileDescriptorSet;
			struct timeval l_oTimeout;
			l_oTimeout.tv_sec=ui32TimeOut/1000;
			l_oTimeout.tv_usec=(ui32TimeOut%1000)*1000;

			FD_ZERO(&l_oOutputFileDescriptorSet);
			FD_SET(m_iFile, &l_oOutputFileDescriptorSet);

			if(!::select(m_iFile+1, nullptr, &l_oOutputFileDescriptorSet, nullptr, &l_oTimeout)) { return false; }

			if(FD_ISSET(m_iFile, &l_oOutputFileDescriptorSet)) { return true; }

#endif

			return false;
		}

		bool isReadyToReceive(const uint32_t ui32TimeOut) const override
		{
			if (!this->isConnected()) { return false; }

#if defined TARGET_OS_Windows

			struct _COMSTAT l_oStatus;
			DWORD l_dwState;

			if (ClearCommError(m_pFile, &l_dwState, &l_oStatus) != 0)
			{
				return l_oStatus.cbInQue != 0;
			}
			return false;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			fd_set  l_oInputFileDescriptorSet;
			struct timeval l_oTimeout;
			l_oTimeout.tv_sec=ui32TimeOut/1000;
			l_oTimeout.tv_usec=(ui32TimeOut%1000)*1000;

			FD_ZERO(&l_oInputFileDescriptorSet);
			FD_SET(m_iFile, &l_oInputFileDescriptorSet);

			if(!::select(m_iFile+1, &l_oInputFileDescriptorSet, nullptr, nullptr, &l_oTimeout)) { return false; }

			if(FD_ISSET(m_iFile, &l_oInputFileDescriptorSet)) { return true; }

#endif

			return false;
		}

		uint32_t getPendingByteCount() override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Serial port not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows

			struct _COMSTAT l_oStatus;
			DWORD l_dwState;

			if (ClearCommError(m_pFile, &l_dwState, &l_oStatus) == 0)
			{
				m_sLastError = "Failed to clear the serial port communication error: " + this->getLastErrorFormated();
				return 0;
			}
			return l_oStatus.cbInQue;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			int l_iByteCount=0;
			if(-1 == ::ioctl(m_iFile, FIONREAD, &l_iByteCount))
			{
				m_sLastError = "Failed to querry pending bytes in ioctl";
				return 0;
			}
			return l_iByteCount;

#endif
		}

		bool flush() override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Serial port not connected.";
				return false;
			}

#if defined TARGET_OS_Windows

			if (!FlushFileBuffers(m_pFile))
			{
				m_sLastError = "Failed to flush serial port buffer: " + this->getLastErrorFormated();
				return false;
			}
			return true;

#elif defined TARGET_OS_Android
			return false;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			if(-1 == ::tcdrain(m_iFile))
			{
				m_sLastError = "Could not flush connection in tcdrain";
				return false;
			}
			return true;

#endif

			return false;
		}

		uint32_t sendBuffer(const void* pBuffer, const uint32_t ui32BufferSize) override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Serial port not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows
			DWORD l_dwWritten = 0;

			if (!WriteFile(m_pFile, pBuffer, ui32BufferSize, &l_dwWritten, nullptr))
			{
				m_sLastError = "Failed to write on serial port: " + this->getLastErrorFormated();
				this->close();
				return 0;
			}

			if (l_dwWritten == 0)
			{
				m_sLastError = "Serial port timeout when trying to write.";
				this->close();
				return 0;
			}

			return l_dwWritten;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			int l_iResult = ::write(m_iFile, pBuffer, ui32BufferSize);
			if(l_iResult < 0)
			{
				m_sLastError = "Could not write on connection";
				this->close();
				return 0;
			}

			return l_iResult;

#endif

			return 0;
		}

		uint32_t receiveBuffer(void* pBuffer, const uint32_t ui32BufferSize) override
		{
			if (!this->isConnected())
			{
				m_sLastError = "Serial port not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows

			DWORD l_dwRead = 0;

			if (!ReadFile(m_pFile, pBuffer, ui32BufferSize, &l_dwRead, nullptr))
			{
				m_sLastError = "Failed to read on serial port: " + this->getLastErrorFormated();
				this->close();
				return 0;
			}

			if (l_dwRead == 0)
			{
				m_sLastError = "Serial port timeout when trying to read.";
				this->close();
				return 0;
			}

			return l_dwRead;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			int l_iResult = ::read(m_iFile, pBuffer, ui32BufferSize);
			if (l_iResult < 0)
			{
				m_sLastError = "Could not read from connection";
				this->close();
				return 0;
			}

			return l_iResult;

#endif

			return 0;
		}

		bool sendBufferBlocking(const void* pBuffer, const uint32_t ui32BufferSize) override
		{
			const char* l_pPointer   = reinterpret_cast<const char*>(pBuffer);
			uint32_t l_ui32BytesLeft = ui32BufferSize;

			while (l_ui32BytesLeft != 0 && this->isConnected())
			{
				l_ui32BytesLeft -= this->sendBuffer(l_pPointer + ui32BufferSize - l_ui32BytesLeft, l_ui32BytesLeft);

				if (this->isErrorRaised()) { return false; }
			}

			return l_ui32BytesLeft == 0;
		}

		bool receiveBufferBlocking(void* pBuffer, const uint32_t ui32BufferSize) override
		{
			char* l_pPointer         = reinterpret_cast<char*>(pBuffer);
			uint32_t l_ui32BytesLeft = ui32BufferSize;

			while (l_ui32BytesLeft != 0 && this->isConnected())
			{
				l_ui32BytesLeft -= this->receiveBuffer(l_pPointer + ui32BufferSize - l_ui32BytesLeft, l_ui32BytesLeft);

				if (this->isErrorRaised()) { return false; }
			}

			return l_ui32BytesLeft == 0;
		}

		bool isConnected() const override
		{
#if defined TARGET_OS_Windows

			return ((m_pFile != nullptr) && (m_pFile != INVALID_HANDLE_VALUE));

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			return m_iFile != 0;

#endif
		}

		void release() override
		{
			delete this;
		}

		bool connect(const char* sURL, unsigned long ul32BaudRate) override
		{
			m_sLastError.clear();

			if (this->isConnected())
			{
				m_sLastError = "Serial port already connected";
				return false;
			}

#if defined TARGET_OS_Windows

			m_pFile = ::CreateFile(sURL, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (m_pFile == INVALID_HANDLE_VALUE || m_pFile == nullptr)
			{
				m_sLastError = "Failed to open serial port: " + this->getLastErrorFormated();
				m_pFile      = nullptr;
				return false;
			}

			DCB l_oDCB = { 0 };

			if (!GetCommState(m_pFile, &l_oDCB))
			{
				m_sLastError = "Failed to get communication state: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

			l_oDCB.DCBlength = sizeof(l_oDCB);
			l_oDCB.BaudRate  = ul32BaudRate;
			l_oDCB.ByteSize  = 8;
			l_oDCB.Parity    = NOPARITY;
			l_oDCB.StopBits  = ONESTOPBIT;

			ClearCommError(m_pFile, nullptr, nullptr);

			if (!SetCommState(m_pFile, &l_oDCB))
			{
				m_sLastError = "Could not set communication state: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			/* $$$ BAUD RATE PARAMETER IS NOT CONSIDERED ON LINUX, MIGHT BE NECESSARY TO CHANGE THIS $$$ */
			assert(ul32BaudRate == 230400);
			if ((m_iFile = ::open(sURL, O_RDWR)) == -1)
			{
				m_sLastError = "Failed to open serial port.";
				return false;
			}

			struct termios l_oTerminalAttributes;
			if(::tcgetattr(m_iFile, &l_oTerminalAttributes)!=0)
			{
				m_sLastError = "Could not get terminal attributes in tcgetattr";
				this->close();
				return false;
			}

			l_oTerminalAttributes.c_cflag = B230400 | CS8 | CLOCAL | CREAD;
			l_oTerminalAttributes.c_iflag = 0;
			l_oTerminalAttributes.c_oflag = OPOST | ONLCR;
			l_oTerminalAttributes.c_lflag = 0;

			if (::tcsetattr(m_iFile, TCSAFLUSH, &l_oTerminalAttributes) != 0)
			{
				m_sLastError = "Could not set terminal attributes in tcgetattr";
				this->close();
				return false;
			}

#endif

			return true;
		}

		bool setTimeouts(unsigned long ui32DecisecondsTimeout) override
		{
			if (!this->isConnected()) { return false; }

#if defined TARGET_OS_Windows

			COMMTIMEOUTS l_Timeouts;

			if (!GetCommTimeouts(m_pFile, &l_Timeouts))
			{
				m_sLastError = "Could not get communication timeouts: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

			l_Timeouts.ReadTotalTimeoutConstant  = ui32DecisecondsTimeout * 100; // Deciseconds to milliseconds
			l_Timeouts.WriteTotalTimeoutConstant = ui32DecisecondsTimeout * 100; // Deciseconds to milliseconds

			if (!SetCommTimeouts(m_pFile, &l_Timeouts))
			{
				m_sLastError = "Could not set communication timeouts: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			struct termios l_oTerminalAttributes;
			if(::tcgetattr(m_iFile, &l_oTerminalAttributes)!=0)
			{
				m_sLastError = "Could not get terminal attributes in tcgetattr";
				this->close();
				return false;
			}

			l_oTerminalAttributes.c_cc[VTIME] = ui32DecisecondsTimeout;

			if (::tcsetattr(m_iFile, TCSAFLUSH, &l_oTerminalAttributes) != 0)
			{
				m_sLastError = "Could not set terminal attributes in tcgetattr";
				this->close();
				return false;
			}

#endif
			return true;
		}

		const char* getLastError() override
		{
			return m_sLastError.c_str();
		}

		std::string getLastErrorFormated()
		{
#if defined TARGET_OS_Windows
			LPTSTR l_sErrorText;
			DWORD l_ui64Error = GetLastError();
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |                  // use system message tables to retrieve error text
						  FORMAT_MESSAGE_ALLOCATE_BUFFER |              // allocate buffer on local heap for error text
						  FORMAT_MESSAGE_IGNORE_INSERTS,               // Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters
						  nullptr,                                        // unused with FORMAT_MESSAGE_FROM_SYSTEM
						  l_ui64Error,
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  (LPTSTR)&l_sErrorText,                       // output
						  0,                                           // minimum size for output buffer
						  nullptr);                                       // arguments - see note
			return l_sErrorText;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return ""; // TODO
#endif
		}

		bool isErrorRaised() override
		{
			return !m_sLastError.empty();
		}

		void clearError() override
		{
			m_sLastError.clear();
		}

		std::string m_sLastError;

#if defined TARGET_OS_Windows
		void* m_pFile;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		int m_iFile;
#endif
	};

	IConnectionSerial* createConnectionSerial() { return new CConnectionSerial(); }
};
