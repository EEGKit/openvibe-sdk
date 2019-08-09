#pragma once

#include "defines.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <string>

namespace Communication
{
	/**
	 * \brief Log level used by the client to log information in the server side
	 */
	enum ELogLevel : uint8_t
	{
		LogLevel_Info = 1,
		LogLevel_Warning = 2,
		LogLevel_Error = 3,
		LogLevel_Fatal = 4,
		LogLevel_MAX = 5,
		LogLevel_Unknown = 0xFF
	};

	/**
	 * \brief Values that represent errors.
	 */
	enum EError : uint16_t
	{
		Error_AuthenticationFail = 1,
		Error_AuthenticationRequested = 2,
		Error_InvalidOutputIndex = 3,
		Error_BadCommunicationProtocol = 4,
		Error_InvalidEBML = 5,
		Error_InvalidLogLevel = 6,
		Error_InvalidMessageType = 7,
		Error_BadMessage = 8,
		Error_MAX = 9,
		Error_Unknown = 0xFF
	};

	/**
	 * \brief Message type
	 */
	enum EMessageType : uint8_t
	{
		MessageType_Authentication = 0,
		MessageType_ProtocolVersion = 1,
		MessageType_BoxInformation = 2,
		MessageType_EBML = 3,
		MessageType_Log = 4,
		MessageType_End = 5,
		MessageType_Error = 6,
		MessageType_Time = 7,
		MessageType_Sync = 8,
		MessageType_MAX = 9,

		MessageType_Unknown = 0xFF,
	};

	/**
	 * \brief A packet part is a part of a packet compound by an Header and a Message.
	 */
	class Communication_API PacketPart
	{
	public:

		virtual ~PacketPart() {}

		/**
		 * \brief Provide array of bytes that represent the object.
		 *
		 * \return A vector with the serialized information of the message that respect the communication protocol.
		 *
		 * \sa fromBytes
		 */
		virtual std::vector<uint8_t> toBytes() const = 0;

		/**
		 * \brief Transform bytes vector into an object.
		 *
		 * \param 		  	buffer	   	The buffer.
		 * \param [in,out]	bufferIndex	Zero-based index of the buffer.
		 *
		 * \retval True if it succeeds
		 * \retval False if it fails.
		 *
		 * \sa toBytes
		 */
		virtual bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) = 0;

		/**
		 * \brief Query if this object is valid.
		 *
		 * \retval True if it is valid.
		 * \retval False if it is invalid.
		 */
		bool isValid() const { return m_IsValid; }

	protected:
		bool m_IsValid = false;
	};

	/**
	 * \brief A message is the second part of a packet after the header.
	 */
	class Message : public PacketPart
	{
	public:
		virtual EMessageType getMessageType() const = 0;
	};

	/**
	 * \brief A header is associated to a message. It give information about the message, like the type and the size.
	 */
	class Header : PacketPart
	{
	public:
		Header();
		Header(EMessageType type, uint64_t id, uint64_t size);
		std::vector<uint8_t> toBytes() const override;
		void setId(uint64_t id);
		uint64_t getId() { return m_Id; }
		EMessageType getType() const;
		uint64_t getSize() const;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;

	private:
		EMessageType m_Type;
		uint64_t m_Id = 0;
		uint64_t m_Size = 0;

	private:
		static const size_t s_TypeIndex   = 0;
		static const size_t s_TypeSize    = sizeof(EMessageType);
		static const size_t s_IdIndex     = s_TypeIndex + s_TypeSize;
		static const size_t s_IdSize      = sizeof(uint64_t);
		static const size_t s_SizeIndex   = s_IdIndex + s_IdSize;
		static const size_t s_SizeSize    = sizeof(uint64_t);
		static const size_t s_MinimumSize = s_TypeSize + s_IdSize + s_SizeSize;
	};

	/**
	 * \brief Represent an Authentication message.
	 */
	class AuthenticationMessage : public Message
	{
	public:
		AuthenticationMessage();
		AuthenticationMessage(const std::string& connectionID);
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;
		std::string getConnectionID();

	private:
		static const size_t s_SizeIndex         = 0;
		static const size_t s_SizeSize          = sizeof(uint64_t);
		static const size_t s_ConnectionIDIndex = s_SizeIndex + s_SizeSize;
		static const size_t s_MinimumSize       = s_SizeSize;

		std::string m_ConnectionID;
	};

	/**
	 * \brief This message is used to inform the server or the client about the current communication protocol version used.
	 */
	class CommunicationProtocolVersionMessage : public Message
	{
	public:

		CommunicationProtocolVersionMessage();
		CommunicationProtocolVersionMessage(uint8_t majorVersion, uint8_t minorVersion);
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;
		uint8_t getMajorVersion() { return m_MajorVersion; }
		uint8_t getMinorVersion() { return m_MinorVersion; }

	private:
		uint8_t m_MinorVersion = 0;
		uint8_t m_MajorVersion = 0;

	private:
		static const size_t s_MajorIndex = 0;
		static const size_t s_MajorSize  = sizeof(uint8_t);
		static const size_t s_MinorIndex = s_MajorIndex + s_MajorSize;
		static const size_t s_MinorSize  = sizeof(uint8_t);
		static const size_t s_MinimumSize = s_MajorSize + s_MinorSize;
	};

	/**
	 * \brief InputOutput class describes the input or output of a box.
	 */
	class InputOutput : public PacketPart
	{
	public:
		InputOutput();
		InputOutput(uint32_t id, uint64_t type, const std::string& name);
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		uint32_t getId() const;
		uint64_t getType() const;
		std::string getName() const;

	private:
		uint32_t m_Id = 0;
		uint64_t m_Type = 0;
		std::string m_Name;

	private:
		static const size_t s_IdIndex       = 0;
		static const size_t s_IdSize        = sizeof(uint32_t);
		static const size_t s_TypeIndex     = s_IdIndex + s_IdSize;
		static const size_t s_TypeSize      = sizeof(uint64_t);
		static const size_t s_NameSizeIndex = s_TypeIndex + s_TypeSize;
		static const size_t s_NameSizeSize  = sizeof(uint32_t);
		static const size_t s_NameIndex     = s_NameSizeIndex + s_NameSizeSize;
		static const size_t s_MinimumSize   = s_IdSize + s_TypeSize + s_NameSizeSize;
	};

	class Parameter : public PacketPart
	{
	public:
		Parameter();
		Parameter(uint32_t id, uint64_t type, const std::string& name, const std::string& value);
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		uint32_t getId() const;
		uint64_t getType() const;
		std::string getName() const;
		std::string getValue() const;

	private:
		uint32_t m_Id = 0;
		uint64_t m_Type = 0;
		std::string m_Name;
		std::string m_Value;

	private:
		static const size_t s_IdIndex        = 0;
		static const size_t s_IdSize         = sizeof(uint32_t);
		static const size_t s_TypeIndex      = s_IdIndex + s_IdSize;
		static const size_t s_TypeSize       = sizeof(uint64_t);
		static const size_t s_NameSizeIndex  = s_TypeIndex + s_TypeSize;
		static const size_t s_NameSizeSize   = sizeof(uint32_t);
		static const size_t s_ValueSizeIndex = s_NameSizeIndex + s_NameSizeSize;
		static const size_t s_ValueSizeSize  = sizeof(uint32_t);
		static const size_t s_NameIndex      = s_ValueSizeIndex + s_ValueSizeSize;
		static const size_t s_MinimumSize    = s_IdSize + s_TypeSize + s_NameSizeSize + s_ValueSizeSize;
	};

	/**
	 * \brief This message contains information about the box used in the NeuroRT pipeline.
	 *	This message is sent by the server and received by the client.
	 */
	class BoxDescriptionMessage final : public Message
	{
	public:
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;
		bool addInput(uint32_t id, uint64_t type, const std::string& name);
		bool addOutput(uint32_t id, uint64_t type, const std::string& name);
		bool addParameter(uint32_t id, uint64_t type, const std::string& name, const std::string& value);
		const std::vector<InputOutput>* getInputs() const;
		const std::vector<InputOutput>* getOutputs() const;
		const std::vector<Parameter>* getParameters() const;

	private:
		std::vector<InputOutput> m_Inputs;
		std::vector<InputOutput> m_Outputs;
		std::vector<Parameter> m_Parameters;

	private:
		static const size_t s_InputCountIndex = 0;
		static const size_t s_InputCountSize  = sizeof(uint32_t);
		static const size_t s_OutputCountIndex = s_InputCountIndex + s_InputCountSize;
		static const size_t s_OutputCountSize  = sizeof(uint32_t);
		static const size_t s_ParameterCountIndex = s_OutputCountIndex + s_OutputCountSize;
		static const size_t s_ParameterCountSize  = sizeof(uint32_t);
		static const size_t s_MinimumSize = s_InputCountSize + s_OutputCountSize + s_ParameterCountSize;
	};

	/**
	 * \brief Log message is a way to communicate information from the client to the server.
	 */
	class LogMessage final : public Message
	{
	public:
		LogMessage();
		LogMessage(ELogLevel type, const std::string& message);
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;
		ELogLevel getType() const;
		std::string getMessage() const;

	private:
		ELogLevel m_Type;
		std::string m_Message;

	private:
		static const size_t s_TypeIndex = 0;
		static const size_t s_TypeSize  = sizeof(ELogLevel);
		static const size_t s_SizeIndex = s_TypeIndex + s_TypeSize;
		static const size_t s_SizeSize  = sizeof(uint32_t);
		static const size_t s_NameIndex = s_SizeIndex + s_SizeSize;
		static const size_t s_MinimumSize = s_TypeSize + s_SizeSize;
	};

	/**
	 * \brief EBML message is used to send EBML data.
	 */
	class EBMLMessage final : public Message
	{
	public:
		EBMLMessage();

		EBMLMessage(uint32_t index, uint64_t startTime, uint64_t endTime, std::shared_ptr<const std::vector<uint8_t>> ebml);

		std::vector<uint8_t> toBytes() const override;

		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;

		uint32_t getIndex() const;

		uint64_t getStartTime() const;

		uint64_t getEndTime() const;

		std::shared_ptr<const std::vector<uint8_t>> getEBML() const;

	private:
		uint32_t m_IOIndex = 0;
		uint64_t m_StartTime = 0;
		uint64_t m_EndTime = 0;

		std::shared_ptr<const std::vector<uint8_t>> m_EBML;

	private:
		static const size_t s_IOIndexIndex = 0;
		static const size_t s_IOIndexSize  = sizeof(uint32_t);

		static const size_t s_StartTimeIndex = s_IOIndexIndex + s_IOIndexSize;
		static const size_t s_StartTimeSize  = sizeof(uint64_t);

		static const size_t s_EndTimeIndex = s_StartTimeIndex + s_StartTimeSize;
		static const size_t s_EndTimeSize  = sizeof(uint64_t);

		static const size_t s_SizeIndex = s_EndTimeIndex + s_EndTimeSize;
		static const size_t s_SizeSize  = sizeof(uint32_t);

		static const size_t s_EBMLIndex = s_SizeIndex + s_SizeSize;

		static const size_t s_MinimumSize = s_IOIndexSize + s_StartTimeSize + s_EndTimeSize + s_SizeSize;
	};

	/**
	 * \brief Error message is used to alert the client that the server raise an error due to a message by the client.
	 */
	class ErrorMessage final : public Message
	{
	public:
		ErrorMessage();
		ErrorMessage(EError error, uint64_t guiltyId);
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;
		EError getType() const;
		uint64_t getGuiltyId() const;

	private:
		EError m_Type;
		uint64_t m_GuiltyId = 0;

	private:
		static const size_t s_TypeIndex     = 0;
		static const size_t s_TypeSize      = sizeof(EError);
		static const size_t s_GuiltyIdIndex = s_TypeIndex + s_TypeSize;
		static const size_t s_GuiltyIdSize  = sizeof(uint64_t);
		static const size_t s_MinimumSize   = s_GuiltyIdSize + s_TypeSize;
	};

	/**
	 * \brief End message can be sent by the server or/and by the client to inform that the processing is stopped and the connection will be closed.
	 */
	class EndMessage final : public Message
	{
	public:
		EndMessage() {}
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;
	};

	/**
	 * \brief Time messages are only sent by the server to inform the client about the current time of the pipeline.
	 */
	class TimeMessage final : public Message
	{
	public:
		TimeMessage(uint64_t time = 0);
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;

		uint64_t getTime() const;

	private:
		uint64_t m_Time = 0;

	private:
		static const size_t s_TimeIndex = 0;
		static const size_t s_TimeSize  = sizeof(uint64_t);

		static const size_t s_MinimumSize = s_TimeSize;
	};

	/**
	 * \brief Sync message can be sent by the server or/and by the client to inform that it's waiting the other part.
	 */
	class SyncMessage final : public Message
	{
	public:
		SyncMessage() {}
		std::vector<uint8_t> toBytes() const override;
		bool fromBytes(const std::vector<uint8_t>& buffer, size_t& bufferIndex) override;
		EMessageType getMessageType() const override;
	};
}
