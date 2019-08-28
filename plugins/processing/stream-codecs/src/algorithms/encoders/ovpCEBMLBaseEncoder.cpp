#include "ovpCEBMLBaseEncoder.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CEBMLBaseEncoder::CEBMLBaseEncoder(): m_oEBMLWriterCallbackProxy(*this, &CEBMLBaseEncoder::write) {}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseEncoder::initialize()
{
	op_pMemoryBuffer.initialize(getOutputParameter(OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_pEBMLWriter       = createWriter(m_oEBMLWriterCallbackProxy);
	m_pEBMLWriterHelper = EBML::createWriterHelper();
	m_pEBMLWriterHelper->connect(m_pEBMLWriter);

	return true;
}

bool CEBMLBaseEncoder::uninitialize()
{
	m_pEBMLWriterHelper->disconnect();
	m_pEBMLWriterHelper->release();
	m_pEBMLWriterHelper = nullptr;

	m_pEBMLWriter->release();
	m_pEBMLWriter = nullptr;

	op_pMemoryBuffer.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CEBMLBaseEncoder::process()
{
	if (isInputTriggerActive(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader))
	{
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header);
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamType);
		m_pEBMLWriterHelper->setUIntegerAsChildData(0);
		m_pEBMLWriterHelper->closeChild();
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamVersion);
		m_pEBMLWriterHelper->setUIntegerAsChildData(0);
		m_pEBMLWriterHelper->closeChild();
		this->processHeader();
		m_pEBMLWriterHelper->closeChild();
		activateOutputTrigger(OVP_Algorithm_EBMLStreamEncoder_OutputTriggerId_MemoryBufferUpdated, true);
	}

	if (isInputTriggerActive(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer))
	{
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer);
		this->processBuffer();
		m_pEBMLWriterHelper->closeChild();
		activateOutputTrigger(OVP_Algorithm_EBMLStreamEncoder_OutputTriggerId_MemoryBufferUpdated, true);
	}

	if (isInputTriggerActive(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeEnd))
	{
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_End);
		this->processEnd();
		m_pEBMLWriterHelper->closeChild();
		activateOutputTrigger(OVP_Algorithm_EBMLStreamEncoder_OutputTriggerId_MemoryBufferUpdated, true);
	}

	return true;
}

// ________________________________________________________________________________________________________________
//

void CEBMLBaseEncoder::write(const void* pBuffer, const uint64_t size)
{
	uint64_t l_ui64CurrentBufferSize = op_pMemoryBuffer->getSize();
	op_pMemoryBuffer->setSize(l_ui64CurrentBufferSize + size, false);
	System::Memory::copy(op_pMemoryBuffer->getDirectPointer() + l_ui64CurrentBufferSize, pBuffer, size);
}
