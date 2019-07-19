#pragma once

#include "IConnection.h"

namespace Socket
{
	struct SConnectionDelegate
	{
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
		TConnectionDelegate(SConnectionDelegate oConnectionDelegate) : m_oConnectionDelegate(oConnectionDelegate) { }

		virtual bool close(void)
		{
			return m_oConnectionDelegate.fpClose(m_oConnectionDelegate.pUserData);
		}

		virtual bool isReadyToSend(uint32_t ui32TimeOut) const
		{
			return m_oConnectionDelegate.fpIsReadyToSend(m_oConnectionDelegate.pUserData, ui32TimeOut);
		}

		virtual bool isReadyToReceive(uint32_t ui32TimeOut) const
		{
			return m_oConnectionDelegate.fpIsReadyToReceive(m_oConnectionDelegate.pUserData, ui32TimeOut);
		}

		virtual uint32_t sendBuffer(const void* pBuffer, const uint32_t ui32BufferSize)
		{
			return m_oConnectionDelegate.fpSendBuffer(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual uint32_t receiveBuffer(void* pBuffer, const uint32_t ui32BufferSize)
		{
			return m_oConnectionDelegate.fpReceiveBuffer(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual bool sendBufferBlocking(const void* pBuffer, const uint32_t ui32BufferSize)
		{
			return m_oConnectionDelegate.fpSendBufferBlocking(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual bool receiveBufferBlocking(void* pBuffer, const uint32_t ui32BufferSize)
		{
			return m_oConnectionDelegate.fpReceiveBufferBlocking(m_oConnectionDelegate.pUserData, pBuffer, ui32BufferSize);
		}

		virtual bool isConnected(void) const
		{
			return m_oConnectionDelegate.fpIsConnected(m_oConnectionDelegate.pUserData);
		}

		virtual void release(void)
		{
			m_oConnectionDelegate.fpRelease(m_oConnectionDelegate.pUserData);
		}

	public:
		virtual ~TConnectionDelegate(void) { }
	protected:
		virtual bool open(void)
		{
			return m_oConnectionDelegate.fpOpen(m_oConnectionDelegate.pUserData);
		}


	protected:
		SConnectionDelegate m_oConnectionDelegate;
	};
}


