#include "ovpCBoxAlgorithmClockStimulator.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace Stimulation;

bool CBoxAlgorithmClockStimulator::initialize()
{
	const double interstimulationInterval = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	const double minInterstimulationInterval = 0.0001;
	OV_ERROR_UNLESS_KRF(!(interstimulationInterval < minInterstimulationInterval),
						"Invalid stimulation interval [" << interstimulationInterval << "] (expected value > " << minInterstimulationInterval << ")",
						ErrorType::BadSetting);

	m_stimulationInterval = interstimulationInterval;
	m_nSentStimulation    = 0;

	m_stimulationID = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_lastStimulationDate = 0;
	m_lastEndTime         = 0;

	m_encoder.initialize(*this, 0);

	return true;
}

bool CBoxAlgorithmClockStimulator::uninitialize()
{
	m_encoder.uninitialize();
	return true;
}

bool CBoxAlgorithmClockStimulator::processClock(IMessageClock& /*messageClock*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmClockStimulator::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	const uint64_t currentTime = getPlayerContext().getCurrentTime();

	CStimulationSet stimulationSet;
	stimulationSet.setStimulationCount(0);

	while (TimeArithmetics::secondsToTime(double(m_nSentStimulation + 1) * m_stimulationInterval) < currentTime)
	{
		m_nSentStimulation += 1;
		m_lastStimulationDate = TimeArithmetics::secondsToTime(m_nSentStimulation * m_stimulationInterval);
		stimulationSet.appendStimulation(m_stimulationID, m_lastStimulationDate, 0);
	}

	if (currentTime == 0)
	{
		m_encoder.encodeHeader();
		boxContext.markOutputAsReadyToSend(0, m_lastEndTime, m_lastEndTime);
	}
	m_encoder.getInputStimulationSet() = &stimulationSet;

	m_encoder.encodeBuffer();
	boxContext.markOutputAsReadyToSend(0, m_lastEndTime, currentTime);

	m_lastEndTime = currentTime;

	return true;
}
