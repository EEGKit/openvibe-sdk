#pragma once

#include "IConnection.h"

namespace Socket
{
	class Socket_API IConnectionClient : public IConnection
	{
	public:

		virtual bool connect(
			const char* sServerName,
			uint32_t ui32ServerPort,
			uint32_t ui32TimeOut = 0xffffffff) = 0;
	};

	extern Socket_API IConnectionClient* createConnectionClient(void);
};


