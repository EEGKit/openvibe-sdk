/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* CertiViBE Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License version 3,
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include "socket/IConnection.h"
#include "socket/IConnectionClient.h"
#include "socket/IConnectionServer.h"

#include "ovtAssert.h"


int uoSocketClientServerBaseTest(int argc, char* argv[])
{
	OVT_ASSERT(argc == 3, "Failure to retrieve tests arguments. Expecting: server_name port_number");

	std::string serverName = argv[1];
	int portNumber = std::atoi(argv[2]);
	
	// basic tests on server and clients

	Socket::IConnectionServer* server = Socket::createConnectionServer();
	Socket::IConnectionClient* client = Socket::createConnectionClient();
	
	OVT_ASSERT(!server->isConnected(), "Failure to check for connection state before connection happens");

	OVT_ASSERT(server->listen(portNumber) && server->isConnected(), "Failure to make socket listening for input connections");

	OVT_ASSERT(!server->listen(portNumber), "Failure to generate connection error if the socket is already connected");
	
	OVT_ASSERT(!server->isReadyToReceive(), "Failure to check for readyness to receive when no client is connected");

	OVT_ASSERT(server->close() && !server->isConnected(), "Failure to close connection");

	OVT_ASSERT(!client->isConnected(), "Failure to check for connection state before connection happens");

	OVT_ASSERT(!client->connect(serverName.c_str(), portNumber) && !client->isConnected(),
		"Failure to generate connection error due to no server currently running");

	OVT_ASSERT(server->listen(portNumber), "Failure to make socket listening for input connections after a disconnection");

	OVT_ASSERT(!client->connect("bad_server_name", portNumber) && !client->isConnected(),
		"Failure to generate connection error caused by wrong server name");

	OVT_ASSERT(!client->connect(serverName.c_str(), 0) && !client->isConnected(),
		"Failure to generate connection error caused by wrong port number");

	OVT_ASSERT(client->connect(serverName.c_str(), portNumber) && client->isConnected(),
		"Failure to connect to server");

	OVT_ASSERT(client->close() && !client->isConnected(),"Failure to disconnect");

	OVT_ASSERT(server->close() && !server->isConnected(), "Failure to close connection");
	
	server->release();
	client->release();

	return EXIT_SUCCESS;
}

