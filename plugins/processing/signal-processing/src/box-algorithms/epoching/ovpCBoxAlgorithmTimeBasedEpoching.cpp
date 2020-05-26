#include "ovpCBoxAlgorithmTimeBasedEpoching.h"
#include <iostream>
#include <algorithm>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::*/Toolkit;
using namespace /*OpenViBE::Plugins::*/SignalProcessing;

bool CBoxAlgorithmTimeBasedEpoching::initialize()
{
	m_duration = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_interval = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(m_duration>0 && m_interval>0,
						"Epocher settings are invalid (duration:" << m_duration << "|interval:"
						<< m_interval << "). These parameters should be strictly positive.", Kernel::ErrorType::Internal);

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

bool CBoxAlgorithmTimeBasedEpoching::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmTimeBasedEpoching::process()
{
	IDynamicBoxContext& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		OV_ERROR_UNLESS_KRF(m_decoder.decode(i), "Failed to decode chunk", Kernel::ErrorType::Internal);

		IMatrix* iMatrix = m_decoder.getOutputMatrix();
		IMatrix* oMatrix = m_encoder.getInputMatrix();

		const size_t nChannel = iMatrix->getDimensionSize(0);
		const size_t nISample = iMatrix->getDimensionSize(1);

		if (m_decoder.isHeaderReceived())
		{
			m_lastInputEndTime = 0;
			m_oSampleIdx       = 0;
			m_oChunkIdx        = 0;
			m_referenceTime    = 0;

			m_sampling = m_decoder.getOutputSamplingRate();
			OV_ERROR_UNLESS_KRZ(m_sampling, "Input sampling frequency is equal to 0. Plugin can not process.", Kernel::ErrorType::Internal);

			m_oNSample             = size_t(m_duration * m_sampling); // sample count per output epoch
			m_oNSampleBetweenEpoch = size_t(m_interval * m_sampling);

			OV_ERROR_UNLESS_KRF(m_oNSample>0 && m_oNSampleBetweenEpoch>0,
								"Input sampling frequency is [" << m_sampling << "]. This is too low in order to produce epochs of ["
								<< m_duration << "] seconds with an interval of [" << m_interval << "] seconds.", Kernel::ErrorType::Internal);

			oMatrix->setDimensionCount(2);
			oMatrix->setDimensionSize(0, nChannel);
			oMatrix->setDimensionSize(1, m_oNSample);
			for (size_t c = 0; c < nChannel; ++c) { oMatrix->setDimensionLabel(0, c, iMatrix->getDimensionLabel(0, c)); }

			m_encoder.encodeHeader();
			boxContext.markOutputAsReadyToSend(0, 0, 0);
		}
		if (m_decoder.isBufferReceived())
		{
			const CTime iTStart = boxContext.getInputChunkStartTime(0, i);
			const CTime iTEnd   = boxContext.getInputChunkEndTime(0, i);

			if (m_lastInputEndTime != iTStart)
			{
				// reset
				m_referenceTime = iTStart; // reference time = start time of the first chunk of the continuous stream of chunks
				m_oSampleIdx    = 0;
				m_oChunkIdx     = 0;
			}

			m_lastInputEndTime = iTEnd;

			// **********************************
			//
			// Epoching
			//
			// **********************************

			double* iBuffer = iMatrix->getBuffer();
			double* oBuffer = oMatrix->getBuffer();

			size_t sampleProcessed = 0;

			// Iterates on bytes to process
			while (sampleProcessed != nISample)
			{
				if (m_oSampleIdx < m_oNSample) // Some samples should be filled
				{
					// Copies samples to buffer
					const size_t sampleToFill = std::min(m_oNSample - m_oSampleIdx, nISample - sampleProcessed);
					for (size_t c = 0; c < nChannel; ++c)
					{
						memcpy(oBuffer + c * m_oNSample + m_oSampleIdx, iBuffer + c * nISample + sampleProcessed, sampleToFill * sizeof(double));
					}
					m_oSampleIdx += sampleToFill;
					sampleProcessed += sampleToFill;

					if (m_oSampleIdx == m_oNSample) // An epoch has been totally filled !
					{
						// Calculates start and end time of output
						const CTime oTStart = m_referenceTime + CTime(m_sampling, m_oChunkIdx * m_oNSampleBetweenEpoch);
						const CTime oTEnd   = m_referenceTime + CTime(m_sampling, m_oChunkIdx * m_oNSampleBetweenEpoch + m_oNSample);
						m_oChunkIdx++;

						// Writes epoch
						m_encoder.encodeBuffer();
						boxContext.markOutputAsReadyToSend(0, oTStart, oTEnd);

						if (m_oNSampleBetweenEpoch < m_oNSample)
						{
							// Shifts samples for next epoch when overlap
							const size_t samplesToSave = m_oNSample - m_oNSampleBetweenEpoch;
							for (size_t c = 0; c < nChannel; ++c)
							{
								memmove(oBuffer + c * m_oNSample, oBuffer + c * m_oNSample + m_oNSample - samplesToSave, samplesToSave * sizeof(double));
							}

							// The counter can be reset
							m_oSampleIdx = samplesToSave;
						}
					}
				}
				else
				{
					// The next few samples are useless: the stream of chunks is not continuous, we can remove the samples before the discontinuity
					const size_t sampleToSkip = std::min(m_oNSampleBetweenEpoch - m_oSampleIdx, nISample - sampleProcessed);
					m_oSampleIdx += sampleToSkip;
					sampleProcessed += sampleToSkip;

					if (m_oSampleIdx == m_oNSampleBetweenEpoch)
					{
						// The counter can be reset
						m_oSampleIdx = 0;
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
