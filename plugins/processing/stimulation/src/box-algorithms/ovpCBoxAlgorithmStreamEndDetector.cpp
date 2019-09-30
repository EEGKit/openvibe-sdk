#include "ovpCBoxAlgorithmStreamEndDetector.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Stimulation;

bool CBoxAlgorithmStreamEndDetector::initialize()
{
	m_stimulationID = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), settingStimulationNameID());

	OV_FATAL_UNLESS_K(this->getStaticBoxContext().getInterfacorIndex(EBoxInterfacorType::Input, inputEBMLId(), m_inputEBMLIdx),
					  "Box does not have input with identifier " << inputEBMLId(), ErrorType::Internal);
	OV_FATAL_UNLESS_K(this->getStaticBoxContext().getInterfacorIndex(EBoxInterfacorType::Output, outputStimulationsID(), m_outputStimulationsIdx),
					  "Box does not have output with identifier " << outputStimulationsID(), ErrorType::Internal);

	m_decoder.initialize(*this, m_inputEBMLIdx);
	m_encoder.initialize(*this, m_outputStimulationsIdx);

	m_isHeaderSent        = false;
	m_endDate             = 0;
	m_currentChunkEndDate = 0;
	m_endState            = EEndState::WaitingForEnd;
	m_previousTime        = 0;

	return true;
}

bool CBoxAlgorithmStreamEndDetector::uninitialize()
{
	m_decoder.uninitialize();
	m_encoder.uninitialize();

	return true;
}

bool CBoxAlgorithmStreamEndDetector::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStreamEndDetector::process()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(m_inputEBMLIdx); chunk++)
	{
		OV_ERROR_UNLESS_KRF(m_decoder.decode(chunk), "Failed to decode chunk", ErrorType::Internal);

		// We can not receive anything before this date anymore, thus we can send an empty stream
		m_currentChunkEndDate = dynamicBoxContext.getInputChunkStartTime(m_inputEBMLIdx, chunk);
		if (m_decoder.isEndReceived())
		{
			m_endState = EEndState::EndReceived;
			m_endDate  = dynamicBoxContext.getInputChunkEndTime(m_inputEBMLIdx, chunk);
			// As this is the last chunk, we make it so it ends at the same time as the received End chunk
			m_currentChunkEndDate = dynamicBoxContext.getInputChunkEndTime(m_inputEBMLIdx, chunk);
		}
	}

	if (!m_isHeaderSent)
	{
		m_encoder.encodeHeader();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
		m_isHeaderSent = true;
	}

	IStimulationSet* stimulationSet = m_encoder.getInputStimulationSet();
	stimulationSet->clear();


	// If the timeout is reached we send the stimulation on the output 0
	if (m_endState == EEndState::EndReceived)
	{
		stimulationSet->appendStimulation(m_stimulationID, m_endDate, 0);
		m_endState = EEndState::StimulationSent;
	}

	if (m_endState != EEndState::Finished && (m_endState == EEndState::StimulationSent || m_previousTime != m_currentChunkEndDate))
	{
		// we need to send an empty chunk even if there's no stim
		m_encoder.encodeBuffer();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_previousTime, m_currentChunkEndDate);
	}

	m_previousTime = m_currentChunkEndDate;

	if (m_endState == EEndState::StimulationSent)
	{
		m_encoder.encodeEnd();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_previousTime, m_previousTime);
		m_endState = EEndState::Finished;
	}

	return true;
}
