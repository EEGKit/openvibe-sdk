#include "ovpCBoxAlgorithmFeatureAggregator.h"

#include <system/ovCMemory.h>

#include <iostream>
#include <cstdio>

using namespace OpenViBE;
using namespace Plugins;
using namespace Kernel;
using namespace OpenViBEPlugins;
using namespace FeatureExtraction;

using namespace OpenViBEToolkit;

using namespace std;

namespace OpenViBEPlugins
{
	namespace FeatureExtraction
	{
		bool CBoxAlgorithmFeatureAggregator::initialize()
		{
			m_ui32NumberOfInput = getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount();

			// Prepares decoders
			for (uint32_t i = 0; i < m_ui32NumberOfInput; i++)
			{
				TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>* streamedMatrixDecoder = new TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>();
				m_pStreamedMatrixDecoder.push_back(streamedMatrixDecoder);
				m_pStreamedMatrixDecoder.back()->initialize(*this, i);
			}
			m_pFeatureVectorEncoder = new TFeatureVectorEncoder<CBoxAlgorithmFeatureAggregator>;
			m_pFeatureVectorEncoder->initialize(*this, 0);

			//resizes everything as needed
			m_oInputBufferSizes.resize(m_ui32NumberOfInput);
			m_oDimensionSize.resize(m_ui32NumberOfInput);
			m_oFeatureNames.resize(m_ui32NumberOfInput);

			m_bHeaderSent = false;

			return true;
		}

		bool CBoxAlgorithmFeatureAggregator::uninitialize()
		{
			for (uint32_t i = 0; i < m_ui32NumberOfInput; i++)
			{
				if (m_pStreamedMatrixDecoder.back())
				{
					m_pStreamedMatrixDecoder.back()->uninitialize();
					delete m_pStreamedMatrixDecoder.back();
					m_pStreamedMatrixDecoder.pop_back();
				}
			}

			if (m_pFeatureVectorEncoder)
			{
				m_pFeatureVectorEncoder->uninitialize();
				delete m_pFeatureVectorEncoder;
			}

			return true;
		}

		bool CBoxAlgorithmFeatureAggregator::processInput(const uint32_t index)
		{
			IBoxIO* l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

			uint64_t lastBufferChunkSize;
			const uint8_t* lastBuffer;

			uint64_t bufferChunkSize;
			const uint8_t* buffer;

			//gets the first buffer from the concerned input
			l_pBoxIO->getInputChunk(index, 0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, lastBufferChunkSize, lastBuffer);

			uint64_t tStart = 0, tEnd = 0;

			bool readyToProcess = true;

			//checks every input's first chunk's dates
			for (uint32_t i = 0; i < m_ui32NumberOfInput && readyToProcess; i++)
			{
				if (l_pBoxIO->getInputChunkCount(i) != 0)
				{
					l_pBoxIO->getInputChunk(i, 0, tStart, tEnd, bufferChunkSize, buffer);
					//if the first buffers don't have the same starting/ending dates, stop
					if (tStart != m_ui64LastChunkStartTime || tEnd != m_ui64LastChunkEndTime) { readyToProcess = false; }

					//checks for problems, buffer lengths differents...
					if (tEnd - tStart != m_ui64LastChunkEndTime - m_ui64LastChunkStartTime)
					{
						//marks everything as deprecated and sends a warning
						for (uint32_t input = 0; input < m_ui32NumberOfInput; input++)
						{
							for (uint32_t chunk = 0; chunk < l_pBoxIO->getInputChunkCount(input); chunk++) { l_pBoxIO->markInputAsDeprecated(input, chunk); }
						}

						readyToProcess = false;

						OV_ERROR_KRF("Invalid incoming input chunks: duration differs between chunks", OpenViBE::Kernel::ErrorType::BadInput);
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

			IMatrix* oMatrix = m_pFeatureVectorEncoder->getInputMatrix();
			std::vector<double> bufferElements;
			uint64_t totalBufferSize = 0;
			bool bufferReceived      = false;

			for (uint32_t input = 0; input < boxContext->getInputCount(); input++)
			{
				m_pStreamedMatrixDecoder[input]->decode(0);
				//*
				if ((m_pStreamedMatrixDecoder[input]->isHeaderReceived()) && !m_bHeaderSent)
				{
					//getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "header " << input << "\n";
					IMatrix* iMatrix = m_pStreamedMatrixDecoder[input]->getOutputMatrix();
					totalBufferSize += iMatrix->getBufferElementCount();
					if (input == boxContext->getInputCount() - 1)
					{
						oMatrix->setDimensionCount(1);
						oMatrix->setDimensionSize(0, uint32_t(totalBufferSize));

						for (uint32_t i = 0; i < uint32_t(totalBufferSize); i++)
						{
							char buffer[64];
							sprintf(buffer, "Feature %d", (i + 1));
							oMatrix->setDimensionLabel(0, i, buffer);
						}

						m_pFeatureVectorEncoder->encodeHeader();
						boxIO->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
						m_bHeaderSent = true;
					}
				}
				//*/
				if (m_pStreamedMatrixDecoder[input]->isBufferReceived())
				{
					bufferReceived      = true;
					IMatrix* iMatrix    = m_pStreamedMatrixDecoder[input]->getOutputMatrix();
					const uint32_t size = iMatrix->getBufferElementCount();

					double* buffer = iMatrix->getBuffer();
					for (uint32_t i = 0; i < size; i++) { bufferElements.push_back(buffer[i]); }
				}
			}

			if (m_bHeaderSent && bufferReceived)
			{
				double* oBuffer = oMatrix->getBuffer();
				for (uint32_t i = 0; i < bufferElements.size(); i++) { oBuffer[i] = bufferElements[i]; }
				m_pFeatureVectorEncoder->encodeBuffer();
				boxIO->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			}

			return true;
		}
	} // namespace FeatureExtraction
} // namespace OpenViBEPlugins
