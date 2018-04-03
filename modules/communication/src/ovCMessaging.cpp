#include <vector>
#include <array>
#include <string>
#include <cstdint>
#include <thread>
#include <mutex>
#include <algorithm>
#include <memory>
#include <cstring>

#include "ovCMessaging.h"

using namespace Communication;

const std::map<CMessaging::ELibraryError, std::string> CMessaging::m_ErrorsString =
{
	{NoError, "No error"},
	{Socket_NotConnected, "Not connected"},
	{Socket_FailedToConnect, "Failed to connect" },
	{Socket_ReceiveBufferFail, "Failed to receive the buffer"},
	{Socket_SendBufferFail, "Failed to send the buffer"},
	{Socket_NoIncomingClientConnection, "No incoming client connection before the timeout"},
	{Socket_NotReadyToSend, "Socket not ready to send the buffer"},
	{Socket_NoDataReceived, "No data received by the socket"},
	{Socket_FailedCloseClientConnection, "Failed to close the client connection"},
	{Socket_FailedToCloseConnection, "Failed to close the server connection"},
	{Socket_FailedConnectClient, "Failed to connect the client"},
	{Socket_ClientAlreadyConnected, "A client is already connected"},
	{Deserialize_BufferTooSmall, "Buffer received is too small to be unpacked"},
	{Deserialize_Header, "Fail to unpack the buffer to a Header" },
	{Deserialize_ProtocolVersionMessage, "Fail to unpack Protocol Version message"},
	{Deserialize_BoxDescriptionMessage,"Fail to unpack Box description message"},
	{Deserialize_EBMLMessage, "Fail to unpack EBML message"},
	{Deserialize_EndMessage, "Fail to unpack End message"},
	{Deserialize_ErrorMessage, "Fail to unpack error message"},
	{Deserialize_LogMessage, "Fail to unpack log message"},
	{Deserialize_AuthenticationMessage, "Fail to unpack Authentication message"},
	{Deserialize_MessageTypeNotSupported, "Message type not supported"},
	{BoxDescriptionAlreadyReceived, "Box Description already received"},
	{BoxDescriptionNotReceived, "Box description not received"},
	{BadAuthenticationReceived, "Authentication received is invalid"},
	{NoAuthenticationReceived, "No authentication received before the timeout"},
	{ThreadJoinFailed, "Failed to terminate the thread"}
};

CMessaging::CMessaging()
	: m_MessageCount(0)
	, m_Connection(nullptr)
	, m_LastLibraryError(CMessaging::ELibraryError::NoError)
	, m_IsStopRequested(false)
	, m_IsInErrorState(false)
	, m_IsEndMessageReceived(false)
{
}

CMessaging::~CMessaging()
{
	this->reset();
}

void CMessaging::reset()
{
	this->stopSyncing();

	m_MessageCount = 0;
	m_IsInErrorState = false;
	m_IsStopRequested = false;
	m_LastLibraryError = CMessaging::ELibraryError::NoError;

	std::queue<std::pair<uint64_t, AuthenticationMessage>>().swap(m_IncomingAuthentications);
	std::queue<std::pair<uint64_t, CommunicationProtocolVersionMessage>>().swap(m_IncomingCommunicationProtocolVersions);
	std::queue<std::pair<uint64_t, BoxDescriptionMessage>>().swap(m_IncomingBoxDescriptions);
	std::queue<std::pair<uint64_t, EBMLMessage>>().swap(m_IncomingEBMLs);
	std::queue<std::pair<uint64_t, LogMessage>>().swap(m_IncomingLogs);
	std::queue<std::pair<uint64_t, ErrorMessage>>().swap(m_IncomingErrors);

	m_RcvBuffer.clear();
	m_SendBuffer.clear();
	m_SendBuffer.reserve(s_BufferSize);

	m_Connection = nullptr;
}

CMessaging::ELibraryError CMessaging::getLastError() const
{
	return m_LastLibraryError;
}

void CMessaging::setLastError(const CMessaging::ELibraryError libraryError)
{
	m_LastLibraryError = libraryError;
}

bool CMessaging::push()
{
	if (!m_SendBuffer.empty() && m_Connection->isReadyToSend(1))
	{
		std::lock_guard<std::mutex> lock(m_SendBufferMutex);
		uint_fast32_t result = m_Connection->sendBufferBlocking(m_SendBuffer.data(), static_cast<unsigned int>(m_SendBuffer.size()));

		if (result == 0)
		{
			this->setLastError(Socket_SendBufferFail);
			return false;
		}

		m_SendBuffer.clear();
	}

	return true;
}

bool CMessaging::pull()
{
	if (m_Connection == nullptr)
	{
		this->setLastError(Socket_NotConnected);
		return false;
	}

	while (m_Connection->isReadyToReceive(1))
	{
		uint_fast32_t bytesReceived = m_Connection->receiveBuffer(m_TemporaryRcvBuffer.data(), static_cast<unsigned int>(m_TemporaryRcvBuffer.size()));

		if (bytesReceived == 0)
		{
			this->setLastError(Socket_ReceiveBufferFail);
			return false;
		}

		m_RcvBuffer.insert(m_RcvBuffer.end(), m_TemporaryRcvBuffer.cbegin(), m_TemporaryRcvBuffer.cbegin() + static_cast<unsigned int>(bytesReceived));
	}

	return true;
}

bool CMessaging::processIncomingMessages()
{
	size_t byteRead = 0;

	while (!m_RcvBuffer.empty())
	{
		if (!this->processBuffer(m_RcvBuffer, byteRead))
		{
			m_RcvBuffer.clear();
			// Error set in the function
			return false;
		}

		// If the processing succeed, we erase the buffer part processed. 
		if (byteRead != 0)
		{
			m_RcvBuffer.erase(m_RcvBuffer.begin(), m_RcvBuffer.begin() + static_cast<const long>(byteRead));
		}
		else
		{
			// The processing succeed but the byte count read is 0 so more data is waited.
			break;
		}
	}

	return true;
}

bool CMessaging::processBuffer(const std::vector<uint8_t>& buffer, size_t& byteRead)
{
	byteRead = 0;

	if (buffer.empty())
	{
		return true;
	}

	// First, we try to fromBytes the buffer to found header information
	Header header;

	if (!header.fromBytes(buffer, byteRead))
	{
		this->setLastError(Deserialize_Header);
		byteRead = static_cast<size_t>(header.getSize());
		return false;
	}

	if (buffer.size() < header.getSize())
	{
		byteRead = 0;
		return true; // Just wait for more data
	}

	// Try to unpack the object according to the type given by the header.
	switch (header.getType())
	{
		case EMessageType::MessageType_Authentication:
		{
			AuthenticationMessage authentication;

			if (!authentication.fromBytes(buffer, byteRead))
			{
				this->pushMessage(ErrorMessage(EError::Error_BadMessage, header.getId()));
				this->setLastError(Deserialize_AuthenticationMessage);
				return false;
			}

			std::lock_guard<std::mutex> lock(m_IncAuthMutex);
			m_IncomingAuthentications.emplace(header.getId(), authentication);
		}
		break;

		case EMessageType::MessageType_ProtocolVersion:
		{
			CommunicationProtocolVersionMessage communicationProtocolVersion;

			if (!communicationProtocolVersion.fromBytes(buffer, byteRead))
			{
				this->pushMessage(ErrorMessage(EError::Error_BadMessage, header.getId()));
				this->setLastError(Deserialize_ProtocolVersionMessage);
				return false;
			}

			std::lock_guard<std::mutex> lock(m_IncCommProVerMutex);
			m_IncomingCommunicationProtocolVersions.emplace(header.getId(), communicationProtocolVersion);
		}
		break;

		case EMessageType::MessageType_BoxInformation:
		{
			BoxDescriptionMessage boxDescription;

			if (!boxDescription.fromBytes(buffer, byteRead))
			{
				this->pushMessage(ErrorMessage(EError::Error_BadMessage, header.getId()));
				this->setLastError(Deserialize_BoxDescriptionMessage);
				return false;
			}

			std::lock_guard<std::mutex> lock(m_IncBoxDescriptionMutex);
			m_IncomingBoxDescriptions.emplace(header.getId(), boxDescription);
		}
		break;

		case EMessageType::MessageType_EBML:
		{
			EBMLMessage ebml;

			if (!ebml.fromBytes(buffer, byteRead))
			{
				this->pushMessage(ErrorMessage(EError::Error_BadMessage, header.getId()));
				this->setLastError(Deserialize_EBMLMessage);
				return false;
			}

			if (ebml.getIndex() >= m_BoxDescription.getOutputs()->size())
			{
				this->pushMessage(ErrorMessage(EError::Error_InvalidOutputIndex, header.getId()));
			}

			std::lock_guard<std::mutex> lock(m_IncEBMLMutex);
			m_IncomingEBMLs.emplace(header.getId(), ebml);
		}
		break;

		case EMessageType::MessageType_Log:
		{
			LogMessage log;

			if (!log.fromBytes(buffer, byteRead))
			{
				this->pushMessage(ErrorMessage(EError::Error_BadMessage, header.getId()));
				this->setLastError(Deserialize_LogMessage);
				return false;
			}

			std::lock_guard<std::mutex> lock(m_IncLogMutex);
			m_IncomingLogs.emplace(header.getId(), log);
		}
		break;

		case EMessageType::MessageType_End:
		{
			m_IsEndMessageReceived = true;
		}
		break;

		case EMessageType::MessageType_Error:
		{
			ErrorMessage error;

			if (!error.fromBytes(buffer, byteRead))
			{
				this->pushMessage(ErrorMessage(EError::Error_BadMessage, header.getId()));
				this->setLastError(Deserialize_ErrorMessage);
				return false;
			}

			std::lock_guard<std::mutex> lock(m_IncErrorsMutex);
			m_IncomingErrors.emplace(header.getId(), error);
		}
		break;

		case EMessageType::MessageType_Time:
		{
			TimeMessage timeMessage;

			if (!timeMessage.fromBytes(buffer, byteRead))
			{
				this->pushMessage(ErrorMessage(EError::Error_BadMessage, header.getId()));
				this->setLastError(Deserialize_ErrorMessage);
				return false;
			}

			m_Time = timeMessage.getTime();
		}
		break;

		case EMessageType::MessageType_Sync:
		{
			m_WasSyncMessageReceived = true;
		}
		break;

		case EMessageType::MessageType_Unknown:
		case EMessageType::MessageType_MAX:
			this->pushMessage(ErrorMessage(EError::Error_InvalidMessageType, header.getId()));
			this->setLastError(Deserialize_MessageTypeNotSupported);
			return false;
	}

	return true;
}

bool CMessaging::pushMessage(const Message& message)
{
	if (this->isInErrorState())
	{
		return false;
	}

	std::vector<uint8_t> messageBuffer = message.toBytes();
	Header header(message.getMessageType(), m_MessageCount++, messageBuffer.size());
	std::vector<uint8_t> headerBuffer = header.toBytes();

	std::lock_guard<std::mutex> lock(m_SendBufferMutex);
	m_SendBuffer.insert(m_SendBuffer.end(), headerBuffer.begin(), headerBuffer.end());
	m_SendBuffer.insert(m_SendBuffer.end(), messageBuffer.begin(), messageBuffer.end());

	return true;
}

std::string CMessaging::getErrorString(ELibraryError error)
{
	return m_ErrorsString.at(error);
}

bool CMessaging::isConnected()
{
	if (m_Connection == nullptr)
	{
		return false;
	}

	return m_Connection->isConnected();
}

void CMessaging::sync()
{
	while (true)
	{
		if (!this->push())
		{
			m_IsInErrorState = true;
			break;
		}

		if (!this->pull())
		{
			m_IsInErrorState = true;
			break;
		}

		if (!this->processIncomingMessages())
		{
			m_IsInErrorState = true;
			break;
		}


		if (m_IsStopRequested)
		{
			// Used to be sure to send the end message
			this->pull();
			break;
		}
	}
}

void CMessaging::setConnection(Socket::IConnection* connection)
{
	m_Connection = connection;
}

bool CMessaging::startSyncing()
{
	m_IsEndMessageReceived = false;
	m_SyncThread = std::thread(&CMessaging::sync, this);
	m_IsStopRequested = false;
	return true;
}

bool CMessaging::stopSyncing()
{
	m_IsStopRequested = true;

	if (m_SyncThread.joinable())
	{
		m_SyncThread.join();
	}

	return true;
}

bool CMessaging::popAuthentication(uint64_t& id, std::string& connectionID)
{
	std::lock_guard<std::mutex> lock(m_IncAuthMutex);

	if (m_IncomingAuthentications.empty())
	{
		return false;
	}

	id = m_IncomingAuthentications.front().first;
	connectionID = m_IncomingAuthentications.front().second.getConnectionID();
	m_IncomingAuthentications.pop();
	return true;
}

bool CMessaging::popBoxDescriptions(uint64_t& id, BoxDescriptionMessage& boxDescription)
{
	std::lock_guard<std::mutex> lock(m_IncBoxDescriptionMutex);

	if (m_IncomingBoxDescriptions.empty())
	{
		return false;
	}

	id = m_IncomingBoxDescriptions.front().first;
	boxDescription = m_IncomingBoxDescriptions.front().second;
	m_IncomingBoxDescriptions.pop();

	return true;
}

bool CMessaging::popCommunicationProtocolVersion(uint64_t& id, uint8_t& majorVersion, uint8_t& minorVersion)
{
	std::lock_guard<std::mutex> lock(m_IncCommProVerMutex);

	if (m_IncomingCommunicationProtocolVersions.empty())
	{
		return false;
	}

	id = m_IncomingCommunicationProtocolVersions.front().first;
	majorVersion = m_IncomingCommunicationProtocolVersions.front().second.getMajorVersion();
	minorVersion = m_IncomingCommunicationProtocolVersions.front().second.getMinorVersion();
	m_IncomingEBMLs.pop();
	return true;
}

bool CMessaging::popLog(uint64_t& id, ELogLevel& type, std::string& message)
{
	std::lock_guard<std::mutex> lock(m_IncLogMutex);

	if (m_IncomingLogs.empty())
	{
		return false;
	}

	id = m_IncomingLogs.front().first;
	type = m_IncomingLogs.front().second.getType();
	message = m_IncomingLogs.front().second.getMessage();
	m_IncomingLogs.pop();
	return true;
}

bool CMessaging::popError(uint64_t& id, EError& type, uint64_t& guiltyId)
{
	std::lock_guard<std::mutex> lock(m_IncErrorsMutex);

	if (m_IncomingErrors.empty())
	{
		return false;
	}

	id = m_IncomingErrors.front().first;
	type = m_IncomingErrors.front().second.getType();
	guiltyId = m_IncomingErrors.front().second.getGuiltyId();

	m_IncomingErrors.pop();
	return true;
}

bool CMessaging::popEBML(uint64_t& id, uint32_t& index, uint64_t& startTime, uint64_t& endTime, std::shared_ptr<const std::vector<uint8_t>>& ebml)
{
	std::lock_guard<std::mutex> lock(m_IncEBMLMutex);

	if (m_IncomingEBMLs.empty())
	{
		return false;
	}

	id = m_IncomingEBMLs.front().first;
	index = m_IncomingEBMLs.front().second.getIndex();
	startTime = m_IncomingEBMLs.front().second.getStartTime();
	endTime = m_IncomingEBMLs.front().second.getEndTime();
	ebml = m_IncomingEBMLs.front().second.getEBML();
	m_IncomingEBMLs.pop();
	return true;
}

bool CMessaging::popEnd(uint64_t& id)
{
	std::lock_guard<std::mutex> lock(m_IncEndMutex);

	if (m_IncomingEnds.empty())
	{
		return false;
	}

	id = m_IncomingEnds.front().first;
	m_IncomingEnds.pop();
	return true;
}

void CMessaging::setConnectionID(const std::string& connectionID)
{
	m_ConnectionID = connectionID;
}

bool CMessaging::waitForSyncMessage()
{
	if (m_WasSyncMessageReceived)
	{
		m_WasSyncMessageReceived = false;
		return true;
	}
	else
	{
		return false;
	}
}

