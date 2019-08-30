#include "ovpCEBMLBaseDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CEBMLBaseDecoder::CEBMLBaseDecoder()
	: m_oEBMLReaderCallbackProxy(*this, &CEBMLBaseDecoder::isMasterChild, &CEBMLBaseDecoder::openChild, &CEBMLBaseDecoder::processChildData, &CEBMLBaseDecoder::closeChild) {}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseDecoder::initialize()
{
	ip_pMemoryBufferToDecode.initialize(getInputParameter(OVP_Algorithm_EBMLStreamDecoder_InputParameterId_MemoryBufferToDecode));
	m_pEBMLReaderHelper = EBML::createReaderHelper();
	m_pEBMLReader = createReader(m_oEBMLReaderCallbackProxy);

	return true;
}

bool CEBMLBaseDecoder::uninitialize()
{
	m_pEBMLReader->release();
	m_pEBMLReader = nullptr;

	m_pEBMLReaderHelper->release();
	m_pEBMLReaderHelper = nullptr;

	ip_pMemoryBufferToDecode.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseDecoder::process()
{
	m_pEBMLReader->processData(ip_pMemoryBufferToDecode->getDirectPointer(), ip_pMemoryBufferToDecode->getSize());
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
	if (identifier == OVTK_NodeId_Header)
	{
		activateOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedHeader, true);
	}
	if (identifier == OVTK_NodeId_Buffer)
	{
		activateOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedBuffer, true);
	}
	if (identifier == OVTK_NodeId_End)
	{
		activateOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedEnd, true);
	}
}

