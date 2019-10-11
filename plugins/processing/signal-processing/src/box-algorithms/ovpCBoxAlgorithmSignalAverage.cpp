#include "ovpCBoxAlgorithmSignalAverage.h"

#include <cmath>

using namespace OpenViBE;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

void CBoxAlgorithmSignalAverage::computeAverage()
{
	const double* input = m_Decoder.getOutputMatrix()->getBuffer();
	double* output      = m_Encoder.getInputMatrix()->getBuffer();

	const uint32_t nChannel = m_Decoder.getOutputMatrix()->getDimensionSize(0);
	const uint32_t nSample  = m_Decoder.getOutputMatrix()->getDimensionSize(1);

	//for each channel
	for (uint32_t c = 0; c < nChannel; ++c)
	{
		double sum = 0;

		//sum its samples
		for (uint32_t i = 0; i < nSample; ++i) { sum += input[(c * nSample) + i]; }

		//computes and stores the average for a channel
		output[c] = sum / nSample;
	}
}


bool CBoxAlgorithmSignalAverage::initialize()
{
	m_Decoder.initialize(*this, 0);
	m_Encoder.initialize(*this, 0);

	return true;
}

bool CBoxAlgorithmSignalAverage::uninitialize()
{
	m_Encoder.uninitialize();
	m_Decoder.uninitialize();

	return true;
}

bool CBoxAlgorithmSignalAverage::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalAverage::process()
{
	IDynamicBoxContext* boxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for (uint32_t i = 0; i < boxContext->getInputChunkCount(0); ++i)
	{
		m_Decoder.decode(i);

		if (m_Decoder.isHeaderReceived())
		{
			// Construct the properties of the output stream
			const IMatrix* iMatrix = m_Decoder.getOutputMatrix();
			IMatrix* oMatrix       = m_Encoder.getInputMatrix();

			// Sampling rate will be decimated in the output
			const uint64_t inputSamplingRate = m_Decoder.getOutputSamplingRate();
			const uint32_t inputSampleCount  = iMatrix->getDimensionSize(1);
			const uint64_t newSamplingRate   = uint64_t(ceil(double(inputSamplingRate) / double(inputSampleCount)));

			m_Encoder.getInputSamplingRate() = newSamplingRate;

			// We keep the number of channels, but the output chunk size will be 1
			oMatrix->setDimensionCount(2);
			oMatrix->setDimensionSize(0, iMatrix->getDimensionSize(0));
			oMatrix->setDimensionSize(1, 1);

			for (uint32_t j = 0; j < oMatrix->getDimensionSize(0); ++j) { oMatrix->setDimensionLabel(0, j, iMatrix->getDimensionLabel(0, j)); }

			m_Encoder.encodeHeader();

			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);
		}

		if (m_Decoder.isBufferReceived())
		{
			const uint64_t tStart = boxContext->getInputChunkStartTime(0, i);
			const uint64_t tEnd   = boxContext->getInputChunkEndTime(0, i);

			computeAverage();

			m_Encoder.encodeBuffer();

			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, tStart, tEnd);
		}

		if (m_Decoder.isEndReceived())
		{
			// NOP
		}
	}

	return true;
}
