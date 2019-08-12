#pragma once

#include "ovCMessaging.h"
#include "ovCMessagingProtocol.h"

#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <thread>
#include <mutex>
#include <algorithm>
#include <memory>
#include <queue>
#include <map>
#include <atomic>

namespace Communication
{
	struct CMessaging::MessagingImplementation
	{
	public:
		std::string m_ConnectionID;
		BoxDescriptionMessage m_BoxDescription;
		std::atomic<uint64_t> m_Time;

		unsigned long long m_MessageCount;

		std::mutex m_IncAuthMutex;
		std::queue<std::pair<uint64_t, AuthenticationMessage>> m_IncomingAuthentications;

		std::mutex m_IncCommProVerMutex;
		std::queue<std::pair<uint64_t, CommunicationProtocolVersionMessage>> m_IncomingCommunicationProtocolVersions;

		std::mutex m_IncBoxDescriptionMutex;
		std::queue<std::pair<uint64_t, BoxDescriptionMessage>> m_IncomingBoxDescriptions;

		std::mutex m_IncEBMLMutex;
		std::queue<std::pair<uint64_t, EBMLMessage>> m_IncomingEBMLs;

		std::mutex m_IncLogMutex;
		std::queue<std::pair<uint64_t, LogMessage>> m_IncomingLogs;

		std::mutex m_IncErrorsMutex;
		std::queue<std::pair<uint64_t, ErrorMessage>> m_IncomingErrors;

		std::mutex m_IncEndMutex;
		std::queue<std::pair<uint64_t, EndMessage>> m_IncomingEnds;

		static const size_t s_BufferSize = 1024 * 64; //< Empirical value

		std::vector<uint8_t> m_RcvBuffer;
		std::array<uint8_t, s_BufferSize> m_TemporaryRcvBuffer;

		std::mutex m_SendBufferMutex;
		std::vector<uint8_t> m_SendBuffer;

		Socket::IConnection* m_Connection = nullptr;

		mutable std::atomic<ELibraryError> m_LastLibraryError;

		std::thread m_SyncThread;

		std::atomic<bool> m_IsStopRequested;
		std::atomic<bool> m_IsInErrorState;
		std::atomic<bool> m_IsEndMessageReceived;

		std::atomic<bool> m_WasSyncMessageReceived;
	};
}
