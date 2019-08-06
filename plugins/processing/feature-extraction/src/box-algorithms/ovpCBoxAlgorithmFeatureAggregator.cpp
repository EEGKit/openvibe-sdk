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
		CBoxAlgorithmFeatureAggregator::CBoxAlgorithmFeatureAggregator() : m_pFeatureVectorEncoder(nullptr),
																		   m_ui64LastChunkStartTime(0),
																		   m_ui64LastChunkEndTime(0),
																		   m_ui32CurrentInput(0),
																		   m_pVectorBuffer(nullptr),
																		   m_ui32VectorSize(0),
																		   m_bHeaderSent(false) { }

		bool CBoxAlgorithmFeatureAggregator::initialize()
		{
			m_ui32NumberOfInput = getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount();

			// Prepares decoders
			for (uint32_t i = 0; i < m_ui32NumberOfInput; i++)
			{
				TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>* l_pStreamedMatrixDecoder = new TStreamedMatrixDecoder<CBoxAlgorithmFeatureAggregator>();
				m_pStreamedMatrixDecoder.push_back(l_pStreamedMatrixDecoder);
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

		bool CBoxAlgorithmFeatureAggregator::processInput(const uint32_t ui32InputIndex)
		{
			IBoxIO* l_pBoxIO = getBoxAlgorithmContext()->getDynamicBoxContext();

			uint64_t l_ui64LastBufferChunkSize;
			const uint8_t* l_pLastBuffer;

			uint64_t l_ui64CurrentBufferChunkSize;
			const uint8_t* l_pCurrentBuffer;

			//gets the first buffer from the concerned input
			l_pBoxIO->getInputChunk(ui32InputIndex, 0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime, l_ui64LastBufferChunkSize, l_pLastBuffer);

			uint64_t l_ui64StartTime = 0;
			uint64_t l_ui64EndTime   = 0;

			bool l_bReadyToProcess = true;

			//checks every input's first chunk's dates
			for (uint32_t i = 0; i < m_ui32NumberOfInput && l_bReadyToProcess; i++)
			{
				if (l_pBoxIO->getInputChunkCount(i) != 0)
				{
					l_pBoxIO->getInputChunk(i, 0, l_ui64StartTime, l_ui64EndTime, l_ui64CurrentBufferChunkSize, l_pCurrentBuffer);
					//if the first buffers don't have the same starting/ending dates, stop
					if (l_ui64StartTime != m_ui64LastChunkStartTime || l_ui64EndTime != m_ui64LastChunkEndTime)
					{
						l_bReadyToProcess = false;
					}

					//checks for problems, buffer lengths differents...
					if (l_ui64EndTime - l_ui64StartTime != m_ui64LastChunkEndTime - m_ui64LastChunkStartTime)
					{
						//marks everything as deprecated and sends a warning
						for (uint32_t input = 0; input < m_ui32NumberOfInput; input++)
						{
							for (uint32_t chunk = 0; chunk < l_pBoxIO->getInputChunkCount(input); chunk++)
							{
								l_pBoxIO->markInputAsDeprecated(input, chunk);
							}
						}

						l_bReadyToProcess = false;

						OV_ERROR_KRF("Invalid incoming input chunks: duration differs between chunks", OpenViBE::Kernel::ErrorType::BadInput);
					}
				}
				else
				{
					l_bReadyToProcess = false;
				}
			}

			//If there is one buffer of the same time period per input, process
			if (l_bReadyToProcess)
			{
				getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
			}

			return true;
		}

		bool CBoxAlgorithmFeatureAggregator::process()
		{
			const IBox* l_pStaticBoxContext = getBoxAlgorithmContext()->getStaticBoxContext();
			IBoxIO* l_pBoxIO                = getBoxAlgorithmContext()->getDynamicBoxContext();

			IMatrix* l_pOutputMatrix = m_pFeatureVectorEncoder->getInputMatrix();
			std::vector<double> l_vBufferElements;
			uint64_t l_ui64TotalBufferSize = 0;
			bool l_bBufferReceived         = false;

			for (uint32_t input = 0; input < l_pStaticBoxContext->getInputCount(); input++)
			{
				m_pStreamedMatrixDecoder[input]->decode(0);
				//*
				if ((m_pStreamedMatrixDecoder[input]->isHeaderReceived()) && !m_bHeaderSent)
				{
					//getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << LogLevel_Warning << "header " << input << "\n";
					IMatrix* l_pInputMatrix = m_pStreamedMatrixDecoder[input]->getOutputMatrix();
					l_ui64TotalBufferSize += l_pInputMatrix->getBufferElementCount();
					if (input == l_pStaticBoxContext->getInputCount() - 1)
					{
						l_pOutputMatrix->setDimensionCount(1);
						l_pOutputMatrix->setDimensionSize(0, (uint32_t)l_ui64TotalBufferSize);

						for (uint32_t i = 0; i < (uint32_t)l_ui64TotalBufferSize; i++)
						{
							char l_sBuffer[64];
							sprintf(l_sBuffer, "Feature %d", (i + 1));
							l_pOutputMatrix->setDimensionLabel(0, i, l_sBuffer);
						}

						m_pFeatureVectorEncoder->encodeHeader();
						l_pBoxIO->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
						m_bHeaderSent = true;
					}
				}
				//*/
				if (m_pStreamedMatrixDecoder[input]->isBufferReceived())
				{
					l_bBufferReceived         = true;
					IMatrix* l_pInputMatrix   = m_pStreamedMatrixDecoder[input]->getOutputMatrix();
					uint32_t l_ui32BufferSize = l_pInputMatrix->getBufferElementCount();

					double* l_pBuffer = l_pInputMatrix->getBuffer();
					for (uint32_t i = 0; i < l_ui32BufferSize; i++)
					{
						l_vBufferElements.push_back(l_pBuffer[i]);
					}
				}
			}

			if (m_bHeaderSent && l_bBufferReceived)
			{
				double* l_pOutputBuffer = l_pOutputMatrix->getBuffer();
				for (uint32_t i = 0; i < l_vBufferElements.size(); i++)
				{
					l_pOutputBuffer[i] = l_vBufferElements[i];
				}
				m_pFeatureVectorEncoder->encodeBuffer();
				l_pBoxIO->markOutputAsReadyToSend(0, m_ui64LastChunkStartTime, m_ui64LastChunkEndTime);
			}

			return true;
		}
	};
};
