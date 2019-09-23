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

bool CBoxAlgorithmTimeBasedEpoching::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmTimeBasedEpoching::process()
{
	IDynamicBoxContext& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		OV_ERROR_UNLESS_KRF(m_SignalDecoder.decode(i), "Failed to decode chunk", ErrorType::Internal);

		IMatrix* iMatrix = m_SignalDecoder.getOutputMatrix();
		IMatrix* oMatrix = m_SignalEncoder.getInputMatrix();

		const uint32_t nChannel = iMatrix->getDimensionSize(0);
		const uint32_t nISample = iMatrix->getDimensionSize(1);

		if (m_SignalDecoder.isHeaderReceived())
		{
			m_LastInputEndTime  = 0;
			m_OutputSampleIndex = 0;
			m_OutputChunkIndex  = 0;
			m_ReferenceTime     = 0;

			m_SamplingRate = m_SignalDecoder.getOutputSamplingRate();
			OV_ERROR_UNLESS_KRZ(m_SamplingRate, "Input sampling frequency is equal to 0. Plugin can not process.", ErrorType::Internal);

			m_OutputSampleCount             = uint32_t(m_EpochDuration * m_SamplingRate); // sample count per output epoch
			m_OutputSampleCountBetweenEpoch = uint32_t(m_EpochInterval * m_SamplingRate);

			OV_ERROR_UNLESS_KRF(m_OutputSampleCount>0 && m_OutputSampleCountBetweenEpoch>0,
								"Input sampling frequency is [" << m_SamplingRate << "]. This is too low in order to produce epochs of [" << m_EpochDuration
								<< "] seconds with an interval of [" << m_EpochInterval << "] seconds.",
								ErrorType::Internal);

			oMatrix->setDimensionCount(2);
			oMatrix->setDimensionSize(0, nChannel);
			oMatrix->setDimensionSize(1, m_OutputSampleCount);
			for (uint32_t c = 0; c < nChannel; c++)
			{
				oMatrix->setDimensionLabel(0, c, iMatrix->getDimensionLabel(0, c));
			}

			m_SignalEncoder.encodeHeader();
			boxContext.markOutputAsReadyToSend(0, 0, 0);
		}
		if (m_SignalDecoder.isBufferReceived())
		{
			const uint64_t iTStart = boxContext.getInputChunkStartTime(0, i);
			const uint64_t iTEnd   = boxContext.getInputChunkEndTime(0, i);

			if (m_LastInputEndTime != iTStart)
			{
				// reset
				m_ReferenceTime     = iTStart; // reference time = start time of the first chunk of the continuous stream of chunks
				m_OutputSampleIndex = 0;
				m_OutputChunkIndex  = 0;
			}

			m_LastInputEndTime = iTEnd;

			// **********************************
			//
			// Epoching
			//
			// **********************************

			double* iBuffer = iMatrix->getBuffer();
			double* oBuffer = oMatrix->getBuffer();

			uint32_t sampleProcessed = 0;

			// Iterates on bytes to process
			while (sampleProcessed != nISample)
			{
				if (m_OutputSampleIndex < m_OutputSampleCount) // Some samples should be filled
				{
					// Copies samples to buffer
					const uint32_t sampleToFill = std::min(m_OutputSampleCount - m_OutputSampleIndex, nISample - sampleProcessed);
					for (uint32_t c = 0; c < nChannel; c++)
					{
						System::Memory::copy(oBuffer + c * m_OutputSampleCount + m_OutputSampleIndex,
											 iBuffer + c * nISample + sampleProcessed,
											 sampleToFill * sizeof(double));
					}
					m_OutputSampleIndex += sampleToFill;
					sampleProcessed += sampleToFill;

					if (m_OutputSampleIndex == m_OutputSampleCount) // An epoch has been totally filled !
					{
						// Calculates start and end time of output
						const uint64_t oTStart = m_ReferenceTime + ITimeArithmetics::sampleCountToTime(m_SamplingRate, m_OutputChunkIndex * m_OutputSampleCountBetweenEpoch);
						const uint64_t oTEnd = m_ReferenceTime + ITimeArithmetics::sampleCountToTime(m_SamplingRate, m_OutputChunkIndex * m_OutputSampleCountBetweenEpoch + m_OutputSampleCount);
						m_OutputChunkIndex++;

						// Writes epoch
						m_SignalEncoder.encodeBuffer();
						boxContext.markOutputAsReadyToSend(0, oTStart, oTEnd);

						if (m_OutputSampleCountBetweenEpoch < m_OutputSampleCount)
						{
							// Shifts samples for next epoch when overlap
							const uint32_t samplesToSave = m_OutputSampleCount - m_OutputSampleCountBetweenEpoch;
							for (uint32_t c = 0; c < nChannel; c++)
							{
								System::Memory::move(oBuffer + c * m_OutputSampleCount,
													 oBuffer + c * m_OutputSampleCount + m_OutputSampleCount - samplesToSave,
													 samplesToSave * sizeof(double));
							}

							// The counter can be reset
							m_OutputSampleIndex = samplesToSave;
						}
					}
				}
				else
				{
					// The next few samples are useless: the stream of chunks is not continuous, we can remove the samples before the discontinuity
					const uint32_t sampleToSkip = std::min(m_OutputSampleCountBetweenEpoch - m_OutputSampleIndex, nISample - sampleProcessed);
					m_OutputSampleIndex += sampleToSkip;
					sampleProcessed += sampleToSkip;

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
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
	}

	return true;
}
