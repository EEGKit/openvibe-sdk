#include "ovpCBoxAlgorithmTimeBasedEpoching.h"

#include <system/ovCMemory.h>

#include <openvibe/ovTimeArithmetics.h>
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
	m_duration = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_interval = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(m_duration>0 && m_interval>0,
						"Epocher settings are invalid (duration:" << m_duration << "|" << "interval:" << m_interval << "). These parameters should be strictly positive.",
						ErrorType::Internal);

	m_decoder.initialize(*this, 0);
	m_encoder.initialize(*this, 0);
	m_encoder.getInputSamplingRate().setReferenceTarget(m_decoder.getOutputSamplingRate());

	return true;
}

bool CBoxAlgorithmTimeBasedEpoching::uninitialize()
{
	m_decoder.uninitialize();
	m_encoder.uninitialize();

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

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		OV_ERROR_UNLESS_KRF(m_decoder.decode(i), "Failed to decode chunk", ErrorType::Internal);

		IMatrix* iMatrix = m_decoder.getOutputMatrix();
		IMatrix* oMatrix = m_encoder.getInputMatrix();

		const uint32_t nChannel = iMatrix->getDimensionSize(0);
		const uint32_t nISample = iMatrix->getDimensionSize(1);

		if (m_decoder.isHeaderReceived())
		{
			m_lastInputEndTime  = 0;
			m_outputSampleIdx = 0;
			m_outputChunkIdx  = 0;
			m_referenceTime     = 0;

			m_samplingRate = m_decoder.getOutputSamplingRate();
			OV_ERROR_UNLESS_KRZ(m_samplingRate, "Input sampling frequency is equal to 0. Plugin can not process.", ErrorType::Internal);

			m_nOutputSample             = uint32_t(m_duration * m_samplingRate); // sample count per output epoch
			m_nOutputSampleBetweenEpoch = uint32_t(m_interval * m_samplingRate);

			OV_ERROR_UNLESS_KRF(m_nOutputSample>0 && m_nOutputSampleBetweenEpoch>0,
								"Input sampling frequency is [" << m_samplingRate << "]. This is too low in order to produce epochs of [" << m_duration
								<< "] seconds with an interval of [" << m_interval << "] seconds.",
								ErrorType::Internal);

			oMatrix->setDimensionCount(2);
			oMatrix->setDimensionSize(0, nChannel);
			oMatrix->setDimensionSize(1, m_nOutputSample);
			for (uint32_t c = 0; c < nChannel; ++c)
			{
				oMatrix->setDimensionLabel(0, c, iMatrix->getDimensionLabel(0, c));
			}

			m_encoder.encodeHeader();
			boxContext.markOutputAsReadyToSend(0, 0, 0);
		}
		if (m_decoder.isBufferReceived())
		{
			const uint64_t iTStart = boxContext.getInputChunkStartTime(0, i);
			const uint64_t iTEnd   = boxContext.getInputChunkEndTime(0, i);

			if (m_lastInputEndTime != iTStart)
			{
				// reset
				m_referenceTime     = iTStart; // reference time = start time of the first chunk of the continuous stream of chunks
				m_outputSampleIdx = 0;
				m_outputChunkIdx  = 0;
			}

			m_lastInputEndTime = iTEnd;

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
				if (m_outputSampleIdx < m_nOutputSample) // Some samples should be filled
				{
					// Copies samples to buffer
					const uint32_t sampleToFill = std::min(m_nOutputSample - m_outputSampleIdx, nISample - sampleProcessed);
					for (uint32_t c = 0; c < nChannel; ++c)
					{
						System::Memory::copy(oBuffer + c * m_nOutputSample + m_outputSampleIdx,
											 iBuffer + c * nISample + sampleProcessed,
											 sampleToFill * sizeof(double));
					}
					m_outputSampleIdx += sampleToFill;
					sampleProcessed += sampleToFill;

					if (m_outputSampleIdx == m_nOutputSample) // An epoch has been totally filled !
					{
						// Calculates start and end time of output
						const uint64_t oTStart = m_referenceTime + TimeArithmetics::sampleCountToTime(m_samplingRate, m_outputChunkIdx * m_nOutputSampleBetweenEpoch);
						const uint64_t oTEnd = m_referenceTime + TimeArithmetics::sampleCountToTime(m_samplingRate, m_outputChunkIdx * m_nOutputSampleBetweenEpoch + m_nOutputSample);
						m_outputChunkIdx++;

						// Writes epoch
						m_encoder.encodeBuffer();
						boxContext.markOutputAsReadyToSend(0, oTStart, oTEnd);

						if (m_nOutputSampleBetweenEpoch < m_nOutputSample)
						{
							// Shifts samples for next epoch when overlap
							const uint32_t samplesToSave = m_nOutputSample - m_nOutputSampleBetweenEpoch;
							for (uint32_t c = 0; c < nChannel; ++c)
							{
								System::Memory::move(oBuffer + c * m_nOutputSample,
													 oBuffer + c * m_nOutputSample + m_nOutputSample - samplesToSave,
													 samplesToSave * sizeof(double));
							}

							// The counter can be reset
							m_outputSampleIdx = samplesToSave;
						}
					}
				}
				else
				{
					// The next few samples are useless: the stream of chunks is not continuous, we can remove the samples before the discontinuity
					const uint32_t sampleToSkip = std::min(m_nOutputSampleBetweenEpoch - m_outputSampleIdx, nISample - sampleProcessed);
					m_outputSampleIdx += sampleToSkip;
					sampleProcessed += sampleToSkip;

					if (m_outputSampleIdx == m_nOutputSampleBetweenEpoch)
					{
						// The counter can be reset
						m_outputSampleIdx = 0;
					}
				}
			}
		}
		if (m_decoder.isEndReceived())
		{
			m_encoder.encodeEnd();
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
	}

	return true;
}
