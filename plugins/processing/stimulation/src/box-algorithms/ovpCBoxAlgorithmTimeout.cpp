#include "ovpCBoxAlgorithmTimeout.h"

#include <cmath>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Stimulation;

bool CBoxAlgorithmTimeout::initialize()
{
	m_TimeoutState = ETimeout_No;

	m_StimulationEncoder.initialize(*this, 0);

	const double timeout = double(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));
	OV_ERROR_UNLESS_KRF(timeout > 0,
						"Timeout delay value must be positive and non-zero",
						ErrorType::BadSetting);
	OV_ERROR_UNLESS_KRF(timeout == std::floor(timeout),
						"Timeout delay value is not an integer",
						ErrorType::BadSetting);

	m_Timeout           = uint64_t(timeout) << 32;
	m_StimulationToSend = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_LastTimePolled = 0;
	m_PreviousTime   = 0;
	m_IsHeaderSent   = false;

	return true;
}
/*******************************************************************************/

bool CBoxAlgorithmTimeout::uninitialize()
{
	m_StimulationEncoder.uninitialize();

	return true;
}
/*******************************************************************************/


bool CBoxAlgorithmTimeout::processClock(IMessageClock& messageClock)
{
	// if there was nothing received on the input for a period of time we raise the
	// timeout flag and let the box send a stimulation
	if (m_TimeoutState == ETimeout_No && getPlayerContext().getCurrentTime() > m_LastTimePolled + m_Timeout)
	{
		this->getLogManager() << LogLevel_Trace << "Timeout reached at time " << time64(this->getPlayerContext().getCurrentTime()) << "\n";
		m_TimeoutState = ETimeout_Occurred;
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}
/*******************************************************************************/


uint64_t CBoxAlgorithmTimeout::getClockFrequency()
{
	return 16LL << 32; // the box clock frequency
}
/*******************************************************************************/


bool CBoxAlgorithmTimeout::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	// every time we receive input we store the last kernel time
	m_LastTimePolled = this->getPlayerContext().getCurrentTime();

	return true;
}
/*******************************************************************************/

bool CBoxAlgorithmTimeout::process()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	// Discard input data
	for (uint32_t i = 0; i < dynamicBoxContext.getInputChunkCount(0); i++)
	{
		dynamicBoxContext.markInputAsDeprecated(0, i);
	}

	// Encoding the header
	if (!m_IsHeaderSent)
	{
		m_StimulationEncoder.encodeHeader();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
		m_IsHeaderSent = true;
	}

	IStimulationSet* stimulationSet = m_StimulationEncoder.getInputStimulationSet();
	stimulationSet->clear();

	const uint64_t stimulationDate = this->getPlayerContext().getCurrentTime();

	// If the timeout is reached we send the stimulation on the output 0
	if (m_TimeoutState == ETimeout_Occurred)
	{
		stimulationSet->appendStimulation(m_StimulationToSend, stimulationDate, 0);
		m_TimeoutState = ETimeout_Sent;
	}

	// we need to send an empty chunk even if there's no stim
	m_StimulationEncoder.encodeBuffer();
	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_PreviousTime, stimulationDate);

	m_PreviousTime = stimulationDate;

	return true;
}
