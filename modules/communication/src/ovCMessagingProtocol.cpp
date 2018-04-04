#include <array>
#include <string>
#include <algorithm>
#include <cstring>

#include "ovCMessaging.h"

using namespace Communication;

namespace
{
	/**
	 * \brief Copy a string to buffer
	 *
	 * \param [out]	dest	   	The buffer destination
	 * \param [out]	bufferIndex	The index of the buffer where the beginning of the string must be copied.
	 * \param		value	   	The string to copy.
	 *
	 * \retval True if it succeeds
	 * \retval False if it fails.
	 */
	bool copyTobuffer(std::vector<uint8_t>& dest, size_t& bufferIndex, const std::string& value)
	{
		if (dest.size() < bufferIndex + value.size())
		{
			return false;
		}

		memcpy(dest.data() + bufferIndex,
			value.data(),
			value.size());

		bufferIndex += value.size();

		return true;
	}

	/**
	 * \brief Copy a value to a buffer.
	 *
	 * \param [out]	dest	   	Destination for the.
	 * \param [out]	bufferIndex	Zero-based index of the buffer.
	 * \param		value	   	The value.
	 *
	 * \return	True if it succeeds, false if it fails.
	 */
	template<class T>
	bool copyTobuffer(std::vector<uint8_t>& dest, size_t& bufferIndex, const T& value)
	{
		if (dest.size() < bufferIndex + sizeof(value))
		{
			return false;
		}

		memcpy(dest.data() + bufferIndex,
			&value,
			sizeof(value));

		bufferIndex += sizeof(value);

		return true;
	}

	template<class T>
	bool copyToVariable(const std::vector<uint8_t>& src, const size_t bufferIndex, T& destVariable)
	{
		if (src.size() < bufferIndex + sizeof(destVariable))
		{
			return false;
		}

		memcpy(&destVariable,
			src.data() + bufferIndex,
			sizeof(destVariable));

		return true;
	}

	/**
	 * \brief Convert a buffer to a string
	 *
	 * \param 		src		   	The buffer
	 * \param 		bufferIndex	The index where to start the convertion.
	 * \param 		size	   	The size of the string.
	 * \param [out]	string	   	The string.
	 *
	 * \retval True if it succeeds
	 * \retval False if it fails.
	 *
	 * \sa copyToVariable
	 */
	bool copyToString(const std::vector<uint8_t>& src, const size_t bufferIndex, const size_t size, std::string& string)
	{
		if (src.size() < bufferIndex + size)
		{
			return false;
		}

		string = std::string(src.begin() + static_cast<const long>(bufferIndex)
			, src.begin() + static_cast<const long>(bufferIndex) + static_cast<const long>(size));

		return true;
	}
}

/******************************************************************************
*
* Header
*
******************************************************************************/

Header::Header()
	: m_Type(MessageType_Unknown)
	, m_Id(std::numeric_limits<decltype(m_Id)>::max())
	, m_Size(0)
{
	m_IsValid = false;
}

Header::Header(const EMessageType type, const uint64_t id, const uint64_t size)
	: m_Type(type)
	, m_Id(id)
	, m_Size(size)
{
	m_IsValid = true;
}

std::vector<uint8_t> Header::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize);
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_Type);
	copyTobuffer(buffer, bufferIndex, m_Id);
	copyTobuffer(buffer, bufferIndex, m_Size);

	return buffer;
}

void Header::setId(uint64_t id)
{
	m_Id = id;
}

EMessageType Header::getType() const
{
	return m_Type;
}

uint64_t Header::getSize() const
{
	return m_Size;
}

bool Header::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	uint8_t typeInt;

	if (!copyToVariable(buffer, bufferIndex + s_TypeIndex, typeInt)) return false;

	if (typeInt >= EMessageType::MessageType_MAX)
	{
		return false;
	}

	m_Type = static_cast<EMessageType>(typeInt);

	if (!copyToVariable(buffer, bufferIndex + s_IdIndex, m_Id)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_SizeIndex, m_Size)) return false;

	bufferIndex += s_MinimumSize;

	m_IsValid = true;

	return true;
}

/******************************************************************************
*
* Authentication
*
******************************************************************************/

AuthenticationMessage::AuthenticationMessage()
{
	m_IsValid = false;
}

AuthenticationMessage::AuthenticationMessage(const std::string& connectionID)
	: m_ConnectionID(connectionID)
{
	m_IsValid = true;
}

std::vector<uint8_t> AuthenticationMessage::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize + m_ConnectionID.size());
	size_t bufferIndex = 0;

	uint64_t size = m_ConnectionID.size();
	copyTobuffer(buffer, bufferIndex, size);
	copyTobuffer(buffer, bufferIndex, m_ConnectionID);

	return buffer;
}

bool AuthenticationMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	size_t passPhraseSize;
	if (!copyToVariable(buffer, bufferIndex + s_SizeIndex, passPhraseSize)) return false;

	if (!copyToString(buffer, bufferIndex + s_ConnectionIDIndex, passPhraseSize, m_ConnectionID))
	{
		return false;
	}

	m_IsValid = true;

	bufferIndex += s_MinimumSize + passPhraseSize;

	return true;
}

std::string AuthenticationMessage::getConnectionID()
{
	return m_ConnectionID;
}

EMessageType AuthenticationMessage::getMessageType() const
{
	return MessageType_Authentication;
}

/******************************************************************************
*
* Communication protocol version
*
******************************************************************************/

CommunicationProtocolVersionMessage::CommunicationProtocolVersionMessage()
	: m_MinorVersion(0)
	, m_MajorVersion(0)
{
	m_IsValid = false;
}

CommunicationProtocolVersionMessage::CommunicationProtocolVersionMessage(uint8_t majorVersion, uint8_t minorVersion)
	: m_MinorVersion(minorVersion)
	, m_MajorVersion(majorVersion)
{
	m_IsValid = true;
}

std::vector<uint8_t> CommunicationProtocolVersionMessage::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize);
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_MajorVersion);
	copyTobuffer(buffer, bufferIndex, m_MinorVersion);

	return buffer;
}

bool CommunicationProtocolVersionMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	if (!copyToVariable(buffer, bufferIndex + s_MajorIndex, m_MajorVersion)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_MinorIndex, m_MinorVersion)) return false;

	m_IsValid = true;

	bufferIndex += s_MinimumSize;

	return true;
}

EMessageType CommunicationProtocolVersionMessage::getMessageType() const
{
	return MessageType_ProtocolVersion;
}

/******************************************************************************
*
* Inout and output
*
******************************************************************************/

InputOutput::InputOutput()
	: m_Id(std::numeric_limits<decltype(m_Id)>::max())
	, m_Type(std::numeric_limits<decltype(m_Type)>::max())
	, m_Name(std::string())
{
	m_IsValid = false;
}

InputOutput::InputOutput(const uint32_t id, const uint64_t type, const std::string& name)
	: m_Id(id)
	, m_Type(type)
	, m_Name(name)
{
	m_IsValid = true;
}

std::vector<uint8_t> InputOutput::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize + m_Name.size());
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_Id);
	copyTobuffer(buffer, bufferIndex, m_Type);
	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_Name.size()));
	copyTobuffer(buffer, bufferIndex, m_Name);

	return buffer;
}

bool InputOutput::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	if (!copyToVariable(buffer, bufferIndex + s_IdIndex, m_Id)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_TypeIndex, m_Type)) return false;

	uint32_t nameSize;
	if (!copyToVariable(buffer, bufferIndex + s_NameSizeIndex, nameSize)) return false;

	if (!copyToString(buffer, bufferIndex + s_NameIndex, nameSize, m_Name)) return false;

	m_IsValid = true;

	bufferIndex += s_MinimumSize + nameSize;

	return true;
}

uint32_t InputOutput::getId() const
{
	return m_Id;
}

uint64_t InputOutput::getType() const
{
	return m_Type;
}

std::string InputOutput::getName() const
{
	return m_Name;
}

/******************************************************************************
*
* Parameter
*
******************************************************************************/

Parameter::Parameter()
	: m_Id(std::numeric_limits<decltype(m_Id)>::max())
	, m_Type(std::numeric_limits<decltype(m_Type)>::max())
	, m_Name(std::string())
	, m_Value(std::string())
{
	m_IsValid = false;
}

Parameter::Parameter(const uint32_t id, const uint64_t type, const std::string& name, const std::string& value)
	: m_Id(id)
	, m_Type(type)
	, m_Name(name)
	, m_Value(value)
{
	m_IsValid = true;
}

std::vector<uint8_t> Parameter::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize + m_Name.size() + m_Value.size());
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_Id);
	copyTobuffer(buffer, bufferIndex, m_Type);
	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_Name.size()));
	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_Value.size()));
	copyTobuffer(buffer, bufferIndex, m_Name);
	copyTobuffer(buffer, bufferIndex, m_Value);

	return buffer;
}

bool Parameter::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	if (!copyToVariable(buffer, bufferIndex + s_IdIndex, m_Id)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_TypeIndex, m_Type)) return false;

	uint32_t nameSize;
	if (!copyToVariable(buffer, bufferIndex + s_NameSizeIndex, nameSize)) return false;

	uint32_t valueSize;
	if (!copyToVariable(buffer, bufferIndex + s_ValueSizeIndex, valueSize)) return false;

	if (!copyToString(buffer, bufferIndex + s_NameIndex, nameSize, m_Name))
	{
		return false;
	}

	if (!copyToString(buffer, bufferIndex + s_NameIndex + nameSize, valueSize, m_Value))
	{
		return false;
	}

	bufferIndex += s_MinimumSize + nameSize + valueSize;

	return true;
}

uint32_t Parameter::getId() const
{
	return m_Id;
}

uint64_t Parameter::getType() const
{
	return m_Type;
}

std::string Parameter::getName() const
{
	return m_Name;
}

std::string Parameter::getValue() const
{
	return m_Value;
}

/******************************************************************************
 *
 * Box description
 *
 ******************************************************************************/

std::vector<uint8_t> BoxDescriptionMessage::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize);
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_Inputs.size()));
	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_Outputs.size()));
	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_Parameters.size()));

	for (const InputOutput& input : m_Inputs)
	{
		std::vector<uint8_t> inputBuffer = input.toBytes();
		buffer.insert(buffer.end(), inputBuffer.begin(), inputBuffer.end());
	}

	for (const InputOutput& output : m_Outputs)
	{
		std::vector<uint8_t> outputBuffer = output.toBytes();
		buffer.insert(buffer.end(), outputBuffer.begin(), outputBuffer.end());
	}

	for (const Parameter& parameter : m_Parameters)
	{
		std::vector<uint8_t> parameterBuffer = parameter.toBytes();
		buffer.insert(buffer.end(), parameterBuffer.begin(), parameterBuffer.end());
	}

	return buffer;
}

bool BoxDescriptionMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	uint32_t inputCount;
	uint32_t outputCount;
	uint32_t parameterCount;

	if (!copyToVariable(buffer, bufferIndex + s_InputCountIndex, inputCount)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_OutputCountIndex, outputCount)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_ParameterCountIndex, parameterCount)) return false;

	bufferIndex += s_MinimumSize;

	m_Inputs.clear();
	m_Outputs.clear();
	m_Parameters.clear();

	for (uint32_t i = 0; i < inputCount; i++)
	{
		InputOutput input;

		if (!input.fromBytes(buffer, bufferIndex))
		{
			return false;
		}

		m_Inputs.push_back(input);
	}

	for (uint32_t i = 0; i < outputCount; i++)
	{
		InputOutput output;

		if (!output.fromBytes(buffer, bufferIndex))
		{
			return false;
		}

		m_Outputs.push_back(output);
	}

	for (uint32_t i = 0; i < parameterCount; i++)
	{
		Parameter parameter;

		if (!parameter.fromBytes(buffer, bufferIndex))
		{
			return false;
		}

		m_Parameters.push_back(parameter);
	}

	m_IsValid = true;

	return true;
}

EMessageType BoxDescriptionMessage::getMessageType() const
{
	return MessageType_BoxInformation;
}

bool BoxDescriptionMessage::addInput(const uint32_t id, const uint64_t type, const std::string& name)
{
	auto it = std::find_if(m_Inputs.begin(), m_Inputs.end(), [&id](const InputOutput& obj) {return obj.getId() == id; });

	if (it != m_Inputs.end())
	{
		return false;
	}

	m_Inputs.emplace_back(id, type, name);
	return true;
}

bool BoxDescriptionMessage::addOutput(const uint32_t id, const uint64_t type, const std::string& name)
{
	auto it = std::find_if(m_Outputs.begin(), m_Outputs.end(), [&id](const InputOutput& obj) {return obj.getId() == id; });

	if (it != m_Outputs.end())
	{
		return false;
	}

	m_Outputs.emplace_back(id, type, name);
	return true;
}

bool BoxDescriptionMessage::addParameter(const uint32_t id, const uint64_t type, const std::string& name, const std::string& value)
{
	auto it = std::find_if(m_Parameters.begin(), m_Parameters.end(), [&id](const Parameter& obj) {return obj.getId() == id; });

	if (it != m_Parameters.end())
	{
		return false;
	}

	m_Parameters.emplace_back(id, type, name, value);
	return true;
}

const std::vector<InputOutput>* BoxDescriptionMessage::getInputs() const
{
	return &m_Inputs;
}

const std::vector<InputOutput>* BoxDescriptionMessage::getOutputs() const
{
	return &m_Outputs;
}

const std::vector<Parameter>* BoxDescriptionMessage::getParameters() const
{
	return &m_Parameters;
}

/******************************************************************************
*
* Packet part
*
******************************************************************************/

/******************************************************************************
*
* Log
*
******************************************************************************/

LogMessage::LogMessage()
	: m_Type(LogLevel_Unknown)
{
	m_IsValid = false;
}

LogMessage::LogMessage(const ELogLevel type, const std::string& message)
	: m_Type(type)
	, m_Message(message)
{
	m_IsValid = true;
}

std::vector<uint8_t> LogMessage::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize + m_Message.size());
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_Type);
	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_Message.size()));
	copyTobuffer(buffer, bufferIndex, m_Message);

	return buffer;
}

bool LogMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	// Type
	uint8_t typeInt;

	if (!copyToVariable(buffer, bufferIndex + s_TypeIndex, typeInt)) return false;

	if (typeInt >= ELogLevel::LogLevel_MAX)
	{
		return false;
	}

	m_Type = static_cast<ELogLevel>(typeInt);

	// Message size
	uint32_t messageSize;
	if (!copyToVariable(buffer, bufferIndex + s_SizeIndex, messageSize)) return false;
	if (!copyToString(buffer, bufferIndex + s_NameIndex, messageSize, m_Message)) return false;

	bufferIndex += s_MinimumSize + messageSize;

	m_IsValid = true;

	return true;
}

EMessageType LogMessage::getMessageType() const
{
	return MessageType_Log;
}

ELogLevel LogMessage::getType() const
{
	return m_Type;
}

std::string LogMessage::getMessage() const
{
	return m_Message;
}

/******************************************************************************
*
* EBML
*
******************************************************************************/

EBMLMessage::EBMLMessage()
	: m_IOIndex(std::numeric_limits<decltype(m_IOIndex)>::max())
	, m_StartTime(std::numeric_limits<decltype(m_StartTime)>::max())
	, m_EndTime(std::numeric_limits<decltype(m_EndTime)>::max())
{
	m_IsValid = false;
}

EBMLMessage::EBMLMessage(uint32_t index,
	uint64_t startTime,
	uint64_t endTime,
	std::shared_ptr<const std::vector<uint8_t>> ebml)
	: m_IOIndex(index)
	, m_StartTime(startTime)
	, m_EndTime(endTime)
	, m_EBML(ebml)
{
	m_IsValid = true;
}

std::vector<uint8_t> EBMLMessage::toBytes() const
{
	if (!m_IsValid)
	{
		return std::vector<uint8_t>();
	}

	std::vector<uint8_t> buffer(s_MinimumSize);
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_IOIndex);
	copyTobuffer(buffer, bufferIndex, m_StartTime);
	copyTobuffer(buffer, bufferIndex, m_EndTime);
	copyTobuffer(buffer, bufferIndex, static_cast<uint32_t>(m_EBML->size()));

	if (!m_EBML->empty())
	{
		buffer.insert(buffer.end(), m_EBML->begin(), m_EBML->end());
	}

	return buffer;
}

bool EBMLMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	if (!copyToVariable(buffer, bufferIndex + s_IOIndexIndex, m_IOIndex)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_StartTimeIndex, m_StartTime)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_EndTimeIndex, m_EndTime)) return false;

	uint32_t EBMLsize;
	if (!copyToVariable(buffer, bufferIndex + s_SizeIndex, EBMLsize)) return false;

	if (buffer.size() < bufferIndex + s_EBMLIndex + EBMLsize)
	{
		return false;
	}

	m_EBML.reset(new std::vector<uint8_t>(buffer.begin() + static_cast<const long>(bufferIndex) + s_EBMLIndex, buffer.begin() + static_cast<const long>(bufferIndex) + s_EBMLIndex + EBMLsize));

	bufferIndex += s_MinimumSize + EBMLsize;

	m_IsValid = true;

	return true;
}

EMessageType EBMLMessage::getMessageType() const
{
	return MessageType_EBML;
}

uint32_t EBMLMessage::getIndex() const
{
	return m_IOIndex;
}

uint64_t EBMLMessage::getStartTime() const
{
	return m_StartTime;
}

uint64_t EBMLMessage::getEndTime() const
{
	return m_EndTime;
}

std::shared_ptr<const std::vector<uint8_t>> EBMLMessage::getEBML() const
{
	return m_EBML;
}

/******************************************************************************
*
* Error
*
******************************************************************************/

ErrorMessage::ErrorMessage()
	: m_Type(Error_Unknown)
	, m_GuiltyId(std::numeric_limits<decltype(m_GuiltyId)>::max())
{
}

ErrorMessage::ErrorMessage(const EError error, const uint64_t guiltyId)
	: m_Type(error)
	, m_GuiltyId(guiltyId)
{
}

std::vector<uint8_t> ErrorMessage::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize);
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_Type);
	copyTobuffer(buffer, bufferIndex, m_GuiltyId);

	return buffer;
}

bool ErrorMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	if (!copyToVariable(buffer, bufferIndex + s_TypeIndex, m_Type)) return false;
	if (!copyToVariable(buffer, bufferIndex + s_GuiltyIdIndex, m_GuiltyId)) return false;

	bufferIndex += s_MinimumSize;

	m_IsValid = true;

	return true;
}

EMessageType ErrorMessage::getMessageType() const
{
	return MessageType_Error;
}

EError ErrorMessage::getType() const
{
	return m_Type;
}

uint64_t ErrorMessage::getGuiltyId() const
{
	return m_GuiltyId;
}

/******************************************************************************
*
* End message
*
******************************************************************************/

std::vector<uint8_t> EndMessage::toBytes() const
{
	return std::vector<uint8_t>();
}

bool EndMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	(void)buffer;
	(void)bufferIndex;
	return false;
}

EMessageType EndMessage::getMessageType() const
{
	return EMessageType::MessageType_End;
}

/******************************************************************************
 *
 * Time
 *
 ******************************************************************************/

TimeMessage::TimeMessage(uint64_t time)
	: m_Time(time)
{
}

std::vector<uint8_t> TimeMessage::toBytes() const
{
	std::vector<uint8_t> buffer(s_MinimumSize);
	size_t bufferIndex = 0;

	copyTobuffer(buffer, bufferIndex, m_Time);

	return buffer;
}

bool TimeMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	m_IsValid = false;

	if (buffer.size() < bufferIndex + s_MinimumSize)
	{
		return false;
	}

	if (!copyToVariable(buffer, bufferIndex + s_TimeIndex, m_Time)) return false;

	bufferIndex += s_MinimumSize;

	m_IsValid = true;

	return true;
}

EMessageType TimeMessage::getMessageType() const
{
	return MessageType_Time;
}

uint64_t TimeMessage::getTime() const
{
	return m_Time;
}

/******************************************************************************
*
* Sync
*
******************************************************************************/

std::vector<uint8_t> SyncMessage::toBytes() const
{
	return std::vector<uint8_t>();
}

bool SyncMessage::fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex)
{
	(void)buffer;
	(void)bufferIndex;
	return false;
}

EMessageType SyncMessage::getMessageType() const
{
	return EMessageType::MessageType_Sync;
}
