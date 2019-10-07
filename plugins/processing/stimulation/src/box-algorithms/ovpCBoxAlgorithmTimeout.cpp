#include "ovpCBoxAlgorithmTimeout.h"

#include <cmath>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Stimulation;

bool CBoxAlgorithmTimeout::initialize()
{
	m_timeoutState = Timeout_No;

	m_encoder.initialize(*this, 0);

	const double timeout = double(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));
	OV_ERROR_UNLESS_KRF(timeout > 0,
						"Timeout delay value must be positive and non-zero",
						ErrorType::BadSetting);
	OV_ERROR_UNLESS_KRF(timeout == std::floor(timeout),
						"Timeout delay value is not an integer",
						ErrorType::BadSetting);

	m_timeout           = uint64_t(timeout) << 32;
	m_stimulationToSend = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_lastTimePolled = 0;
	m_previousTime   = 0;
	m_isHeaderSent   = false;

	return true;
}
/*******************************************************************************/

bool CBoxAlgorithmTimeout::uninitialize()
{
	m_encoder.uninitialize();

	return true;
}
/*******************************************************************************/


bool CBoxAlgorithmTimeout::processClock(IMessageClock& /*messageClock*/)
{
	// if there was nothing received on the input for a period of time we raise the
	// timeout flag and let the box send a stimulation
	if (m_timeoutState == Timeout_No && getPlayerContext().getCurrentTime() > m_lastTimePolled + m_timeout)
	{
		this->getLogManager() << LogLevel_Trace << "Timeout reached at time " << time64(this->getPlayerContext().getCurrentTime()) << "\n";
		m_timeoutState = Timeout_Occurred;
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
	m_lastTimePolled = this->getPlayerContext().getCurrentTime();

	return true;
}
/*******************************************************************************/

bool CBoxAlgorithmTimeout::process()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	// Discard input data
	for (uint32_t i = 0; i < dynamicBoxContext.getInputChunkCount(0); ++i) { dynamicBoxContext.markInputAsDeprecated(0, i); }

	// Encoding the header
	if (!m_isHeaderSent)
	{
		m_encoder.encodeHeader();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
		m_isHeaderSent = true;
	}

	IStimulationSet* stimulationSet = m_encoder.getInputStimulationSet();
	stimulationSet->clear();

	const uint64_t stimulationDate = this->getPlayerContext().getCurrentTime();

	// If the timeout is reached we send the stimulation on the output 0
	if (m_timeoutState == Timeout_Occurred)
	{
		stimulationSet->appendStimulation(m_stimulationToSend, stimulationDate, 0);
		m_timeoutState = Timeout_Sent;
	}

	// we need to send an empty chunk even if there's no stim
	m_encoder.encodeBuffer();
	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_previousTime, stimulationDate);

	m_previousTime = stimulationDate;

	return true;
}
