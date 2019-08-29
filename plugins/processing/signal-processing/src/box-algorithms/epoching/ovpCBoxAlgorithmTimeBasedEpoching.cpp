#include "ovpCBoxAlgorithmTimeBasedEpoching.h"

#include <system/ovCMemory.h>

#include <openvibe/ovITimeArithmetics.h>
#include <cstdio>
#include <iostream>
#include <algorithm>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace OpenViBEToolkit;

bool CBoxAlgorithmTimeBasedEpoching::initialize()
{
	m_EpochDuration = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_EpochInterval = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(m_EpochDuration>0 && m_EpochInterval>0,
						"Epocher settings are invalid (duration:" << m_EpochDuration
						<< "|" << "interval:" << m_EpochInterval << "). These parameters should be strictly positive.",
						ErrorType::Internal);

	m_SignalDecoder.initialize(*this, 0);
	m_SignalEncoder.initialize(*this, 0);
	m_SignalEncoder.getInputSamplingRate().setReferenceTarget(m_SignalDecoder.getOutputSamplingRate());

	return true;
}

bool CBoxAlgorithmTimeBasedEpoching::uninitialize()
{
	m_SignalDecoder.uninitialize();
	m_SignalEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmTimeBasedEpoching::processInput(const uint32_t index)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmTimeBasedEpoching::process()
{
	IDynamicBoxContext& boxContext = this->getDynamicBoxContext();

	for (uint32_t l_ui32ChunkIndex = 0; l_ui32ChunkIndex < boxContext.getInputChunkCount(0); l_ui32ChunkIndex++)
	{
		OV_ERROR_UNLESS_KRF(m_SignalDecoder.decode(l_ui32ChunkIndex),
							"Failed to decode chunk",
							ErrorType::Internal);

		IMatrix* l_pInputMatrix  = m_SignalDecoder.getOutputMatrix();
		IMatrix* l_pOutputMatrix = m_SignalEncoder.getInputMatrix();

		uint32_t l_ui32ChannelCount     = l_pInputMatrix->getDimensionSize(0);
		uint32_t l_ui32InputSampleCount = l_pInputMatrix->getDimensionSize(1);

		if (m_SignalDecoder.isHeaderReceived())
		{
			m_LastInputEndTime  = 0;
			m_OutputSampleIndex = 0;
			m_OutputChunkIndex  = 0;
			m_ReferenceTime     = 0;

			m_SamplingRate = m_SignalDecoder.getOutputSamplingRate();
			OV_ERROR_UNLESS_KRZ(m_SamplingRate,
								"Input sampling frequency is equal to 0. Plugin can not process.",
								ErrorType::Internal);

			m_OutputSampleCount             = uint32_t(m_EpochDuration * m_SamplingRate); // sample count per output epoch
			m_OutputSampleCountBetweenEpoch = uint32_t(m_EpochInterval * m_SamplingRate);

			OV_ERROR_UNLESS_KRF(m_OutputSampleCount>0 && m_OutputSampleCountBetweenEpoch>0,
								"Input sampling frequency is [" << m_SamplingRate << "]. This is too low in order to produce epochs of [" << m_EpochDuration
								<< "] seconds with an interval of [" << m_EpochInterval << "] seconds.",
								ErrorType::Internal);

			l_pOutputMatrix->setDimensionCount(2);
			l_pOutputMatrix->setDimensionSize(0, l_ui32ChannelCount);
			l_pOutputMatrix->setDimensionSize(1, m_OutputSampleCount);
			for (uint32_t l_ui32ChannelIndex = 0; l_ui32ChannelIndex < l_ui32ChannelCount; l_ui32ChannelIndex++)
			{
				l_pOutputMatrix->setDimensionLabel(0, l_ui32ChannelIndex, l_pInputMatrix->getDimensionLabel(0, l_ui32ChannelIndex));
			}

			m_SignalEncoder.encodeHeader();
			boxContext.markOutputAsReadyToSend(0, 0, 0);
		}
		if (m_SignalDecoder.isBufferReceived())
		{
			uint64_t l_ui64InputChunkStartTime = boxContext.getInputChunkStartTime(0, l_ui32ChunkIndex);
			uint64_t l_ui64InputChunkEndTime   = boxContext.getInputChunkEndTime(0, l_ui32ChunkIndex);

			if (m_LastInputEndTime != l_ui64InputChunkStartTime)
			{
				// reset
				m_ReferenceTime     = l_ui64InputChunkStartTime; // reference time = start time of the first chunk of the continuous stream of chunks
				m_OutputSampleIndex = 0;
				m_OutputChunkIndex  = 0;
			}

			m_LastInputEndTime = l_ui64InputChunkEndTime;

			// **********************************
			//
			// Epoching
			//
			// **********************************

			double* l_pInputBuffer  = l_pInputMatrix->getBuffer();
			double* l_pOutputBuffer = l_pOutputMatrix->getBuffer();

			uint32_t l_ui32SampleProcessed = 0;

			// Iterates on bytes to process
			while (l_ui32SampleProcessed != l_ui32InputSampleCount)
			{
				if (m_OutputSampleIndex < m_OutputSampleCount) // Some samples should be filled
				{
					// Copies samples to buffer
					uint32_t l_ui32SampleToFill = std::min(m_OutputSampleCount - m_OutputSampleIndex, l_ui32InputSampleCount - l_ui32SampleProcessed);
					for (uint32_t l_ui32ChannelIndex = 0; l_ui32ChannelIndex < l_ui32ChannelCount; l_ui32ChannelIndex++)
					{
						System::Memory::copy(l_pOutputBuffer + l_ui32ChannelIndex * m_OutputSampleCount + m_OutputSampleIndex,
											 l_pInputBuffer + l_ui32ChannelIndex * l_ui32InputSampleCount + l_ui32SampleProcessed, l_ui32SampleToFill * sizeof(double));
					}
					m_OutputSampleIndex += l_ui32SampleToFill;
					l_ui32SampleProcessed += l_ui32SampleToFill;

					if (m_OutputSampleIndex == m_OutputSampleCount) // An epoch has been totally filled !
					{
						// Calculates start and end time of output
						uint64_t l_ui64OutputChunkStartTime = m_ReferenceTime + ITimeArithmetics::sampleCountToTime(m_SamplingRate, uint64_t(m_OutputChunkIndex * m_OutputSampleCountBetweenEpoch));
						uint64_t l_ui64OutputChunkEndTime   = m_ReferenceTime + ITimeArithmetics::sampleCountToTime(m_SamplingRate, uint64_t(m_OutputChunkIndex * m_OutputSampleCountBetweenEpoch + m_OutputSampleCount));
						m_OutputChunkIndex++;

						// Writes epoch
						m_SignalEncoder.encodeBuffer();
						boxContext.markOutputAsReadyToSend(0, l_ui64OutputChunkStartTime, l_ui64OutputChunkEndTime);

						if (m_OutputSampleCountBetweenEpoch < m_OutputSampleCount)
						{
							// Shifts samples for next epoch when overlap
							uint32_t l_ui32SamplesToSave = m_OutputSampleCount - m_OutputSampleCountBetweenEpoch;
							for (uint32_t l_ui32ChannelIndex = 0; l_ui32ChannelIndex < l_ui32ChannelCount; l_ui32ChannelIndex++)
							{
								System::Memory::move(l_pOutputBuffer + l_ui32ChannelIndex * m_OutputSampleCount,
													 l_pOutputBuffer + l_ui32ChannelIndex * m_OutputSampleCount + m_OutputSampleCount - l_ui32SamplesToSave, l_ui32SamplesToSave * sizeof(double));
							}

							// The counter can be reset
							m_OutputSampleIndex = l_ui32SamplesToSave;
						}
					}
				}
				else
				{
					// The next few samples are useless: the stream of chunks is not continuous, we can remove the samples before the discontinuity
					uint32_t l_ui32SampleToSkip = std::min(m_OutputSampleCountBetweenEpoch - m_OutputSampleIndex, l_ui32InputSampleCount - l_ui32SampleProcessed);
					m_OutputSampleIndex += l_ui32SampleToSkip;
					l_ui32SampleProcessed += l_ui32SampleToSkip;

					if (m_OutputSampleIndex == m_OutputSampleCountBetweenEpoch)
					{
						// The counter can be reset
						m_OutputSampleIndex = 0;
					}
				}
			}
		}
		if (m_SignalDecoder.isEndReceived())
		{
			m_SignalEncoder.encodeEnd();
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, l_ui32ChunkIndex), boxContext.getInputChunkEndTime(0, l_ui32ChunkIndex));
		}
	}

	return true;
}
