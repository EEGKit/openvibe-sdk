#include "ovpCBoxAlgorithmSignalAverage.h"

#include <cmath>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;
using namespace /*OpenViBE::*/Toolkit;
using namespace std;

void CBoxAlgorithmSignalAverage::computeAverage()
{
	const double* input = m_decoder.getOutputMatrix()->getBuffer();
	double* output      = m_encoder.getInputMatrix()->getBuffer();

	const size_t nChannel = m_decoder.getOutputMatrix()->getDimensionSize(0);
	const size_t nSample  = m_decoder.getOutputMatrix()->getDimensionSize(1);

	//computes and stores the average for each channel
	for (size_t c = 0; c < nChannel; ++c)
	{
		double sum = 0;
		for (size_t i = 0; i < nSample; ++i) { sum += input[(c * nSample) + i]; }
		output[c] = sum / nSample;
	}
}


bool CBoxAlgorithmSignalAverage::initialize()
{
	m_decoder.initialize(*this, 0);
	m_encoder.initialize(*this, 0);
	return true;
}

bool CBoxAlgorithmSignalAverage::uninitialize()
{
	m_encoder.uninitialize();
	m_decoder.uninitialize();
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
	for (size_t i = 0; i < boxContext->getInputChunkCount(0); ++i)
	{
		m_decoder.decode(i);

		if (m_decoder.isHeaderReceived())
		{
			// Construct the properties of the output stream
			const IMatrix* iMatrix = m_decoder.getOutputMatrix();
			IMatrix* oMatrix       = m_encoder.getInputMatrix();

			// Sampling rate will be decimated in the output
			const uint64_t iSampling   = m_decoder.getOutputSamplingRate();
			const size_t iSampleCount  = iMatrix->getDimensionSize(1);
			const uint64_t newSampling = uint64_t(ceil(double(iSampling) / double(iSampleCount)));

			m_encoder.getInputSamplingRate() = newSampling;

			// We keep the number of channels, but the output chunk size will be 1
			oMatrix->setDimensionCount(2);
			oMatrix->setDimensionSize(0, iMatrix->getDimensionSize(0));
			oMatrix->setDimensionSize(1, 1);

			for (size_t j = 0; j < oMatrix->getDimensionSize(0); ++j) { oMatrix->setDimensionLabel(0, j, iMatrix->getDimensionLabel(0, j)); }

			m_encoder.encodeHeader();
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, 0, 0);
		}

		if (m_decoder.isBufferReceived())
		{
			const uint64_t tStart = boxContext->getInputChunkStartTime(0, i);
			const uint64_t tEnd   = boxContext->getInputChunkEndTime(0, i);

			computeAverage();
			
			m_encoder.encodeBuffer();
			getBoxAlgorithmContext()->getDynamicBoxContext()->markOutputAsReadyToSend(0, tStart, tEnd);
		}
		// if (m_decoder.isEndReceived()) { }	// NOP
	}

	return true;
}
