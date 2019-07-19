#pragma once

#include "IConnection.h"

namespace Socket
{
	class Socket_API IConnectionServer : public IConnection
	{
	public:

		/*
		* \brief Places a socket in a listening state on the specified port.
		* \param ui32Port [in]: port on the one the socket should listen.
		*                       when set to '0', the socket wil start on 
		*                       an available port.
		*/
		virtual bool listen(
			uint32_t ui32Port) = 0;

		virtual IConnection* accept(void) = 0;

		/*
		 * \brief Returns the port on the one the server is listening.
		 * This is useful if you set the port to '0'.
		 * \param port [out]: port on the one the server is listening
		 */
		virtual bool getSocketPort(uint32_t& port) = 0;
	};

	extern Socket_API IConnectionServer* createConnectionServer(void);
};


