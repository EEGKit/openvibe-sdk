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

		virtual bool close() { return m_oConnectionDelegate.fpClose(m_oConnectionDelegate.pUserData); }

		virtual bool isReadyToSend(const uint32_t timeOut) const
		{
			return m_oConnectionDelegate.fpIsReadyToSend(m_oConnectionDelegate.pUserData, timeOut);
		}

		virtual bool isReadyToReceive(const uint32_t timeOut) const
		{
			return m_oConnectionDelegate.fpIsReadyToReceive(m_oConnectionDelegate.pUserData, timeOut);
		}

		virtual uint32_t sendBuffer(const void* buffer, const uint32_t size)
		{
			return m_oConnectionDelegate.fpSendBuffer(m_oConnectionDelegate.pUserData, buffer, size);
		}

		virtual uint32_t receiveBuffer(void* buffer, const uint32_t size)
		{
			return m_oConnectionDelegate.fpReceiveBuffer(m_oConnectionDelegate.pUserData, buffer, size);
		}

		virtual bool sendBufferBlocking(const void* buffer, const uint32_t size)
		{
			return m_oConnectionDelegate.fpSendBufferBlocking(m_oConnectionDelegate.pUserData, buffer, size);
		}

		virtual bool receiveBufferBlocking(void* buffer, const uint32_t bufferSize)
		{
			return m_oConnectionDelegate.fpReceiveBufferBlocking(m_oConnectionDelegate.pUserData, buffer, bufferSize);
		}

		virtual bool isConnected() const { return m_oConnectionDelegate.fpIsConnected(m_oConnectionDelegate.pUserData); }

		virtual void release() { m_oConnectionDelegate.fpRelease(m_oConnectionDelegate.pUserData); }

		virtual ~TConnectionDelegate() { }
	protected:
		virtual bool open() { return m_oConnectionDelegate.fpOpen(m_oConnectionDelegate.pUserData); }

		SConnectionDelegate m_oConnectionDelegate;
	};
} // namespace Socket
