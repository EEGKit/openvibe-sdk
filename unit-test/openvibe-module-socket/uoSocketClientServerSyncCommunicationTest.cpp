/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
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

#include <iostream>
#include <string>
#include <thread>
#include <atomic>
#include <list>
#include <chrono>
#include <condition_variable>
#include <vector>

#include "socket/IConnection.h"
#include "socket/IConnectionClient.h"
#include "socket/IConnectionServer.h"

#include "ovtAssert.h"

namespace
{
	std::condition_variable g_ServerStartedCondVar;
	std::mutex g_ServerStartedMutex;
	std::condition_variable g_ClientConnectedCondVar;
	std::mutex g_ClientConnectedMutex;
	std::vector<std::string> g_ReceivedData;
	bool g_ServerStarted   = false;
	bool g_ClientConnected = false;

	// server callback run from a child thread
	void onServerListening(const int port, const uint32_t packetCount)
	{
		// only the server side modifies g_ReceivedData thus no need to handle race condition
		g_ReceivedData.clear();

		Socket::IConnection* clientConnection = nullptr;

		// create server
		Socket::IConnectionServer* server = Socket::createConnectionServer();
		server->listen(port);

		// keep the scope braces here, as it ensures mutex is released
		{
			std::lock_guard<std::mutex> lockOnServerStart(g_ServerStartedMutex);
			g_ServerStarted = true;
		}

		g_ServerStartedCondVar.notify_one();

		// connect clients
		while (!clientConnection) { if (server->isReadyToReceive()) { clientConnection = server->accept(); } }

		// keep the scope braces here, as it ensures mutex is released
		{
			std::lock_guard<std::mutex> lockOnClientConnected(g_ClientConnectedMutex);
			g_ClientConnected = true;
		}

		g_ClientConnectedCondVar.notify_one();

		while (g_ReceivedData.size() < packetCount)
		{
			if (clientConnection->isReadyToReceive())
			{
				uint32_t dataSize = 0;
				char dataBuffer[64];
				clientConnection->receiveBufferBlocking(&dataSize, sizeof(dataSize));
				clientConnection->receiveBufferBlocking(dataBuffer, dataSize);
				g_ReceivedData.push_back(std::string(dataBuffer, dataSize));
			}
		}

		server->release();
	}
}

int uoSocketClientServerSyncCommunicationTest(int argc, char* argv[])
{
	OVT_ASSERT(argc == 4, "Failure to retrieve tests arguments. Expecting: server_name port_number packet_count");

	const std::string serverName   = argv[1];
	int portNumber           = std::atoi(argv[2]);
	uint32_t packetCount = uint32_t(std::atoi(argv[3]));

	// test synchronous data transmission from a single client to server:
	// - launch a server on a background thread
	// - connect single client
	// - make client sending data
	// - make server receiving and storing data
	// - join the thread and do assertions on received data in the main thread

	// create a client
	Socket::IConnectionClient* client = Socket::createConnectionClient();

	// launch server on background thread
	std::thread serverThread(onServerListening, portNumber, packetCount);

	// wait until the server is started to connect client
	std::unique_lock<std::mutex> lockOnServerStart(g_ServerStartedMutex);
	g_ServerStartedCondVar.wait(lockOnServerStart, []() { return g_ServerStarted; });


	client->connect(serverName.c_str(), portNumber);

	// wait until the connection is made to transmit data
	std::unique_lock<std::mutex> lockOnClientConnected(g_ClientConnectedMutex);
	g_ClientConnectedCondVar.wait(lockOnClientConnected, []() { return g_ClientConnected; });

	// transmit data
	// transmission follows the protocol: data size transmission + data transmission
	const std::string baseData = "Data packet index: ";

	for (uint32_t sendIndex = 0; sendIndex < packetCount; ++sendIndex)
	{
		std::string dataString = baseData + std::to_string(sendIndex);
		uint32_t dataSize      = uint32_t(dataString.size());

		client->sendBufferBlocking(&dataSize, sizeof(dataSize));
		client->sendBufferBlocking(dataString.c_str(), dataSize);
	}

	serverThread.join(); // wait until the end of the thread

	// release resources
	client->close();
	client->release();

	// do the assertion on the main thread
	OVT_ASSERT(g_ReceivedData.size() == packetCount, "Failure to retrieve packet count");

	for (uint32_t receivedIndex = 0; receivedIndex < packetCount; ++receivedIndex)
	{
		OVT_ASSERT_STREQ(g_ReceivedData[receivedIndex], (baseData + std::to_string(receivedIndex)), "Failure to retrieve packet");
	}

	return EXIT_SUCCESS;
}
