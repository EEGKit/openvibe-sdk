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
#include <cstring>
#include <thread>
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
	std::vector<std::string> g_ReceivedData;
	bool g_ServerStarted = false;

	// server callback run from a child thread
	void onServerListening(int port, unsigned int expectedPacketCount)
	{
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

		// notify main thread that the server is created so that it can connect a single client
		g_ServerStartedCondVar.notify_one();

		// loop until all packet are received
		while (g_ReceivedData.size() < expectedPacketCount)
		{
			if (server->isReadyToReceive())
			{
				clientConnection = server->accept();
			}

			if (clientConnection && clientConnection->isReadyToReceive())
			{
				unsigned int dataSize =  0;
				unsigned int bytesToReceive = sizeof(dataSize);
				unsigned int bytesReceived = 0;
				char dataBuffer[32];

				// first receive data size
				while (bytesReceived < bytesToReceive)
				{
					bytesReceived += clientConnection->receiveBuffer(&dataSize, bytesToReceive - bytesReceived);
				}

				// then receive data
				bytesToReceive = dataSize;
				bytesReceived = 0;

				while (bytesReceived < bytesToReceive)
				{
					bytesReceived += clientConnection->receiveBuffer(dataBuffer, bytesToReceive - bytesReceived);
				}
				
				g_ReceivedData.push_back(std::string(dataBuffer, dataSize));
			}
		}

		server->release();
	}

	void sendData(Socket::IConnectionClient* client, void* data, unsigned int size)
	{
		unsigned int bytesToSend = size;
		unsigned int bytesSent = 0;

		while (bytesSent < bytesToSend)
		{
			bytesSent += client->sendBuffer(data, bytesToSend - bytesSent);
		}
	}
}

int uoSocketClientServerASyncCommunicationTest(int argc, char* argv[])
{
	OVT_ASSERT(argc == 4, "Failure to retrieve tests arguments. Expecting: server_name port_number packet_count");

	std::string serverName = argv[1];
	int portNumber = std::atoi(argv[2]);
	unsigned int packetCount = static_cast<unsigned int>(std::atoi(argv[3]));

	// test asynchronous data transmission from a single client to server:
	// - launch a server on a background thread
	// - connect a single client
	// - make client sending data
	// - marke server receiving and storing data
	// - join the thread and do the assertions on received data when no data race is possible

	// create client
	Socket::IConnectionClient* client = Socket::createConnectionClient();

	// launch server on background thread
	std::thread serverThread(onServerListening, portNumber, packetCount);

	// wait until the server is started to connect clients
	std::unique_lock<std::mutex> lock(g_ServerStartedMutex);
	g_ServerStartedCondVar.wait(lock, []() { return g_ServerStarted; });

	client->connect(serverName.c_str(), portNumber);

	// transmit data
	// transmission follows the protocol: data size transmission + data transmission
	std::string baseData = "Data packet index: ";

	char dataBuffer[32];
	for (unsigned int sendIndex = 0; sendIndex < packetCount; ++sendIndex) 
	{
		std::string dataString = baseData + std::to_string(sendIndex);
		std::strcpy(dataBuffer, dataString.c_str());
		uint32_t dataSize = static_cast<uint32_t>(dataString.size());

		sendData(client, &dataSize, sizeof(dataSize));
		sendData(client, dataBuffer, dataSize);
	}

	serverThread.join(); // wait until the end of the thread

	// release resources
	client->close();
	client->release();

	// do the assertion on the main thread
	OVT_ASSERT(g_ReceivedData.size() == packetCount, "Failure to retrieve packet count");

	for (unsigned int receivedIndex = 0; receivedIndex < packetCount; ++receivedIndex)
	{
		OVT_ASSERT_STREQ(g_ReceivedData[receivedIndex], (baseData + std::to_string(receivedIndex)), "Failure to retrieve packet");
	}

	return EXIT_SUCCESS;
}

