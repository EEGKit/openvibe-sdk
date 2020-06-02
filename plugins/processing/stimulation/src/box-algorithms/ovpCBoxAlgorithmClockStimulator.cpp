#include "ovpCBoxAlgorithmClockStimulator.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;
using namespace Stimulation;

bool CBoxAlgorithmClockStimulator::initialize()
{
	const double interstimulationInterval = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	const double minInterstimulationInterval = 0.0001;
	OV_ERROR_UNLESS_KRF(!(interstimulationInterval < minInterstimulationInterval),
						"Invalid stimulation interval [" << interstimulationInterval << "] (expected value > " << minInterstimulationInterval << ")",
						Kernel::ErrorType::BadSetting);

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

bool CBoxAlgorithmClockStimulator::processClock(CMessage& /*msg*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmClockStimulator::process()
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();

	const CTime time = getPlayerContext().getCurrentTime();

	CStimulationSet set;
	set.clear();

	while (CTime(double(m_nSentStimulation + 1) * m_stimulationInterval) < time)
	{
		m_nSentStimulation += 1;
		m_lastStimulationDate = CTime(double(m_nSentStimulation) * m_stimulationInterval).time();
		set.append(m_stimulationID, m_lastStimulationDate, 0);
	}

	if (time == CTime(0))
	{
		m_encoder.encodeHeader();
		boxContext.markOutputAsReadyToSend(0, m_lastEndTime, m_lastEndTime);
	}
	m_encoder.getInputStimulationSet() = &set;

	m_encoder.encodeBuffer();
	boxContext.markOutputAsReadyToSend(0, m_lastEndTime, time);

	m_lastEndTime = time;

	return true;
}
