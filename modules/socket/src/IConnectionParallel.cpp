#include "IConnectionParallel.h"

#if defined TARGET_OS_Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#include <sys/ioctl.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/errno.h>
#else
#error "Unsupported platform"
#endif

#if defined TARGET_OS_Linux && !defined TARGET_OS_Android
#include <linux/ppdev.h>
#include <linux/parport.h>
#endif

#include <assert.h>

namespace Socket
{
	class CConnectionParallel : public IConnectionParallel
	{
	protected:
		unsigned short m_ui16PortNumber;
		std::string m_sLastError;
	public:

		CConnectionParallel()

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			:m_iFile(0)
#endif
		{
#if defined TARGET_OS_Windows

			m_ui16PortNumber = 0;
			m_hmodTVicPort   = LoadLibrary(TEXT("TVicPort.dll"));

			if (m_hmodTVicPort != nullptr)
			{
				m_lpfnTVicIsDriverOpened = LPFNTVICPORTISDRIVEROPENED(GetProcAddress(m_hmodTVicPort, "IsDriverOpened"));
				m_lpfnTVicPortOpen       = LPFNTVICPORTOPEN(GetProcAddress(m_hmodTVicPort, "OpenTVicPort"));
				m_lpfnTVicPortClose      = LPFNTVICPORTCLOSE(GetProcAddress(m_hmodTVicPort, "CloseTVicPort"));
				m_lpfnTVicPortWrite      = LPFNTVICPORTWRITE(GetProcAddress(m_hmodTVicPort, "WritePort"));

				if (!m_lpfnTVicIsDriverOpened || !m_lpfnTVicPortOpen || !m_lpfnTVicPortClose || !m_lpfnTVicPortWrite)
				{
					m_sLastError = "Cannot load function from TVicPort.dll: " + this->getLastErrorFormated();
					this->release();
				}
			}
			else
			{
				m_sLastError = "Cannot found or open TVicPort.dll: " + this->getLastErrorFormated();
			}
#endif
		}

		bool open() override
		{
			// Should never be used
			return false;
		}

		bool close() override
		{
#if defined TARGET_OS_Windows

			if (m_hmodTVicPort != nullptr)
			{
				if (m_lpfnTVicIsDriverOpened())
				{
					m_ui16PortNumber = 0;
					m_lpfnTVicPortClose();
					return true;
				}
				m_sLastError = "Cannot close the TVicPort library because it is not opened.";
				return false;
			}
			return false;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			return false;

			/*if (ioctl(fd, PPRELEASE) < 0)  { return false; }

			if (close(m_iFile) < 0) { return false; }
			else
			{
				m_iFile = -1;
			}*/
#endif
		}

		bool isReadyToSend(const uint32_t ui32TimeOut = 0) const override { return this->isConnected(); }

		bool isReadyToReceive(const uint32_t ui32TimeOut = 0) const override { return this->isConnected(); }

		uint32_t getPendingByteCount() { return (this->isConnected() ? 0 : 1); }

		uint32_t sendBuffer(const void* pBuffer, const uint32_t ui32BufferSize = 8) override
		{
			if (!this->isConnected()) { return 0; }

#if defined TARGET_OS_Windows
			uint8_t l_ui8Value = *(static_cast<const uint8_t*>(pBuffer));

			m_lpfnTVicPortWrite(m_ui16PortNumber, l_ui8Value);
			return ui32BufferSize;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			return 0;

			/*if (ioctl(m_iFile, PPWDATA, &l_ui8Value) < 0) { return ui32BufferSize; }*/

#endif
		}

		uint32_t receiveBuffer(void* pBuffer, const uint32_t ui32BufferSize = 8) override
		{
			if (!this->isConnected()) { return 0; }

#if defined TARGET_OS_Windows


#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			/*unsigned char valin;
			int dirin=1;
			int dirout=0;

			if (ioctl (fd, PPDATADIR, &dirin)  < 0) 
			{
				
				return 0;
			}

			if (ioctl(fd, PPRDATA, &valin) < 0) {
				return 0;
			}

			int l_iResult = ::read(m_iFile, pBuffer, ui32BufferSize);
			if(l_iResult < 0)
			{
				this->close();
				return 0;
			}

			return l_iResult;*/

#endif

			return 0;
		}

		bool sendBufferBlocking(const void* pBuffer, const uint32_t ui32BufferSize) override
		{
			const char* p   = reinterpret_cast<const char*>(pBuffer);
			uint32_t l_ui32BytesLeft = ui32BufferSize;

			while (l_ui32BytesLeft != 0 && this->isConnected())
			{
				l_ui32BytesLeft -= this->sendBuffer(p + ui32BufferSize - l_ui32BytesLeft, l_ui32BytesLeft);
			}

			return this->isConnected();
		}

		bool receiveBufferBlocking(void* pBuffer, const uint32_t ui32BufferSize) override
		{
			char* p         = reinterpret_cast<char*>(pBuffer);
			uint32_t l_ui32BytesLeft = ui32BufferSize;

			while (l_ui32BytesLeft != 0 && this->isConnected())
			{
				l_ui32BytesLeft -= this->receiveBuffer(p + ui32BufferSize - l_ui32BytesLeft, l_ui32BytesLeft);
			}

			return this->isConnected();
		}

#if defined TARGET_OS_Windows
		bool isConnected() const override { return m_ui16PortNumber != 0; }
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		bool isConnected() const override { return m_iFile != 0; }
#endif

		void release() override
		{
#if defined TARGET_OS_Windows
			if (m_hmodTVicPort != nullptr && !FreeLibrary(m_hmodTVicPort)) { m_sLastError = getLastErrorFormated(); }
#endif
			delete this;
		}

		bool connect(unsigned short ui16PortNumber) override
		{
			if (this->isConnected()) { return false; }

#if defined TARGET_OS_Windows
			if (m_hmodTVicPort != nullptr)
			{
				if (m_lpfnTVicPortOpen())
				{
					m_sLastError     = "No error";
					m_ui16PortNumber = ui16PortNumber;
					return true;
				}
				m_sLastError     = "Cannot open the TVic library";
				m_ui16PortNumber = 0;
				return false;
			}
			m_sLastError = "TVicPort library is not loaded.";
			return false;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

			return false;

			/*std::string l_sUrl = "/dev/parport" + std::to_string(ui16PortNumber);

			if ((m_iFile = open(l_sUrl.c_str() , O_RDWR)) < 0) 
			{
				this->close();
				return false;
			}
			else
			{
				if (ioctl(m_iFile, PPCLAIM) < 0)
				{
					this->close();
					return false;
				}
			}*/

#endif
		}

		std::string getLastError() override {return m_sLastError;}

		std::string getLastErrorFormated()
		{
#if defined TARGET_OS_Windows

			LPTSTR errorText;
			const DWORD error = GetLastError();

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM					// use system message tables to retrieve error text
						  | FORMAT_MESSAGE_ALLOCATE_BUFFER				// allocate buffer on local heap for error text
						  | FORMAT_MESSAGE_IGNORE_INSERTS,				// Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters 
						  nullptr,											// unused with FORMAT_MESSAGE_FROM_SYSTEM
						  error,
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  LPTSTR(&errorText),						// output 
						  0,											// minimum size for output buffer
						  nullptr);										// arguments - see note

			return errorText + std::to_string(static_cast<unsigned long long>(error));

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return "Not implemented.";
#endif
		}

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	int m_iFile;
#endif
	};

	IConnectionParallel* createConnectionParallel() { return new CConnectionParallel(); }
} // namespace Socket
