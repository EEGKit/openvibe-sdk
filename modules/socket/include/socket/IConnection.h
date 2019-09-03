#pragma once

#include "defines.h"

namespace Socket
{
	class Socket_API IConnection
	{
	protected:

		virtual bool open() = 0;

	public:

		virtual bool close() = 0;

		virtual bool isReadyToSend(const uint32_t ui32TimeOut = 0) const = 0;
		virtual bool isReadyToReceive(const uint32_t ui32TimeOut = 0) const = 0;

		virtual uint32_t sendBuffer(const void* buffer, const uint32_t ui32BufferSize) = 0;
		virtual uint32_t receiveBuffer(void* buffer, const uint32_t ui32BufferSize) = 0;

		virtual bool sendBufferBlocking(const void* buffer, const uint32_t ui32BufferSize) = 0;
		virtual bool receiveBufferBlocking(void* buffer, const uint32_t ui32BufferSize) = 0;

		virtual bool isConnected() const = 0;

		virtual void release() = 0;

	protected:

		virtual ~IConnection();
	};
} // namespace Socket
