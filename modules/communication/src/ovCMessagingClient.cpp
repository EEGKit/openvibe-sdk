#include "ovCMessagingClient.h"

#include <array>
#include <algorithm>
#include <iterator>

using namespace Communication;

MessagingClient::MessagingClient()
	: CMessaging()
	, m_Client(Socket::createConnectionClient())
	, m_BoxDescriptionReceived(false)
{
}

MessagingClient::~MessagingClient()
{
	this->close();
	m_Client->release();
}

bool MessagingClient::connect(const std::string& URI, const unsigned int port)
{
	this->reset();

	if (!m_Client->connect(URI.c_str(), port))
	{
		this->setLastError(Socket_FailedToConnect);
		return false;
	}

	this->setConnection(m_Client);

	// Once the connection is done, the client push an authentication message to the server
	if (!this->pushAuthentication(m_ConnectionID))
	{
		// Error set in the function
		ELibraryError error = this->getLastError();
		this->close();
		this->setLastError(error);
		return false;
	}

	if (!this->startSyncing())
	{
		this->close();
		return false;
	}

	// To work, the client need to receive box information message. So we wait to receive the box information message.
	// A timeout of 10 seconds is set.
	const std::chrono::time_point<std::chrono::system_clock> startClock = std::chrono::system_clock::now();
	uint64_t packetId;

	while (std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - startClock).count() < 10)
	{
		if (this->popBoxDescriptions(packetId, m_BoxDescription))
		{
			m_BoxDescriptionReceived = true;
			break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	if (!m_BoxDescriptionReceived)
	{
		this->close();
		this->setLastError(BoxDescriptionNotReceived);
		return false;
	}

	return true;
}

bool MessagingClient::close()
{
	this->pushMessage(EndMessage());

	this->stopSyncing();

	if (m_Client != nullptr)
	{
		if (!m_Client->close())
		{
			this->setLastError(Socket_FailedToCloseConnection);
			return false;
		}
	}

	return true;
}

uint32_t MessagingClient::getParameterCount() const
{
	if (!m_BoxDescriptionReceived)
	{
		return 0;
	}

	return static_cast<uint32_t>(m_BoxDescription.getParameters()->size());
}

uint32_t MessagingClient::getInputCount() const
{
	if (!m_BoxDescriptionReceived)
	{
		return 0;
	}

	return static_cast<uint32_t>(m_BoxDescription.getInputs()->size());
}

uint32_t MessagingClient::getOutputCount() const
{
	if (!m_BoxDescriptionReceived)
	{
		return 0;
	}

	return static_cast<uint32_t>(m_BoxDescription.getOutputs()->size());
}

bool MessagingClient::getParameter(const size_t i, uint32_t& id, uint64_t& type, std::string& name, std::string& value) const
{
	if (!m_BoxDescriptionReceived)
	{
		return false;
	}

	const std::vector<Parameter>* parameters = m_BoxDescription.getParameters();

	if (parameters->size() <= i)
	{
		return false;
	}

	id = parameters->at(i).getId();
	type = parameters->at(i).getType();
	name = parameters->at(i).getName();
	value = parameters->at(i).getValue();

	return true;
}

bool MessagingClient::getInput(const size_t i, uint32_t& id, uint64_t& type, std::string& name) const
{
	if (!m_BoxDescriptionReceived)
	{
		return false;
	}

	const std::vector<InputOutput>* inputs = m_BoxDescription.getInputs();

	if (inputs->size() <= i)
	{
		return false;
	}

	id = inputs->at(i).getId();
	type = inputs->at(i).getType();
	name = inputs->at(i).getName();

	return true;
}

bool MessagingClient::getOutput(const size_t i, uint32_t& id, uint64_t& type, std::string& name) const
{
	if (!m_BoxDescriptionReceived)
	{
		const_cast<MessagingClient*>(this)->setLastError(BoxDescriptionNotReceived);
		return false;
	}

	const std::vector<InputOutput>* outputs = m_BoxDescription.getOutputs();

	if (outputs->size() <= i)
	{
		return false;
	}

	id = outputs->at(i).getId();
	type = outputs->at(i).getType();
	name = outputs->at(i).getName();

	return true;
}

bool MessagingClient::popError(uint64_t& packetId, EError& type, uint64_t& guiltyId)
{
	return CMessaging::popError(packetId, type, guiltyId);
};

bool MessagingClient::popEBML(uint64_t& packetId, uint32_t& index, uint64_t& startTime, uint64_t& endTime, std::shared_ptr<const std::vector<uint8_t>>& ebml)
{
	return CMessaging::popEBML(packetId, index, startTime, endTime, ebml);
}

bool MessagingClient::pushAuthentication(std::string connectionID)
{
	return this->pushMessage(AuthenticationMessage(connectionID));
}

bool MessagingClient::pushLog(ELogLevel logLevel, const std::string& log)
{
	return this->pushMessage(LogMessage(logLevel, log));
}

bool MessagingClient::pushEBML(const uint32_t index, const uint64_t startTime, const uint64_t endTime, std::shared_ptr<const std::vector<uint8_t>> ebml)
{
	return this->pushMessage(EBMLMessage(index, startTime, endTime, ebml));
}

bool MessagingClient::pushSync()
{
	return this->pushMessage(SyncMessage());
}

bool MessagingClient::waitForSyncMessage()
{
	return CMessaging::waitForSyncMessage();
}

