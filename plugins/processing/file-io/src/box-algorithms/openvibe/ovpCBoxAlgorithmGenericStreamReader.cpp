#include "ovpCBoxAlgorithmGenericStreamReader.h"

#include <iostream>
#include <limits>
#include <fs/Files.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace FileIO;

CBoxAlgorithmGenericStreamReader::CBoxAlgorithmGenericStreamReader() : m_oReader(*this) {}

uint64_t CBoxAlgorithmGenericStreamReader::getClockFrequency()
{
	return 128LL << 32; // the box clock frequency
}

bool CBoxAlgorithmGenericStreamReader::initialize()
{
	m_sFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	m_bPending = false;

	m_vStreamIndexToOutputIndex.clear();
	m_vStreamIndexToTypeIdentifier.clear();

	return true;
}

bool CBoxAlgorithmGenericStreamReader::uninitialize()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	}

	return true;
}

bool CBoxAlgorithmGenericStreamReader::initializeFile()
{
	m_pFile = FS::Files::open(m_sFilename.toASCIIString(), "rb");

	OV_ERROR_UNLESS_KRF(
		m_pFile,
		"Error opening file [" << m_sFilename << "] for reading",
		OpenViBE::Kernel::ErrorType::BadFileRead);

	return true;
}

bool CBoxAlgorithmGenericStreamReader::processClock(IMessageClock& /*messageClock*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmGenericStreamReader::process()
{
	if (m_pFile == nullptr) { if (!initializeFile()) { return false; } }
	IBoxIO& boxContext              = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getOutputCount();
	const uint64_t time = this->getPlayerContext().getCurrentTime();
	bool finished    = false;

	while (!finished && (!feof(m_pFile) || m_bPending))
	{
		if (m_bPending)
		{
			if (m_ui64EndTime <= time)
			{
				OV_ERROR_UNLESS_KRF(m_ui32OutputIndex < nInput,
									"Stream index " << m_ui32OutputIndex << " can not be output from this box because it does not have enough outputs",
									OpenViBE::Kernel::ErrorType::BadOutput);

				boxContext.getOutputChunk(m_ui32OutputIndex)->append(m_oPendingChunk);
				boxContext.markOutputAsReadyToSend(m_ui32OutputIndex, m_ui64StartTime, m_ui64EndTime);
				m_bPending = false;
			}
			else { finished = true; }
		}
		else
		{
			bool l_bJustStarted = true;
			while (!feof(m_pFile) && m_oReader.getCurrentNodeIdentifier() == EBML::CIdentifier())
			{
				uint8_t l_ui8Byte;
				const size_t s = fread(&l_ui8Byte, sizeof(uint8_t), 1, m_pFile);

				OV_ERROR_UNLESS_KRF(s == 1 || l_bJustStarted, "Unexpected EOF in " << m_sFilename, OpenViBE::Kernel::ErrorType::BadParsing);

				m_oReader.processData(&l_ui8Byte, sizeof(l_ui8Byte));
				l_bJustStarted = false;
			}
			if (!feof(m_pFile) && m_oReader.getCurrentNodeSize() != 0)
			{
				m_oSwap.setSize(m_oReader.getCurrentNodeSize(), true);
				const size_t s = size_t(fread(m_oSwap.getDirectPointer(), sizeof(uint8_t), size_t(m_oSwap.getSize()), m_pFile));

				OV_ERROR_UNLESS_KRF(s == m_oSwap.getSize(), "Unexpected EOF in " << m_sFilename, OpenViBE::Kernel::ErrorType::BadParsing);

				m_oPendingChunk.setSize(0, true);
				m_ui64StartTime   = std::numeric_limits<uint64_t>::max();
				m_ui64EndTime     = std::numeric_limits<uint64_t>::max();
				m_ui32OutputIndex = std::numeric_limits<uint32_t>::max();

				m_oReader.processData(m_oSwap.getDirectPointer(), m_oSwap.getSize());
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
	m_vNodes.push(identifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if (l_rTop == EBML_Identifier_Header) { m_bHasEBMLHeader = true; }
	if (l_rTop == OVP_NodeId_OpenViBEStream_Header)
	{
		if (!m_bHasEBMLHeader)
		{
			this->getLogManager() << LogLevel_Info << "The file " << m_sFilename << " uses an outdated (but still compatible) version of the .ov file format\n";
		}
	}
	if (l_rTop == OVP_NodeId_OpenViBEStream_Header)
	{
		m_vStreamIndexToOutputIndex.clear();
		m_vStreamIndexToTypeIdentifier.clear();
	}
}

void CBoxAlgorithmGenericStreamReader::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	// Uncomment this when ebml version will be used
	//if(l_rTop == EBML_Identifier_EBMLVersion)
	//{
	//	const uint64_t l_ui64VersionNumber=(uint64_t)m_oReaderHelper.getUIntegerFromChildData(buffer, size);
	//}

	if (l_rTop == OVP_NodeId_OpenViBEStream_Header_Compression)
	{
		if (m_oReaderHelper.getUIntegerFromChildData(buffer, size) != 0) { OV_WARNING_K("Impossible to use compression as it is not yet implemented"); }
	}
	if (l_rTop == OVP_NodeId_OpenViBEStream_Header_StreamType)
	{
		m_vStreamIndexToTypeIdentifier[m_vStreamIndexToTypeIdentifier.size()] = m_oReaderHelper.getUIntegerFromChildData(buffer, size);
	}

	if (l_rTop == OVP_NodeId_OpenViBEStream_Buffer_StreamIndex)
	{
		const uint32_t streamIdx = uint32_t(m_oReaderHelper.getUIntegerFromChildData(buffer, size));
		if (m_vStreamIndexToTypeIdentifier.find(streamIdx) != m_vStreamIndexToTypeIdentifier.end())
		{
			m_ui32OutputIndex = m_vStreamIndexToOutputIndex[streamIdx];
		}
	}
	if (l_rTop == OVP_NodeId_OpenViBEStream_Buffer_StartTime) { m_ui64StartTime = m_oReaderHelper.getUIntegerFromChildData(buffer, size); }
	if (l_rTop == OVP_NodeId_OpenViBEStream_Buffer_EndTime) { m_ui64EndTime = m_oReaderHelper.getUIntegerFromChildData(buffer, size); }
	if (l_rTop == OVP_NodeId_OpenViBEStream_Buffer_Content)
	{
		m_oPendingChunk.setSize(0, true);
		m_oPendingChunk.append(reinterpret_cast<const uint8_t*>(buffer), size);
	}
}

void CBoxAlgorithmGenericStreamReader::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if (l_rTop == OVP_NodeId_OpenViBEStream_Header)
	{
		const IBox& boxContext = this->getStaticBoxContext();

		std::map<uint32_t, uint32_t> l_vOutputIndexToStreamIndex;

		bool lostStreams = false;
		bool lastOutputs = false;

		// Go on each stream of the file
		for (auto it = m_vStreamIndexToTypeIdentifier.begin(); it != m_vStreamIndexToTypeIdentifier.end(); ++it)
		{
			CIdentifier OutputTypeID;
			uint32_t index = std::numeric_limits<uint32_t>::max();

			// Find the first box output with this type that has no file stream connected
			for (uint32_t i = 0; i < boxContext.getOutputCount() && index == std::numeric_limits<uint32_t>::max(); i++)
			{
				if (boxContext.getOutputType(i, OutputTypeID))
				{
					if (l_vOutputIndexToStreamIndex.find(i) == l_vOutputIndexToStreamIndex.end())
					{
						if (OutputTypeID == it->second)
						{
							//const CString l_sTypeName = this->getTypeManager().getTypeName(it->second);
							index = i;
						}
					}
				}
			}

			// In case no suitable output was found, see if we can downcast some type
			for (uint32_t i = 0; i < boxContext.getOutputCount() && index == std::numeric_limits<uint32_t>::max(); i++)
			{
				if (boxContext.getOutputType(i, OutputTypeID))
				{
					if (l_vOutputIndexToStreamIndex.find(i) == l_vOutputIndexToStreamIndex.end())
					{
						if (this->getTypeManager().isDerivedFromStream(it->second, OutputTypeID))
						{
							const CString l_sSourceTypeName = this->getTypeManager().getTypeName(it->second);
							const CString l_sOutputTypeName = this->getTypeManager().getTypeName(OutputTypeID);
							this->getLogManager() << LogLevel_Info << "Note: downcasting output " << i + 1 << " from "
									<< l_sSourceTypeName << " to " << l_sOutputTypeName << ", as there is no exactly type-matching output connector.\n";
							index = i;
						}
					}
				}
			}

			// In case it was not found
			if (index == std::numeric_limits<uint32_t>::max())
			{
				CString l_sTypeName = this->getTypeManager().getTypeName(it->second);

				OV_WARNING_K("No free output connector for stream " << it->first << " of type " << it->second << " (" << l_sTypeName << ")");

				m_vStreamIndexToOutputIndex[it->first] = std::numeric_limits<uint32_t>::max();
				lostStreams                         = true;
			}
			else
			{
				m_vStreamIndexToOutputIndex[it->first]   = index;
				l_vOutputIndexToStreamIndex[index] = it->first;
			}
		}

		// Warns for output with no stream connected to them
		for (uint32_t i = 0; i < boxContext.getOutputCount(); i++)
		{
			if (l_vOutputIndexToStreamIndex.find(i) == l_vOutputIndexToStreamIndex.end())
			{
				OV_WARNING_K("No stream candidate in file for output " << i+1);
				lastOutputs = true;
			}
		}

		// When both outputs and streams were lost, there most probably was a damn mistake
		OV_ERROR_UNLESS_KRV(!lastOutputs || !lostStreams, "Invalid configuration: missing output for stream(s) and missing stream for output(s)",
							OpenViBE::Kernel::ErrorType::BadConfig);
	}

	if (l_rTop == OVP_NodeId_OpenViBEStream_Buffer)
	{
		m_bPending = ((m_ui32OutputIndex != std::numeric_limits<uint32_t>::max()) &&
					  (m_ui64StartTime != std::numeric_limits<uint64_t>::max()) &&
					  (m_ui64EndTime != std::numeric_limits<uint64_t>::max()));
	}

	m_vNodes.pop();
}
