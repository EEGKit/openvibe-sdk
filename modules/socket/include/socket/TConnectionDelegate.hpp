#ifndef __Socket_IConnectionDelegate_H__
#define __Socket_IConnectionDelegate_H__

#include "IConnection.h"

namespace Socket
{
	struct SConnectionDelegate {
		void* pUserData;
		bool (*fpOpen)(void*);
		bool (*fpClose)(void*);
		bool (*fpIsReadyToSend)(void*, unsigned int);
		bool (*fpIsReadyToReceive)(void*, unsigned int);
		unsigned int (*fpSendBuffer)(void*, const void*, unsigned int);
		unsigned int (*fpReceiveBuffer)(void*, void*, unsigned int);
		bool (*fpSendBufferBlocking)(void*, const void*, unsigned int);
		bool (*fpReceiveBufferBlocking)(void*, void*, unsigned int);
		bool (*fpIsConnected)(void*);
		bool (*fpRelease)(void*);
	};

	template <class T>
	class Socket_API TConnectionDelegate : public T
	{
	public:
		TConnectionDelegate(SConnectionDelegate oConnectionDelegate) :
			m_oConnectionDelegate(oConnectionDelegate)
		{
		}

		virtual Socket::boolean close(void)
		{
			return m_oConnectionDelegate.fpClose(m_oConnectionDelegate.pUserData);
		}

		virtual Socket::boolean isReadyToSend(Socket::uint32 ui32TimeOut) const
		{
			return m_oConnectionDelegate.fpIsReadyToSend(m_oConnectionDelegate.pUserData, ui32TimeOut);
		}

		virtual Socket::boolean isReadyToReceive(Socket::uint32 ui32TimeOut) const
		{
			return m_oConnectionDelegate.fpIsReadyToReceive(m_oConnectionDelegate.pUserData, ui32TimeOut);
		}

		virtual Socket::uint32 sendBuffer(const void* pBuffer, const Socket::uint32 ui32BufferSize)
		{
			return m_oConnectionDelegate.fpSendBuffer(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual Socket::uint32 receiveBuffer(void* pBuffer, const Socket::uint32 ui32BufferSize)
		{
			return m_oConnectionDelegate.fpReceiveBuffer(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual Socket::boolean sendBufferBlocking(const void* pBuffer, const Socket::uint32 ui32BufferSize)
		{
			return m_oConnectionDelegate.fpSendBufferBlocking(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual Socket::boolean receiveBufferBlocking(void* pBuffer, const Socket::uint32 ui32BufferSize)
		{
			return m_oConnectionDelegate.fpReceiveBufferBlocking(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual Socket::boolean isConnected(void) const
		{
			return m_oConnectionDelegate.fpIsConnected(m_oConnectionDelegate.pUserData);
		}

		virtual void release(void)
		{
			m_oConnectionDelegate.fpRelease(m_oConnectionDelegate.pUserData);
		}

	public:
		virtual ~TConnectionDelegate(void)
		{

		}
	protected:
		virtual Socket::boolean open(void) {
			return m_oConnectionDelegate.fpOpen(m_oConnectionDelegate.pUserData);
		}


	protected:
		SConnectionDelegate m_oConnectionDelegate;
	};
}

#endif // __Socket_IConnectionDelegate_H__
