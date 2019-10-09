#include "ovpCBoxAlgorithmTimeSignalGenerator.h"

#include <openvibe/ovTimeArithmetics.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace DataGeneration;
using namespace OpenViBEToolkit;
using namespace std;

CBoxAlgorithmTimeSignalGenerator::CBoxAlgorithmTimeSignalGenerator() {}

void CBoxAlgorithmTimeSignalGenerator::release() { delete this; }

bool CBoxAlgorithmTimeSignalGenerator::initialize()
{
	m_oSignalEncoder.initialize(*this, 0);

	// Parses box settings to try connecting to server
	m_samplingFrequency         = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_nGeneratedEpochSample = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_headerSent                   = false;

	m_nSentSample = 0;

	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::uninitialize()
{
	m_oSignalEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::processClock(CMessageClock& /*messageClock*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::process()
{
	IBoxIO* l_pDynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	if (!m_headerSent)
	{
		m_oSignalEncoder.getInputSamplingRate() = m_samplingFrequency;

		IMatrix* l_pMatrix = m_oSignalEncoder.getInputMatrix();

		l_pMatrix->setDimensionCount(2);
		l_pMatrix->setDimensionSize(0, 1);
		l_pMatrix->setDimensionSize(1, m_nGeneratedEpochSample);
		l_pMatrix->setDimensionLabel(0, 0, "Time signal");

		m_oSignalEncoder.encodeHeader();

		m_headerSent = true;

		l_pDynamicBoxContext->markOutputAsReadyToSend(0, 0, 0);
	}
	else
	{

		// Create sample chunks up until the next step (current time + 1/128) but do not overshoot it
		// This way we will always create the correct number of samples for frequencies that are above 128Hz
		const uint64_t nextStepDate = TimeArithmetics::timeToSampleCount(uint64_t(m_samplingFrequency), uint64_t(this->getPlayerContext().getCurrentTime() + (1ULL << 25)));
		while (m_nSentSample + m_nGeneratedEpochSample < nextStepDate)
		{
			double* l_pSampleBuffer = m_oSignalEncoder.getInputMatrix()->getBuffer();

			for (uint32_t i = 0; i < m_nGeneratedEpochSample; ++i)
			{
				l_pSampleBuffer[i] = (i + m_nSentSample) / double(m_samplingFrequency);
			}

			m_oSignalEncoder.encodeBuffer();

			const uint64_t tStart = TimeArithmetics::sampleCountToTime(m_samplingFrequency, m_nSentSample);
			m_nSentSample += m_nGeneratedEpochSample;
			const uint64_t tEnd = TimeArithmetics::sampleCountToTime(m_samplingFrequency, m_nSentSample);

			l_pDynamicBoxContext->markOutputAsReadyToSend(0, tStart, tEnd);
		}
	}

	return true;
}

uint64_t CBoxAlgorithmTimeSignalGenerator::getClockFrequency() { return 128LL << 32; }
