#pragma once

#include "IConnection.h"

namespace Socket
{
	class Socket_API IConnectionClient : public IConnection
	{
	public:

		virtual bool connect(const char* serverName, const uint32_t serverPort, const uint32_t timeOut = 0xffffffff) = 0;
	};

	extern Socket_API IConnectionClient* createConnectionClient();
} // namespace Socket
