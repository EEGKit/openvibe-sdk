#include "ovpCBoxAlgorithmGenericStreamReader.h"

#include <iostream>
#include <limits>
#include <fs/Files.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;
using namespace FileIO;

bool CBoxAlgorithmGenericStreamReader::initialize()
{
	m_filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_pending  = false;
	m_streamIdxToOutputIdxs.clear();
	m_streamIdxToTypeIDs.clear();

	return true;
}

bool CBoxAlgorithmGenericStreamReader::uninitialize()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = nullptr;
	}

	return true;
}

bool CBoxAlgorithmGenericStreamReader::initializeFile()
{
	m_file = FS::Files::open(m_filename.toASCIIString(), "rb");

	OV_ERROR_UNLESS_KRF(m_file, "Error opening file [" << m_filename << "] for reading", Kernel::ErrorType::BadFileRead);

	return true;
}

bool CBoxAlgorithmGenericStreamReader::processClock(CMessage& /*msg*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmGenericStreamReader::process()
{
	if (m_file == nullptr) { if (!initializeFile()) { return false; } }
	Kernel::IBoxIO& boxCtx = this->getDynamicBoxContext();
	const size_t nInput    = this->getStaticBoxContext().getOutputCount();
	const CTime time       = this->getPlayerContext().getCurrentTime();
	bool finished          = false;

	while (!finished && (!feof(m_file) || m_pending))
	{
		if (m_pending)
		{
			if (m_endTime <= time)
			{
				OV_ERROR_UNLESS_KRF(m_outputIdx < nInput,
									"Stream index " << m_outputIdx << " can not be output from this box because it does not have enough outputs",
									Kernel::ErrorType::BadOutput);

				boxCtx.getOutputChunk(m_outputIdx)->append(m_pendingChunk);
				boxCtx.markOutputAsReadyToSend(m_outputIdx, m_startTime, m_endTime);
				m_pending = false;
			}
			else { finished = true; }
		}
		else
		{
			bool justStarted = true;
			while (!feof(m_file) && m_reader.getCurrentNodeID() == EBML::CIdentifier())
			{
				uint8_t byte;
				const size_t s = fread(&byte, sizeof(uint8_t), 1, m_file);

				OV_ERROR_UNLESS_KRF(s == 1 || justStarted, "Unexpected EOF in " << m_filename, Kernel::ErrorType::BadParsing);

				m_reader.processData(&byte, sizeof(byte));
				justStarted = false;
			}
			if (!feof(m_file) && m_reader.getCurrentNodeSize() != 0)
			{
				m_swap.setSize(m_reader.getCurrentNodeSize(), true);
				const size_t s = size_t(fread(m_swap.getDirectPointer(), sizeof(uint8_t), size_t(m_swap.getSize()), m_file));

				OV_ERROR_UNLESS_KRF(s == m_swap.getSize(), "Unexpected EOF in " << m_filename, Kernel::ErrorType::BadParsing);

				m_pendingChunk.setSize(0, true);
				m_startTime = std::numeric_limits<uint64_t>::max();
				m_endTime   = std::numeric_limits<uint64_t>::max();
				m_outputIdx = std::numeric_limits<size_t>::max();

				m_reader.processData(m_swap.getDirectPointer(), m_swap.getSize());
			}
		}
	}

	return true;
}

bool CBoxAlgorithmGenericStreamReader::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == EBML_Identifier_Header) { return true; }
	if (identifier == OVP_NodeId_OpenViBEStream_Header) { return true; }
	if (identifier == OVP_NodeId_OpenViBEStream_Header_Compression) { return false; }
	if (identifier == OVP_NodeId_OpenViBEStream_Header_StreamType) { return false; }
	if (identifier == OVP_NodeId_OpenViBEStream_Buffer) { return true; }
	if (identifier == OVP_NodeId_OpenViBEStream_Buffer_StreamIndex) { return false; }
	if (identifier == OVP_NodeId_OpenViBEStream_Buffer_StartTime) { return false; }
	if (identifier == OVP_NodeId_OpenViBEStream_Buffer_EndTime) { return false; }
	if (identifier == OVP_NodeId_OpenViBEStream_Buffer_Content) { return false; }
	return false;
}

void CBoxAlgorithmGenericStreamReader::openChild(const EBML::CIdentifier& identifier)
{
	m_nodes.push(identifier);

	EBML::CIdentifier& top = m_nodes.top();

	if (top == EBML_Identifier_Header) { m_hasEBMLHeader = true; }
	if (top == OVP_NodeId_OpenViBEStream_Header)
	{
		if (!m_hasEBMLHeader)
		{
			this->getLogManager() << Kernel::LogLevel_Info << "The file " << m_filename <<
					" uses an outdated (but still compatible) version of the .ov file format\n";
		}
	}
	if (top == OVP_NodeId_OpenViBEStream_Header)
	{
		m_streamIdxToOutputIdxs.clear();
		m_streamIdxToTypeIDs.clear();
	}
}

void CBoxAlgorithmGenericStreamReader::processChildData(const void* buffer, const size_t size)
{
	EBML::CIdentifier& top = m_nodes.top();

	// Uncomment this when ebml version will be used
	//if(top == EBML_Identifier_EBMLVersion) { const uint64_t versionNumber=(uint64_t)m_readerHelper.getUInt(buffer, size); }

	if (top == OVP_NodeId_OpenViBEStream_Header_Compression)
	{
		if (m_readerHelper.getUInt(buffer, size) != 0) { OV_WARNING_K("Impossible to use compression as it is not yet implemented"); }
	}
	if (top == OVP_NodeId_OpenViBEStream_Header_StreamType) { m_streamIdxToTypeIDs[m_streamIdxToTypeIDs.size()] = m_readerHelper.getUInt(buffer, size); }

	if (top == OVP_NodeId_OpenViBEStream_Buffer_StreamIndex)
	{
		const size_t streamIdx = size_t(m_readerHelper.getUInt(buffer, size));
		if (m_streamIdxToTypeIDs.find(streamIdx) != m_streamIdxToTypeIDs.end()) { m_outputIdx = m_streamIdxToOutputIdxs[streamIdx]; }
	}
	if (top == OVP_NodeId_OpenViBEStream_Buffer_StartTime) { m_startTime = m_readerHelper.getUInt(buffer, size); }
	if (top == OVP_NodeId_OpenViBEStream_Buffer_EndTime) { m_endTime = m_readerHelper.getUInt(buffer, size); }
	if (top == OVP_NodeId_OpenViBEStream_Buffer_Content)
	{
		m_pendingChunk.setSize(0, true);
		m_pendingChunk.append(reinterpret_cast<const uint8_t*>(buffer), size);
	}
}

void CBoxAlgorithmGenericStreamReader::closeChild()
{
	EBML::CIdentifier& top = m_nodes.top();

	if (top == OVP_NodeId_OpenViBEStream_Header)
	{
		const Kernel::IBox& boxCtx = this->getStaticBoxContext();

		std::map<size_t, size_t> outputIndexToStreamIdx;

		bool lostStreams = false;
		bool lastOutputs = false;

		// Go on each stream of the file
		for (auto it = m_streamIdxToTypeIDs.begin(); it != m_streamIdxToTypeIDs.end(); ++it)
		{
			CIdentifier typeID;
			size_t index = std::numeric_limits<size_t>::max();

			// Find the first box output with this type that has no file stream connected
			for (size_t i = 0; i < boxCtx.getOutputCount() && index == std::numeric_limits<size_t>::max(); ++i)
			{
				if (boxCtx.getOutputType(i, typeID))
				{
					if (outputIndexToStreamIdx.find(i) == outputIndexToStreamIdx.end())
					{
						if (typeID == it->second)
						{
							//const CString typeName = this->getTypeManager().getTypeName(it->second);
							index = i;
						}
					}
				}
			}

			// In case no suitable output was found, see if we can downcast some type
			for (size_t i = 0; i < boxCtx.getOutputCount() && index == std::numeric_limits<size_t>::max(); ++i)
			{
				if (boxCtx.getOutputType(i, typeID))
				{
					if (outputIndexToStreamIdx.find(i) == outputIndexToStreamIdx.end())
					{
						if (this->getTypeManager().isDerivedFromStream(it->second, typeID))
						{
							const CString srcTypeName = this->getTypeManager().getTypeName(it->second);
							const CString dstTypeName = this->getTypeManager().getTypeName(typeID);
							this->getLogManager() << Kernel::LogLevel_Info << "Note: downcasting output " << i + 1 << " from "
									<< srcTypeName << " to " << dstTypeName << ", as there is no exactly type-matching output connector.\n";
							index = i;
						}
					}
				}
			}

			// In case it was not found
			if (index == std::numeric_limits<size_t>::max())
			{
				CString typeName = this->getTypeManager().getTypeName(it->second);

				OV_WARNING_K("No free output connector for stream " << it->first << " of type " << it->second << " (" << typeName << ")");

				m_streamIdxToOutputIdxs[it->first] = std::numeric_limits<size_t>::max();
				lostStreams                        = true;
			}
			else
			{
				m_streamIdxToOutputIdxs[it->first] = index;
				outputIndexToStreamIdx[index]      = it->first;
			}
		}

		// Warns for output with no stream connected to them
		for (size_t i = 0; i < boxCtx.getOutputCount(); ++i)
		{
			if (outputIndexToStreamIdx.find(i) == outputIndexToStreamIdx.end())
			{
				OV_WARNING_K("No stream candidate in file for output " << i+1);
				lastOutputs = true;
			}
		}

		// When both outputs and streams were lost, there most probably was a damn mistake
		OV_ERROR_UNLESS_KRV(!lastOutputs || !lostStreams, "Invalid configuration: missing output for stream(s) and missing stream for output(s)",
							Kernel::ErrorType::BadConfig);
	}

	if (top == OVP_NodeId_OpenViBEStream_Buffer)
	{
		m_pending = ((m_outputIdx != std::numeric_limits<size_t>::max()) && (m_startTime != CTime::max()) && (m_endTime != CTime::max()));
	}

	m_nodes.pop();
}
