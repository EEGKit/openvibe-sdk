#include "ovpCBoxAlgorithmClockStimulator.h"
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Stimulation;

uint64_t CBoxAlgorithmClockStimulator::getClockFrequency(void)
{
	return (1LL << 32) * 32;
}

bool CBoxAlgorithmClockStimulator::initialize(void)
{
	double l_f64InterstimulationInterval = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	const double l_f64MinInterstimulationInterval = 0.0001;
	OV_ERROR_UNLESS_KRF(
		!(l_f64InterstimulationInterval < l_f64MinInterstimulationInterval),
		"Invalid stimulation interval [" << l_f64InterstimulationInterval << "] (expected value > " << l_f64MinInterstimulationInterval << ")",
		OpenViBE::Kernel::ErrorType::BadSetting
	);

	m_StimulationInterval  = l_f64InterstimulationInterval;
	m_SentStimulationCount = 0;

	m_ui64StimulationId = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	m_ui64LastStimulationDate = 0;
	m_ui64LastEndTime         = 0;

	m_oStimulationEncoder.initialize(*this, 0);

	return true;
}

bool CBoxAlgorithmClockStimulator::uninitialize(void)
{
	m_oStimulationEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmClockStimulator::processClock(IMessageClock& rMessageClock)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmClockStimulator::process(void)
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	uint64_t l_ui64CurrentTime = getPlayerContext().getCurrentTime();

	CStimulationSet l_oStimulationSet;
	l_oStimulationSet.setStimulationCount(0);

	while (ITimeArithmetics::secondsToTime((m_SentStimulationCount + 1) * m_StimulationInterval) < l_ui64CurrentTime)
	{
		m_SentStimulationCount += 1;
		m_ui64LastStimulationDate = ITimeArithmetics::secondsToTime(m_SentStimulationCount * m_StimulationInterval);
		l_oStimulationSet.appendStimulation(m_ui64StimulationId, m_ui64LastStimulationDate, 0);
	}

	if (l_ui64CurrentTime == 0)
	{
		m_oStimulationEncoder.encodeHeader();
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastEndTime, m_ui64LastEndTime);
	}
	m_oStimulationEncoder.getInputStimulationSet() = &l_oStimulationSet;

	m_oStimulationEncoder.encodeBuffer();
	l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastEndTime, l_ui64CurrentTime);

	m_ui64LastEndTime = l_ui64CurrentTime;

	return true;
}
