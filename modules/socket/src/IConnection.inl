#include "IConnection.h"

#include <iostream>

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <sys/select.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 // #include <netinet/in.h>
 // #include <netinet/tcp.h>
 #include <arpa/inet.h>
 #include <unistd.h>
 // #include <netdb.h>
 #include <ctime>

#if defined TARGET_OS_MacOS
  #define Socket_SendFlags    0
  #define Socket_ReceiveFlags 0
#else
  #define Socket_SendFlags    MSG_NOSIGNAL
  #define Socket_ReceiveFlags MSG_NOSIGNAL
#endif
#elif defined TARGET_OS_Windows
#include <WinSock2.h>
#include <Windows.h>

#define Socket_SendFlags    0
#define Socket_ReceiveFlags 0
#else

#endif

namespace Socket
{
	static bool FD_ISSET_PROXY(int fd, fd_set* set) { return FD_ISSET(fd, set) ? true : false; }

	template <class T>
	class TConnection : public T
	{
	public:

		TConnection() : m_i32Socket(-1)
		{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#elif defined TARGET_OS_Windows
			int32_t l_i32VersionHigh = 2;
			int32_t l_i32VersionLow  = 0;
			WORD l_oWinsockVersion   = MAKEWORD(l_i32VersionHigh, l_i32VersionLow);
			WSADATA l_oWSAData;
			WSAStartup(l_oWinsockVersion, &l_oWSAData);
#else
#endif
		}

		explicit TConnection(int32_t i32Socket) : m_i32Socket(i32Socket)
		{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#elif defined TARGET_OS_Windows
			int32_t l_i32VersionHigh = 2;
			int32_t l_i32VersionLow  = 0;
			WORD l_oWinsockVersion   = MAKEWORD(l_i32VersionHigh, l_i32VersionLow);
			WSADATA l_oWSAData;
			WSAStartup(l_oWinsockVersion, &l_oWSAData);
#else
#endif
		}

		virtual ~TConnection()
		{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#elif defined TARGET_OS_Windows
			WSACleanup();
#else
#endif
		}

	protected:

		virtual bool open()
		{
			if (isConnected()) { return false; }

			m_i32Socket = static_cast<int32_t>(socket(AF_INET, SOCK_STREAM, 0));
			if (m_i32Socket == -1) { return false; }

			return true;
		}

	public:

		virtual bool close()
		{
			if (!isConnected()) { return false; }

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			::shutdown(m_i32Socket, SHUT_RDWR);
			::close(m_i32Socket);
#elif defined TARGET_OS_Windows
			shutdown(m_i32Socket, SD_BOTH);
			closesocket(m_i32Socket);
#else
#endif

			m_i32Socket = -1;
			return true;
		}

		virtual bool isReadyToSend(uint32_t ui32TimeOut = 0) const
		{
			if (!isConnected()) { return false; }

			struct timeval l_oTimeVal;
			l_oTimeVal.tv_sec  = (ui32TimeOut / 1000);
			l_oTimeVal.tv_usec = ((ui32TimeOut - l_oTimeVal.tv_sec * 1000) * 1000);

			fd_set l_oWriteFileDescriptors;
			FD_ZERO(&l_oWriteFileDescriptors);
			FD_SET(m_i32Socket, &l_oWriteFileDescriptors);

			if (select(m_i32Socket + 1, NULL, &l_oWriteFileDescriptors, NULL, &l_oTimeVal) < 0) { return false; }
			if (!FD_ISSET_PROXY(m_i32Socket, &l_oWriteFileDescriptors)) { return false; }
			return true;
		}

		virtual bool isReadyToReceive(uint32_t ui32TimeOut = 0) const
		{
			if (!isConnected()) { return false; }

			struct timeval l_oTimeVal;
			l_oTimeVal.tv_sec  = (ui32TimeOut / 1000);
			l_oTimeVal.tv_usec = ((ui32TimeOut - l_oTimeVal.tv_sec * 1000) * 1000);

			fd_set l_oReadFileDescriptors;
			FD_ZERO(&l_oReadFileDescriptors);
			FD_SET(m_i32Socket, &l_oReadFileDescriptors);

			if (select(m_i32Socket + 1, &l_oReadFileDescriptors, NULL, NULL, &l_oTimeVal) < 0) { return false; }
			if (!(FD_ISSET_PROXY(m_i32Socket, &l_oReadFileDescriptors))) { return false; }
			return true;
		}

		virtual uint32_t sendBuffer(const void* pBuffer, const uint32_t ui32BufferSize)
		{
			if (!isConnected()) { return 0; }
#if 0
			int l_iTrue=1;
			setsockopt(m_i32Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&l_iTrue, sizeof(l_iTrue));
#endif
			int l_iResult = send(m_i32Socket, static_cast<const char*>(pBuffer), ui32BufferSize, Socket_SendFlags);
			if (ui32BufferSize != 0 && l_iResult <= 0)
			{
				close();
			}
			return l_iResult <= 0 ? 0 : (uint32_t)l_iResult;
		}

		virtual uint32_t receiveBuffer(void* pBuffer, const uint32_t ui32BufferSize)
		{
			if (!isConnected() || !ui32BufferSize) { return 0; }
#if 0
			int l_iTrue=1;
			setsockopt(m_i32Socket, IPPROTO_TCP, TCP_NODELAY, (char*)&l_iTrue, sizeof(l_iTrue));
#endif
			int l_iResult = recv(m_i32Socket, static_cast<char *>(pBuffer), ui32BufferSize, Socket_ReceiveFlags);
			if (ui32BufferSize != 0 && l_iResult <= 0)
			{
				close();
			}
			return l_iResult <= 0 ? 0 : (uint32_t)l_iResult;
		}

		virtual bool sendBufferBlocking(const void* pBuffer, const uint32_t ui32BufferSize)
		{
			uint32_t l_ui32LeftBytes = ui32BufferSize;
			const char* l_pBuffer    = static_cast<const char*>(pBuffer);
			do
			{
				l_ui32LeftBytes -= sendBuffer(l_pBuffer + ui32BufferSize - l_ui32LeftBytes, l_ui32LeftBytes);
				if (!isConnected()) { return false; }
			} while (l_ui32LeftBytes != 0);
			return true;
		}

		virtual bool receiveBufferBlocking(void* pBuffer, const uint32_t ui32BufferSize)
		{
			uint32_t l_ui32LeftBytes = ui32BufferSize;
			char* l_pBuffer          = static_cast<char*>(pBuffer);
			do
			{
				l_ui32LeftBytes -= receiveBuffer(l_pBuffer + ui32BufferSize - l_ui32LeftBytes, l_ui32LeftBytes);
				if (!isConnected()) { return false; }
			} while (l_ui32LeftBytes != 0);
			return true;
		}

		virtual bool isConnected() const
		{
			return m_i32Socket != -1;
		}

		virtual void release()
		{
			if (isConnected())
			{
				close();
			}
			delete this;
		}

	protected:

		int32_t m_i32Socket;
	};
} // namespace Socket
