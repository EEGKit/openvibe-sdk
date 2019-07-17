#include "ovpCBoxAlgorithmStreamEndDetector.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

bool CBoxAlgorithmStreamEndDetector::initialize(void)
{
	m_StimulationIdentifier = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), id_SettingStimulationName());

	OV_FATAL_UNLESS_K(this->getStaticBoxContext().getInterfacorIndex(BoxInterfacorType::Input, id_InputEBML(), m_InputEBMLIndex),
					  "Box does not have input with identifier " << id_InputEBML(),
					  ErrorType::Internal);
	OV_FATAL_UNLESS_K(this->getStaticBoxContext().getInterfacorIndex(BoxInterfacorType::Output, id_OutputStimulations(), m_OutputStimulationsIndex),
					  "Box does not have output with identifier " << id_OutputStimulations(),
					  ErrorType::Internal);

	m_StructureDecoder.initialize(*this, m_InputEBMLIndex);
	m_StimulationEncoder.initialize(*this, m_OutputStimulationsIndex);

	m_IsHeaderSent        = false;
	m_EndDate             = 0;
	m_CurrentChunkEndDate = 0;
	m_EndState            = EEndState::WaitingForEnd;
	m_PreviousTime        = 0;

	return true;
}

bool CBoxAlgorithmStreamEndDetector::uninitialize(void)
{
	m_StructureDecoder.uninitialize();
	m_StimulationEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmStreamEndDetector::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStreamEndDetector::process(void)
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(m_InputEBMLIndex); chunk++)
	{
		OV_ERROR_UNLESS_KRF(m_StructureDecoder.decode(chunk),
							"Failed to decode chunk",
							ErrorType::Internal);

		// We can not receive anything before this date anymore, thus we can send an empty stream
		m_CurrentChunkEndDate = dynamicBoxContext.getInputChunkStartTime(m_InputEBMLIndex, chunk);
		if (m_StructureDecoder.isEndReceived())
		{
			m_EndState = EEndState::EndReceived;
			m_EndDate  = dynamicBoxContext.getInputChunkEndTime(m_InputEBMLIndex, chunk);
			// As this is the last chunk, we make it so it ends at the same time as the received End chunk
			m_CurrentChunkEndDate = dynamicBoxContext.getInputChunkEndTime(m_InputEBMLIndex, chunk);
		}
	}

	if (!m_IsHeaderSent)
	{
		m_StimulationEncoder.encodeHeader();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
		m_IsHeaderSent = true;
	}

	IStimulationSet* stimulationSet = m_StimulationEncoder.getInputStimulationSet();
	stimulationSet->clear();


	// If the timeout is reached we send the stimulation on the output 0
	if (m_EndState == EEndState::EndReceived)
	{
		stimulationSet->appendStimulation(m_StimulationIdentifier, m_EndDate, 0);
		m_EndState = EEndState::StimulationSent;
	}

	if (m_EndState != EEndState::Finished && (m_EndState == EEndState::StimulationSent || m_PreviousTime != m_CurrentChunkEndDate))
	{
		// we need to send an empty chunk even if there's no stim
		m_StimulationEncoder.encodeBuffer();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_PreviousTime, m_CurrentChunkEndDate);
	}

	m_PreviousTime = m_CurrentChunkEndDate;

	if (m_EndState == EEndState::StimulationSent)
	{
		m_StimulationEncoder.encodeEnd();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_PreviousTime, m_PreviousTime);
		m_EndState = EEndState::Finished;
	}

	return true;
}
