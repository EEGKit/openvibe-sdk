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
	class CConnectionSerial final : public IConnectionSerial
	{
	public:

#if defined TARGET_OS_Windows
		CConnectionSerial() : m_pFile(nullptr) { }
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		CConnectionSerial() : m_iFile(0) { }
#endif


		bool open() override { return false; }	// Should never be used

		bool close() override
		{
#if defined TARGET_OS_Windows

			if (m_pFile != nullptr)
			{
				if (!CloseHandle(m_pFile))
				{
					m_LastError = "Failed to close the serial port:" + this->getLastErrorFormated();
					m_pFile     = nullptr;
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


#if defined TARGET_OS_Windows

		bool isReadyToSend(const size_t /*timeOut*/) const override
		{
			if (!this->isConnected()) { return false; }
			return true;
		}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

		bool isReadyToSend(const size_t timeOut) const override
		{
			if (!this->isConnected()) { return false; }
			fd_set  oFileDescSet;
			struct timeval l_oTimeout;
			l_oTimeout.tv_sec=timeOut/1000;
			l_oTimeout.tv_usec=(timeOut%1000)*1000;

			FD_ZERO(&oFileDescSet);
			FD_SET(m_iFile, &oFileDescSet);

			if(!::select(m_iFile+1, nullptr, &oFileDescSet, nullptr, &l_oTimeout)) { return false; }

			if(FD_ISSET(m_iFile, &oFileDescSet)) { return true; }
			return false;
	}

#endif

#if defined TARGET_OS_Windows
		bool isReadyToReceive(const size_t /*timeOut*/) const override
		{
			if (!this->isConnected()) { return false; }
			struct _COMSTAT status;
			DWORD state;

			if (ClearCommError(m_pFile, &state, &status) != 0) { return status.cbInQue != 0; }
			return false;
		}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		bool isReadyToReceive(const size_t timeOut) const override
		{
			if (!this->isConnected()) { return false; }
			fd_set  iFileDescSet;
			struct timeval l_oTimeout;
			l_oTimeout.tv_sec=timeOut/1000;
			l_oTimeout.tv_usec=(timeOut%1000)*1000;

			FD_ZERO(&iFileDescSet);
			FD_SET(m_iFile, &iFileDescSet);

			if(!::select(m_iFile+1, &iFileDescSet, nullptr, nullptr, &l_oTimeout)) { return false; }

			if(FD_ISSET(m_iFile, &iFileDescSet)) { return true; }
			return false;
		}
#endif

		size_t getPendingByteCount() override
		{
			if (!this->isConnected())
			{
				m_LastError = "Serial port not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows

			struct _COMSTAT status;
			DWORD state;

			if (ClearCommError(m_pFile, &state, &status) == 0)
			{
				m_LastError = "Failed to clear the serial port communication error: " + this->getLastErrorFormated();
				return 0;
			}
			return status.cbInQue;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			int l_iByteCount=0;
			if(-1 == ::ioctl(m_iFile, FIONREAD, &l_iByteCount))
			{
				m_LastError = "Failed to querry pending bytes in ioctl";
				return 0;
			}
			return l_iByteCount;

#endif
		}

		bool flush() override
		{
			if (!this->isConnected())
			{
				m_LastError = "Serial port not connected.";
				return false;
			}

#if defined TARGET_OS_Windows

			if (!FlushFileBuffers(m_pFile))
			{
				m_LastError = "Failed to flush serial port buffer: " + this->getLastErrorFormated();
				return false;
			}
			return true;

#elif defined TARGET_OS_Android
			return false;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			if(-1 == ::tcdrain(m_iFile))
			{
				m_LastError = "Could not flush connection in tcdrain";
				return false;
			}
			return true;
#endif
			return false;
		}

		size_t sendBuffer(const void* buffer, const size_t size) override
		{
			if (!this->isConnected())
			{
				m_LastError = "Serial port not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows
			DWORD l_dwWritten = 0;

			if (!WriteFile(m_pFile, buffer, DWORD(size), &l_dwWritten, nullptr))
			{
				m_LastError = "Failed to write on serial port: " + this->getLastErrorFormated();
				this->close();
				return 0;
			}

			if (l_dwWritten == 0)
			{
				m_LastError = "Serial port timeout when trying to write.";
				this->close();
				return 0;
			}

			return l_dwWritten;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			int res = ::write(m_iFile, buffer, size);
			if(res < 0)
			{
				m_LastError = "Could not write on connection";
				this->close();
				return 0;
			}

			return res;
#endif
			return 0;
		}

		size_t receiveBuffer(void* buffer, const size_t size) override
		{
			if (!this->isConnected())
			{
				m_LastError = "Serial port not connected.";
				return 0;
			}

#if defined TARGET_OS_Windows

			DWORD l_dwRead = 0;

			if (!ReadFile(m_pFile, buffer, DWORD(size), &l_dwRead, nullptr))
			{
				m_LastError = "Failed to read on serial port: " + this->getLastErrorFormated();
				this->close();
				return 0;
			}

			if (l_dwRead == 0)
			{
				m_LastError = "Serial port timeout when trying to read.";
				this->close();
				return 0;
			}

			return l_dwRead;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			int res = ::read(m_iFile, buffer, size);
			if (res < 0)
			{
				m_LastError = "Could not read from connection";
				this->close();
				return 0;
			}

			return res;
#endif
			return 0;
		}

		bool sendBufferBlocking(const void* buffer, const size_t size) override
		{
			const char* p    = reinterpret_cast<const char*>(buffer);
			size_t bytesLeft = size;

			while (bytesLeft != 0 && this->isConnected())
			{
				bytesLeft -= this->sendBuffer(p + size - bytesLeft, bytesLeft);
				if (this->isErrorRaised()) { return false; }
			}

			return bytesLeft == 0;
		}

		bool receiveBufferBlocking(void* buffer, const size_t size) override
		{
			char* p          = reinterpret_cast<char*>(buffer);
			size_t bytesLeft = size;

			while (bytesLeft != 0 && this->isConnected())
			{
				bytesLeft -= this->receiveBuffer(p + size - bytesLeft, bytesLeft);
				if (this->isErrorRaised()) { return false; }
			}
			return bytesLeft == 0;
		}

		bool isConnected() const override
		{
#if defined TARGET_OS_Windows
			return ((m_pFile != nullptr) && (m_pFile != INVALID_HANDLE_VALUE));
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return m_iFile != 0;
#endif
		}

		void release() override { delete this; }

		bool connect(const char* sURL, const size_t baudrate) override
		{
			m_LastError.clear();

			if (this->isConnected())
			{
				m_LastError = "Serial port already connected";
				return false;
			}

#if defined TARGET_OS_Windows

			m_pFile = ::CreateFile(sURL, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

			if (m_pFile == INVALID_HANDLE_VALUE || m_pFile == nullptr)
			{
				m_LastError = "Failed to open serial port: " + this->getLastErrorFormated();
				m_pFile     = nullptr;
				return false;
			}

			DCB dcb = { 0 };

			if (!GetCommState(m_pFile, &dcb))
			{
				m_LastError = "Failed to get communication state: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

			dcb.DCBlength = sizeof(dcb);
			dcb.BaudRate  = DWORD(baudrate);
			dcb.ByteSize  = 8;
			dcb.Parity    = NOPARITY;
			dcb.StopBits  = ONESTOPBIT;

			ClearCommError(m_pFile, nullptr, nullptr);

			if (!SetCommState(m_pFile, &dcb))
			{
				m_LastError = "Could not set communication state: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			/* $$$ BAUD RATE PARAMETER IS NOT CONSIDERED ON LINUX, MIGHT BE NECESSARY TO CHANGE THIS $$$ */
			assert(baudrate == 230400);
			if ((m_iFile = ::open(sURL, O_RDWR)) == -1)
			{
				m_LastError = "Failed to open serial port.";
				return false;
			}

			struct termios l_oTerminalAttributes;
			if(::tcgetattr(m_iFile, &l_oTerminalAttributes)!=0)
			{
				m_LastError = "Could not get terminal attributes in tcgetattr";
				this->close();
				return false;
			}

			l_oTerminalAttributes.c_cflag = B230400 | CS8 | CLOCAL | CREAD;
			l_oTerminalAttributes.c_iflag = 0;
			l_oTerminalAttributes.c_oflag = OPOST | ONLCR;
			l_oTerminalAttributes.c_lflag = 0;

			if (::tcsetattr(m_iFile, TCSAFLUSH, &l_oTerminalAttributes) != 0)
			{
				m_LastError = "Could not set terminal attributes in tcgetattr";
				this->close();
				return false;
			}
#endif
			return true;
		}

		bool setTimeouts(const size_t decisecondsTimeout) override
		{
			if (!this->isConnected()) { return false; }

#if defined TARGET_OS_Windows

			COMMTIMEOUTS l_Timeouts;

			if (!GetCommTimeouts(m_pFile, &l_Timeouts))
			{
				m_LastError = "Could not get communication timeouts: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

			l_Timeouts.ReadTotalTimeoutConstant  = decisecondsTimeout * 100; // Deciseconds to milliseconds
			l_Timeouts.WriteTotalTimeoutConstant = decisecondsTimeout * 100; // Deciseconds to milliseconds

			if (!SetCommTimeouts(m_pFile, &l_Timeouts))
			{
				m_LastError = "Could not set communication timeouts: " + this->getLastErrorFormated();
				this->close();
				return false;
			}

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			struct termios l_oTerminalAttributes;
			if(::tcgetattr(m_iFile, &l_oTerminalAttributes)!=0)
			{
				m_LastError = "Could not get terminal attributes in tcgetattr";
				this->close();
				return false;
			}

			l_oTerminalAttributes.c_cc[VTIME] = decisecondsTimeout;

			if (::tcsetattr(m_iFile, TCSAFLUSH, &l_oTerminalAttributes) != 0)
			{
				m_LastError = "Could not set terminal attributes in tcgetattr";
				this->close();
				return false;
			}

#endif
			return true;
		}

		const char* getLastError() override { return m_LastError.c_str(); }

		std::string getLastErrorFormated()
		{
#if defined TARGET_OS_Windows
			LPTSTR l_sErrorText;
			const DWORD error = GetLastError();
			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |                  // use system message tables to retrieve error text
						  FORMAT_MESSAGE_ALLOCATE_BUFFER |              // allocate buffer on local heap for error text
						  FORMAT_MESSAGE_IGNORE_INSERTS,               // Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters
						  nullptr,                                        // unused with FORMAT_MESSAGE_FROM_SYSTEM
						  error,
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  LPTSTR(&l_sErrorText),                       // output
						  0,                                           // minimum size for output buffer
						  nullptr);                                       // arguments - see note
			return l_sErrorText;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return ""; // TODO
#endif
		}

		bool isErrorRaised() override { return !m_LastError.empty(); }
		void clearError() override { m_LastError.clear(); }

		std::string m_LastError;

#if defined TARGET_OS_Windows
		void* m_pFile;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		int m_iFile;
#endif
	};

	IConnectionSerial* createConnectionSerial() { return new CConnectionSerial(); }
} // namespace Socket
