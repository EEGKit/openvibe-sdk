#ifndef __Socket_IConnection_H__
#define __Socket_IConnection_H__

#include "defines.h"

namespace Socket
{
	class Socket_API IConnection
	{
	protected:

		virtual bool open(void) = 0;

	public:

		virtual bool close(void) = 0;

		virtual bool isReadyToSend(
			uint32_t ui32TimeOut = 0) const = 0;
		virtual bool isReadyToReceive(
			uint32_t ui32TimeOut = 0) const = 0;

		virtual uint32_t sendBuffer(
			const void* pBuffer,
			const uint32_t ui32BufferSize) = 0;
		virtual uint32_t receiveBuffer(
			void* pBuffer,
			const uint32_t ui32BufferSize) = 0;

		virtual bool sendBufferBlocking(
			const void* pBuffer,
			const uint32_t ui32BufferSize) = 0;
		virtual bool receiveBufferBlocking(
			void* pBuffer,
			const uint32_t ui32BufferSize) = 0;

		virtual bool isConnected(void) const = 0;

		virtual void release(void) = 0;

	protected:

		virtual ~IConnection(void);
	};
};

#endif // __Socket_IConnection_H__
