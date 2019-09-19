#pragma once

#include "IConnection.h"

namespace Socket
{
	struct SConnectionDelegate
	{
		void* data;
		bool (*fpOpen)(void*);
		bool (*fpClose)(void*);
		bool (*fpIsReadyToSend)(void*, uint32_t);
		bool (*fpIsReadyToReceive)(void*, uint32_t);
		uint32_t (*fpSendBuffer)(void*, const void*, uint32_t);
		uint32_t (*fpReceiveBuffer)(void*, void*, uint32_t);
		bool (*fpSendBufferBlocking)(void*, const void*, uint32_t);
		bool (*fpReceiveBufferBlocking)(void*, void*, uint32_t);
		bool (*fpIsConnected)(void*);
		bool (*fpRelease)(void*);
	};

	template <class T>
	class Socket_API TConnectionDelegate : public T
	{
	public:
		TConnectionDelegate(SConnectionDelegate oConnectionDelegate) : m_oConnectionDelegate(oConnectionDelegate) { }

		virtual bool close() { return m_oConnectionDelegate.fpClose(m_oConnectionDelegate.data); }

		virtual bool isReadyToSend(const uint32_t timeOut) const { return m_oConnectionDelegate.fpIsReadyToSend(m_oConnectionDelegate.data, timeOut); }

		virtual bool isReadyToReceive(const uint32_t timeOut) const
		{
			return m_oConnectionDelegate.fpIsReadyToReceive(m_oConnectionDelegate.data, timeOut);
		}

		virtual uint32_t sendBuffer(const void* buffer, const uint32_t size)
		{
			return m_oConnectionDelegate.fpSendBuffer(m_oConnectionDelegate.data, buffer, size);
		}

		virtual uint32_t receiveBuffer(void* buffer, const uint32_t size)
		{
			return m_oConnectionDelegate.fpReceiveBuffer(m_oConnectionDelegate.data, buffer, size);
		}

		virtual bool sendBufferBlocking(const void* buffer, const uint32_t size)
		{
			return m_oConnectionDelegate.fpSendBufferBlocking(m_oConnectionDelegate.data, buffer, size);
		}

		virtual bool receiveBufferBlocking(void* buffer, const uint32_t bufferSize)
		{
			return m_oConnectionDelegate.fpReceiveBufferBlocking(m_oConnectionDelegate.data, buffer, bufferSize);
		}

		virtual bool isConnected() const { return m_oConnectionDelegate.fpIsConnected(m_oConnectionDelegate.data); }

		virtual void release() { m_oConnectionDelegate.fpRelease(m_oConnectionDelegate.data); }

		virtual ~TConnectionDelegate() { }
	protected:
		virtual bool open() { return m_oConnectionDelegate.fpOpen(m_oConnectionDelegate.data); }

		SConnectionDelegate m_oConnectionDelegate;
	};
} // namespace Socket
