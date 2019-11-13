#include "ovpCBoxAlgorithmGenericStreamWriter.h"

#include <fs/Files.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace FileIO;

CBoxAlgorithmGenericStreamWriter::CBoxAlgorithmGenericStreamWriter() : m_oWriter(*this) {}

bool CBoxAlgorithmGenericStreamWriter::initialize()
{
	m_sFilename            = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const bool compression = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if (compression) { OV_WARNING_K("Impossible to use compression as it is not yet implemented"); }

	return true;
}

bool CBoxAlgorithmGenericStreamWriter::uninitialize()
{
	if (m_oFile.is_open()) { m_oFile.close(); }
	return true;
}

bool CBoxAlgorithmGenericStreamWriter::generateFileHeader()
{
	const IBox& boxContext = this->getStaticBoxContext();

	m_oSwap.setSize(0, true);

	m_oWriterHelper.connect(&m_oWriter);

	m_oWriterHelper.openChild(EBML_Identifier_Header);
	m_oWriterHelper.openChild(EBML_Identifier_DocType);
	m_oWriterHelper.setStr("OpenViBE_Stream_File");
	m_oWriterHelper.closeChild();

	m_oWriterHelper.openChild(EBML_Identifier_EBMLVersion);
	m_oWriterHelper.setUInt(1);
	m_oWriterHelper.closeChild();

	m_oWriterHelper.openChild(EBML_Identifier_EBMLIdLength);
	m_oWriterHelper.setUInt(10);
	m_oWriterHelper.closeChild();
	m_oWriterHelper.closeChild();

	m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Header);
	m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Header_Compression);
	m_oWriterHelper.setUInt(0 /* compression flag */);
	m_oWriterHelper.closeChild();
	for (size_t i = 0; i < boxContext.getInputCount(); ++i)
	{
		CIdentifier id;
		boxContext.getInputType(i, id);

		m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Header_StreamType);
		m_oWriterHelper.setUInt(id.toUInteger());
		m_oWriterHelper.closeChild();
	}
	m_oWriterHelper.closeChild();
	m_oWriterHelper.disconnect();

	FS::Files::openOFStream(m_oFile, m_sFilename.toASCIIString(), std::ios::binary | std::ios::trunc);

	OV_ERROR_UNLESS_KRF(m_oFile.good(), "Error opening file [" << m_sFilename << "] for writing", OpenViBE::Kernel::ErrorType::BadFileWrite);

	m_oFile.write(reinterpret_cast<const char*>(m_oSwap.getDirectPointer()), std::streamsize(m_oSwap.getSize()));

	m_isHeaderGenerate = true;
	return true;
}

bool CBoxAlgorithmGenericStreamWriter::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmGenericStreamWriter::process()
{
	IBoxIO& boxContext  = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	if (!m_isHeaderGenerate) { if (!generateFileHeader()) { return false; } }

	m_oSwap.setSize(0, true);

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
		{
			m_oWriterHelper.connect(&m_oWriter);
			m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Buffer);
			m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Buffer_StreamIndex);
			m_oWriterHelper.setUInt(i);
			m_oWriterHelper.closeChild();
			m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Buffer_StartTime);
			m_oWriterHelper.setUInt(boxContext.getInputChunkStartTime(i, j));
			m_oWriterHelper.closeChild();
			m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Buffer_EndTime);
			m_oWriterHelper.setUInt(boxContext.getInputChunkEndTime(i, j));
			m_oWriterHelper.closeChild();
			m_oWriterHelper.openChild(OVP_NodeId_OpenViBEStream_Buffer_Content);
			m_oWriterHelper.setBinary(boxContext.getInputChunk(i, j)->getDirectPointer(), boxContext.getInputChunk(i, j)->getSize());
			m_oWriterHelper.closeChild();
			m_oWriterHelper.closeChild();
			m_oWriterHelper.disconnect();
			boxContext.markInputAsDeprecated(i, j);
		}
	}

	if (m_oSwap.getSize() != 0)
	{
		m_oFile.write(reinterpret_cast<const char*>(m_oSwap.getDirectPointer()), std::streamsize(m_oSwap.getSize()));

		OV_ERROR_UNLESS_KRF(m_oFile.good(), "Error opening file [" << m_sFilename << "] for writing", OpenViBE::Kernel::ErrorType::BadFileWrite);
	}

	return true;
}

void CBoxAlgorithmGenericStreamWriter::write(const void* buffer, const size_t size) { m_oSwap.append(reinterpret_cast<const uint8_t*>(buffer), size); }
