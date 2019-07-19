#include "ovpCBoxAlgorithmTimeSignalGenerator.h"

#include <iostream>
#include <cmath>
#include <cstdlib>

#include <openvibe/ovITimeArithmetics.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace DataGeneration;
using namespace OpenViBEToolkit;
using namespace std;

CBoxAlgorithmTimeSignalGenerator::CBoxAlgorithmTimeSignalGenerator()
	: m_bHeaderSent(false)
	  , m_ui32SamplingFrequency(0)
	  , m_ui32GeneratedEpochSampleCount(0)
	  , m_ui32SentSampleCount(0) {}

void CBoxAlgorithmTimeSignalGenerator::release()
{
	delete this;
}

bool CBoxAlgorithmTimeSignalGenerator::initialize()
{
	m_oSignalEncoder.initialize(*this, 0);

	// Parses box settings to try connecting to server
	m_ui32SamplingFrequency         = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui32GeneratedEpochSampleCount = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_bHeaderSent                   = false;

	m_ui32SentSampleCount = 0;

	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::uninitialize()
{
	m_oSignalEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::processClock(CMessageClock& rMessageClock)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::process()
{
	IBoxIO* l_pDynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	if (!m_bHeaderSent)
	{
		m_oSignalEncoder.getInputSamplingRate() = m_ui32SamplingFrequency;

		IMatrix* l_pMatrix = m_oSignalEncoder.getInputMatrix();

		l_pMatrix->setDimensionCount(2);
		l_pMatrix->setDimensionSize(0, 1);
		l_pMatrix->setDimensionSize(1, m_ui32GeneratedEpochSampleCount);
		l_pMatrix->setDimensionLabel(0, 0, "Time signal");

		m_oSignalEncoder.encodeHeader();

		m_bHeaderSent = true;

		l_pDynamicBoxContext->markOutputAsReadyToSend(0, 0, 0);
	}
	else
	{

		// Create sample chunks up until the next step (current time + 1/128) but do not overshoot it
		// This way we will always create the correct number of samples for frequencies that are above 128Hz
		uint64_t nextStepDate = ITimeArithmetics::timeToSampleCount(static_cast<uint64_t>(m_ui32SamplingFrequency), static_cast<uint64_t>(this->getPlayerContext().getCurrentTime() + (1ULL << 25)));
		while (m_ui32SentSampleCount + m_ui32GeneratedEpochSampleCount < nextStepDate)
		{
			double* l_pSampleBuffer = m_oSignalEncoder.getInputMatrix()->getBuffer();

			for (uint32_t i = 0; i < m_ui32GeneratedEpochSampleCount; i++)
			{
				l_pSampleBuffer[i] = (i + m_ui32SentSampleCount) / static_cast<double>(m_ui32SamplingFrequency);
			}

			m_oSignalEncoder.encodeBuffer();

			uint64_t l_ui64StartTime = ITimeArithmetics::sampleCountToTime(m_ui32SamplingFrequency, m_ui32SentSampleCount);
			m_ui32SentSampleCount += m_ui32GeneratedEpochSampleCount;
			uint64_t l_ui64EndTime = ITimeArithmetics::sampleCountToTime(m_ui32SamplingFrequency, m_ui32SentSampleCount);

			l_pDynamicBoxContext->markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
		}
	}

	return true;
}

uint64_t CBoxAlgorithmTimeSignalGenerator::getClockFrequency()
{
	return 128LL << 32;
}
