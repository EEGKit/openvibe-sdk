#include "ovpCBoxAlgorithmTimeout.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

boolean CBoxAlgorithmTimeout::initialize(void)
{
	m_bTimeoutReached = false;
	m_bIsFinished = false;
	m_oStimulationEncoder.initialize(*this);
	
	m_ui64Timeout = static_cast<uint64>(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0)) << 32;
	m_ui64StimulationToSend = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_ui64LastTimePolled = 0;
	m_ui64LastEndTime = 0;
	m_bIsHeaderSent = false;

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmTimeout::uninitialize(void)
{
	m_oStimulationEncoder.uninitialize();

	return true;
}
/*******************************************************************************/


boolean CBoxAlgorithmTimeout::processClock(IMessageClock& rMessageClock)
{
	if (m_bTimeoutReached)
	{
		return true;
	}

	// if there was nothing received on the input for a period of time we raise the
	// timeout flag and let the box send a stimulation
	if (this->getPlayerContext().getCurrentTime() > m_ui64LastTimePolled + m_ui64Timeout)
	{
		this->getLogManager() << LogLevel_Trace << "Timeout reached at time " << time64(this->getPlayerContext().getCurrentTime()) << "\n";
		m_bTimeoutReached = true;
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/


uint64 CBoxAlgorithmTimeout::getClockFrequency(void)
{
	return 16LL<<32; // the box clock frequency
}
/*******************************************************************************/


boolean CBoxAlgorithmTimeout::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	// every time we receive input we store the last kernel time
	m_ui64LastTimePolled = this->getPlayerContext().getCurrentTime();

	return true;
}
/*******************************************************************************/

boolean CBoxAlgorithmTimeout::process(void)
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();
	uint64 l_ui64CurrentTime = this->getPlayerContext().getCurrentTime();

	// Discard input data
	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		l_rDynamicBoxContext.markInputAsDeprecated(0, i);
	}

	// If time out has been sent, just leave
	if(m_bIsFinished)
	{
		return true;
	}

	// If the timeout was not send but was reached we send the stimulation on the output 0
	if(m_bTimeoutReached)
	{
		IStimulationSet* l_pStimulationSet = m_oStimulationEncoder.getInputStimulationSet();
		l_pStimulationSet->clear();
		l_pStimulationSet->appendStimulation(m_ui64StimulationToSend, l_ui64CurrentTime, 0);
		m_bIsFinished = true;
	}

	// Encoding the header
	if(!m_bIsHeaderSent)
	{
		m_oStimulationEncoder.encodeHeader(0);
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
		m_bIsHeaderSent = true;
	}

	// Encoding the stimulation in the buffer
	m_oStimulationEncoder.encodeBuffer(0);
	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_ui64LastEndTime, l_ui64CurrentTime);
	m_ui64LastEndTime = l_ui64CurrentTime;

	return true;
}
