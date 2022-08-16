#include "CEBMLBaseDecoder.hpp"

namespace OpenViBE {
namespace Plugins {
namespace StreamCodecs {

bool CEBMLBaseDecoder::initialize()
{
	ip_bufferToDecode.initialize(getInputParameter(EBMLDecoder_InputParameterId_MemoryBufferToDecode));
	m_readerHelper = EBML::createReaderHelper();
	m_reader       = createReader(m_callbackProxy);

	return true;
}

bool CEBMLBaseDecoder::uninitialize()
{
	m_reader->release();
	m_reader = nullptr;

	m_readerHelper->release();
	m_readerHelper = nullptr;

	ip_bufferToDecode.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseDecoder::process()
{
	m_reader->processData(ip_bufferToDecode->getDirectPointer(), ip_bufferToDecode->getSize());
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Header) { return true; }
	if (identifier == OVTK_NodeId_Header_StreamVersion) { return false; }
	if (identifier == OVTK_NodeId_Header_StreamType) { return false; }
	if (identifier == OVTK_NodeId_Buffer) { return true; }
	if (identifier == OVTK_NodeId_End) { return true; }
	return false;
}

void CEBMLBaseDecoder::openChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Header) { activateOutputTrigger(EBMLDecoder_OutputTriggerId_ReceivedHeader, true); }
	if (identifier == OVTK_NodeId_Buffer) { activateOutputTrigger(EBMLDecoder_OutputTriggerId_ReceivedBuffer, true); }
	if (identifier == OVTK_NodeId_End) { activateOutputTrigger(EBMLDecoder_OutputTriggerId_ReceivedEnd, true); }
}

}  // namespace StreamCodecs
}  // namespace Plugins
}  // namespace OpenViBE
