#include "ovpCBoxAlgorithmSignalAverage.h"

#include <cmath>
#include <iostream>
#include <sstream>

using namespace OpenViBE;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

void CBoxAlgorithmSignalAverage::computeAverage()
{
	const double* l_pInput = m_oSignalDecoder.getOutputMatrix()->getBuffer();
	double* l_pOutput      = m_oSignalEncoder.getInputMatrix()->getBuffer();

	const uint32_t l_ui32ChannelCount = m_oSignalDecoder.getOutputMatrix()->getDimensionSize(0);
	const uint32_t l_ui32SampleCount  = m_oSignalDecoder.getOutputMatrix()->getDimensionSize(1);

	//for each channel
	for (uint32_t c = 0; c < l_ui32ChannelCount; c++)
	{
		double l_f64SamplesSum = 0;

		//sum its samples
		for (uint32_t i = 0; i < l_ui32SampleCount; i++)
		{
			l_f64SamplesSum += l_pInput[(c * l_ui32SampleCount) + i];
		}

		//computes and stores the average for a channel
		l_pOutput[c] = l_f64SamplesSum / l_ui32SampleCount;
	}
}


CBoxAlgorithmSignalAverage::CBoxAlgorithmSignalAverage() {}

void CBoxAlgorithmSignalAverage::release() {}

bool CBoxAlgorithmSignalAverage::initialize()
{
	m_oSignalDecoder.initialize(*this, 0);
	m_oSignalEncoder.initialize(*this, 0);

	return true;
}

bool CBoxAlgorithmSignalAverage::uninitialize()
{
	m_oSignalEncoder.uninitialize();
	m_oSignalDecoder.uninitialize();

	return true;
}

bool CBoxAlgorithmSignalAverage::processInput(uint32_t ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalAverage::process()
{
	IDynamicBoxContext* l_pDynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for (uint32_t i = 0; i < l_pDynamicBoxContext->getInputChunkCount(0); i++)
	{
		m_oSignalDecoder.decode(i);

		if (m_oSignalDecoder.isHeaderReceived())
		{
			// Construct the properties of the output stream
			const IMatrix* l_pInputMatrix = m_oSignalDecoder.getOutputMatrix();
			IMatrix* l_pOutputMatrix      = m_oSignalEncoder.getInputMatrix();

			// Sampling rate will be decimated in the output
			const uint64_t l_ui64InputSamplingRate = m_oSignalDecoder.getOutputSamplingRate();
			const uint32_t l_ui32InputSampleCount  = l_pInputMatrix->getDimensionSize(1);
			const uint64_t l_ui64NewSamplingRate   = static_cast<uint64_t>(ceil((double)l_ui64InputSamplingRate / (double)l_ui32InputSampleCount));

			m_oSignalEncoder.getInputSamplingRate() = l_ui64NewSamplingRate;

			// We keep the number of channels, but the output chunk size will be 1
			l_pOutputMatrix->setDimensionCount(2);
			l_pOutputMatrix->setDimensionSize(0, l_pInputMatrix->getDimensionSize(0));
			l_pOutputMatrix->setDimensionSize(1, 1);

			for (uint32_t j = 0; j < l_pOutputMatrix->getDimensionSize(0); j++)
			{
				l_pOutputMatrix->setDimensionLabel(0, j, l_pInputMatrix->getDimensionLabel(0, j));
			}

			m_oSignalEncoder.encodeHeader();

			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);
		}

		if (m_oSignalDecoder.isBufferReceived())
		{
			const uint64_t l_ui64LastChunkStartTime = l_pDynamicBoxContext->getInputChunkStartTime(0, i);
			const uint64_t l_ui64LastChunkEndTime   = l_pDynamicBoxContext->getInputChunkEndTime(0, i);

			computeAverage();

			m_oSignalEncoder.encodeBuffer();

			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, l_ui64LastChunkStartTime, l_ui64LastChunkEndTime);
		}

		if (m_oSignalDecoder.isEndReceived())
		{
			// NOP
		}
	}

	return true;
}
