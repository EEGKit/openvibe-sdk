#include "ovpCBoxAlgorithmFeatureAggregator.h"
#include <iostream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace FeatureExtraction;

using namespace /*OpenViBE::*/Toolkit;

using namespace std;

namespace OpenViBE {
namespace Plugins {
namespace FeatureExtraction {

bool CBoxAlgorithmFeatureAggregator::initialize()
{
	m_nInput = getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount();

	// Prepares decoders
	for (size_t i = 0; i < m_nInput; ++i)
	{
		TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>* streamedMatrixDecoder = new TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>
				();
		m_decoder.push_back(streamedMatrixDecoder);
		m_decoder.back()->initialize(*this, i);
	}
	m_encoder = new TFeatureVectorEncoder<CBoxAlgorithmFeatureAggregator>;
	m_encoder->initialize(*this, 0);

	//resizes everything as needed
	m_iBufferSizes.resize(m_nInput);
	m_dimSize.resize(m_nInput);
	m_featureNames.resize(m_nInput);

	m_headerSent = false;

	return true;
}

bool CBoxAlgorithmFeatureAggregator::uninitialize()
{
	for (size_t i = 0; i < m_nInput; ++i)
	{
		if (m_decoder.back())
		{
			m_decoder.back()->uninitialize();
			delete m_decoder.back();
			m_decoder.pop_back();
		}
	}

	if (m_encoder)
	{
		m_encoder->uninitialize();
		delete m_encoder;
	}

	return true;
}

bool CBoxAlgorithmFeatureAggregator::processInput(const size_t index)
{
	IBoxIO* boxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

	size_t lastBufferChunkSize;
	const uint8_t* lastBuffer;

	size_t bufferChunkSize;
	const uint8_t* buffer;

	//gets the first buffer from the concerned input
	boxIO->getInputChunk(index, 0, m_lastChunkStartTime, m_lastChunkEndTime, lastBufferChunkSize, lastBuffer);

	CTime tStart = 0, tEnd = 0;

	bool readyToProcess = true;

	//checks every input's first chunk's dates
	for (size_t i = 0; i < m_nInput && readyToProcess; ++i)
	{
		if (boxIO->getInputChunkCount(i) != 0)
		{
			boxIO->getInputChunk(i, 0, tStart, tEnd, bufferChunkSize, buffer);
			//if the first buffers don't have the same starting/ending dates, stop
			if (tStart != m_lastChunkStartTime || tEnd != m_lastChunkEndTime) { readyToProcess = false; }

			//checks for problems, buffer lengths differents...
			if (tEnd - tStart != m_lastChunkEndTime - m_lastChunkStartTime)
			{
				//marks everything as deprecated and sends a error
				for (size_t input = 0; input < m_nInput; ++input)
				{
					for (size_t chunk = 0; chunk < boxIO->getInputChunkCount(input); ++chunk) { boxIO->markInputAsDeprecated(input, chunk); }
				}

				//readyToProcess = false;
				OV_ERROR_KRF("Invalid incoming input chunks: duration differs between chunks", Kernel::ErrorType::BadInput);
			}
		}
		else { readyToProcess = false; }
	}

	//If there is one buffer of the same time period per input, process
	if (readyToProcess) { getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(); }

	return true;
}

bool CBoxAlgorithmFeatureAggregator::process()
{
	const IBox* boxContext = getBoxAlgorithmContext()->getStaticBoxContext();
	IBoxIO* boxIO          = getBoxAlgorithmContext()->getDynamicBoxContext();

	CMatrix* oMatrix = m_encoder->getInputMatrix();
	std::vector<double> bufferElements;
	size_t totalBufferSize = 0;
	bool bufferReceived    = false;

	for (size_t input = 0; input < boxContext->getInputCount(); ++input)
	{
		m_decoder[input]->decode(0);
		//*
		if ((m_decoder[input]->isHeaderReceived()) && !m_headerSent)
		{
			//getLogManager() << LogLevel_Warning << "header " << input << "\n";
			CMatrix* iMatrix = m_decoder[input]->getOutputMatrix();
			totalBufferSize += iMatrix->getSize();
			if (input == boxContext->getInputCount() - 1)
			{
				oMatrix->resize(totalBufferSize);
				for (size_t i = 0; i < totalBufferSize; ++i) { oMatrix->setDimensionLabel(0, i, ("Feature " + std::to_string(i + 1)).c_str()); }

				m_encoder->encodeHeader();
				boxIO->markOutputAsReadyToSend(0, m_lastChunkStartTime, m_lastChunkEndTime);
				m_headerSent = true;
			}
		}
		//*/
		if (m_decoder[input]->isBufferReceived())
		{
			bufferReceived    = true;
			CMatrix* iMatrix  = m_decoder[input]->getOutputMatrix();
			const size_t size = iMatrix->getSize();

			double* buffer = iMatrix->getBuffer();
			for (size_t i = 0; i < size; ++i) { bufferElements.push_back(buffer[i]); }
		}
	}

	if (m_headerSent && bufferReceived)
	{
		double* oBuffer = oMatrix->getBuffer();
		for (size_t i = 0; i < bufferElements.size(); ++i) { oBuffer[i] = bufferElements[i]; }
		m_encoder->encodeBuffer();
		boxIO->markOutputAsReadyToSend(0, m_lastChunkStartTime, m_lastChunkEndTime);
	}

	return true;
}

}  // namespace FeatureExtraction
}  // namespace Plugins
}  // namespace OpenViBE
