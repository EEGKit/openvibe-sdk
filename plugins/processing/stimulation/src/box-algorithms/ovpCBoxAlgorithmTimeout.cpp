#include "ovpCBoxAlgorithmTimeout.h"

#include <cmath>

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {

bool CBoxAlgorithmTimeout::initialize()
{
	m_timeoutState = ETimeoutState::No;

	m_encoder.initialize(*this, 0);

	const double timeout = double(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));
	OV_ERROR_UNLESS_KRF(timeout > 0, "Timeout delay value must be positive and non-zero", Kernel::ErrorType::BadSetting);
	OV_ERROR_UNLESS_KRF(timeout == std::floor(timeout), "Timeout delay value is not an integer", Kernel::ErrorType::BadSetting);

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


bool CBoxAlgorithmTimeout::processClock(Kernel::IMessageClock& /*messageClock*/)
{
	// if there was nothing received on the input for a period of time we raise the
	// timeout flag and let the box send a stimulation
	if (m_timeoutState == ETimeoutState::No && getPlayerContext().getCurrentTime() > m_lastTimePolled + m_timeout)
	{
		this->getLogManager() << Kernel::LogLevel_Trace << "Timeout reached at time " << CTime(this->getPlayerContext().getCurrentTime()) << "\n";
		m_timeoutState = ETimeoutState::Occurred;
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


bool CBoxAlgorithmTimeout::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	// every time we receive input we store the last kernel time
	m_lastTimePolled = this->getPlayerContext().getCurrentTime();

	return true;
}
/*******************************************************************************/

bool CBoxAlgorithmTimeout::process()
{
	Kernel::IBoxIO& boxCtx = this->getDynamicBoxContext();

	// Discard input data
	for (size_t i = 0; i < boxCtx.getInputChunkCount(0); ++i) { boxCtx.markInputAsDeprecated(0, i); }

	// Encoding the header
	if (!m_isHeaderSent)
	{
		m_encoder.encodeHeader();
		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
		m_isHeaderSent = true;
	}

	IStimulationSet* stimSet = m_encoder.getInputStimulationSet();
	stimSet->clear();

	const uint64_t date = this->getPlayerContext().getCurrentTime();

	// If the timeout is reached we send the stimulation on the output 0
	if (m_timeoutState == ETimeoutState::Occurred)
	{
		stimSet->appendStimulation(m_stimulationToSend, date, 0);
		m_timeoutState = ETimeoutState::Sent;
	}

	// we need to send an empty chunk even if there's no stim
	m_encoder.encodeBuffer();
	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_previousTime, date);

	m_previousTime = date;

	return true;
}

} // namespace Stimulation
} // namespace Plugins
} // namespace OpenViBE
