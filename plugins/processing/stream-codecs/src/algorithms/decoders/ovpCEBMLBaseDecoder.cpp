#include "ovpCEBMLBaseDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CEBMLBaseDecoder::CEBMLBaseDecoder(void)
	: m_pEBMLReaderHelper(NULL)
	  , m_pEBMLReader(NULL)
	  , m_oEBMLReaderCallbackProxy(
		  *this,
		  &CEBMLBaseDecoder::isMasterChild,
		  &CEBMLBaseDecoder::openChild,
		  &CEBMLBaseDecoder::processChildData,
		  &CEBMLBaseDecoder::closeChild) {}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseDecoder::initialize(void)
{
	ip_pMemoryBufferToDecode.initialize(getInputParameter(OVP_Algorithm_EBMLStreamDecoder_InputParameterId_MemoryBufferToDecode));

	m_pEBMLReaderHelper = EBML::createReaderHelper();

	m_pEBMLReader = createReader(m_oEBMLReaderCallbackProxy);

	return true;
}

bool CEBMLBaseDecoder::uninitialize(void)
{
	m_pEBMLReader->release();
	m_pEBMLReader = NULL;

	m_pEBMLReaderHelper->release();
	m_pEBMLReaderHelper = NULL;

	ip_pMemoryBufferToDecode.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseDecoder::process(void)
{
	m_pEBMLReader->processData(ip_pMemoryBufferToDecode->getDirectPointer(), ip_pMemoryBufferToDecode->getSize());

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if (rIdentifier == OVTK_NodeId_Header) { return true; }
	else if (rIdentifier == OVTK_NodeId_Header_StreamVersion) { return false; }
	else if (rIdentifier == OVTK_NodeId_Header_StreamType) { return false; }
	else if (rIdentifier == OVTK_NodeId_Buffer) { return true; }
	else if (rIdentifier == OVTK_NodeId_End) { return true; }
	return false;
}

void CEBMLBaseDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	if (rIdentifier == OVTK_NodeId_Header)
	{
		activateOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedHeader, true);
	}
	if (rIdentifier == OVTK_NodeId_Buffer)
	{
		activateOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedBuffer, true);
	}
	if (rIdentifier == OVTK_NodeId_End)
	{
		activateOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedEnd, true);
	}
}

void CEBMLBaseDecoder::processChildData(const void* pBuffer, const uint64_t ui64BufferSize) {}

void CEBMLBaseDecoder::closeChild(void) {}
