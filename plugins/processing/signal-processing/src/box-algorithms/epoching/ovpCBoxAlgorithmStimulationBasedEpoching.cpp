#include <cmath>
#include <algorithm>
#include <limits>

#include <openvibe/ovITimeArithmetics.h>
#include "ovpCBoxAlgorithmStimulationBasedEpoching.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

namespace
{
	const int inputSignalIndex       = 0;
	const int inputStimulationsIndex = 1;
	const int outputSignalIndex      = 0;
}

bool CBoxAlgorithmStimulationBasedEpoching::initialize()
{
	m_EpochDurationInSeconds = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const double epochOffset = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_StimulationId          = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	m_EpochDuration = ITimeArithmetics::secondsToTime(m_EpochDurationInSeconds);

	const int epochOffsetSign = (epochOffset > 0) - (epochOffset < 0);
	m_EpochOffset             = epochOffsetSign * int64_t(ITimeArithmetics::secondsToTime(std::fabs(epochOffset)));

	m_LastReceivedStimulationDate   = 0;
	m_LastStimulationChunkStartTime = 0;
	m_LastSignalChunkEndTime        = 0;

	m_SignalDecoder.initialize(*this, 0);
	m_StimulationDecoder.initialize(*this, 1);

	m_SignalEncoder.initialize(*this, 0);

	m_SignalEncoder.getInputSamplingRate().setReferenceTarget(m_SignalDecoder.getOutputSamplingRate());
	m_ChannelCount = 0;
	m_SamplingRate = 0;

	m_CachedChunks.clear();

	OV_ERROR_UNLESS_KRF(m_EpochDurationInSeconds > 0,
						"Epocher setting is invalid. Duration (= " << m_EpochDurationInSeconds << ") must have a strictly positive value.",
						ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmStimulationBasedEpoching::uninitialize()
{
	m_SignalDecoder.uninitialize();
	m_SignalEncoder.uninitialize();
	m_StimulationDecoder.uninitialize();
	m_CachedChunks.clear();
	return true;
}

bool CBoxAlgorithmStimulationBasedEpoching::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmStimulationBasedEpoching::process()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(inputSignalIndex); ++chunk)
	{
		OV_ERROR_UNLESS_KRF(m_SignalDecoder.decode(chunk),
							"Failed to decode chunk",
							ErrorType::Internal);
		IMatrix* inputMatrix         = m_SignalDecoder.getOutputMatrix();
		uint64_t inputChunkStartTime = dynamicBoxContext.getInputChunkStartTime(inputSignalIndex, chunk);
		uint64_t inputChunkEndTime   = dynamicBoxContext.getInputChunkEndTime(inputSignalIndex, chunk);

		if (m_SignalDecoder.isHeaderReceived())
		{
			IMatrix* outputMatrix = m_SignalEncoder.getInputMatrix();

			m_ChannelCount              = inputMatrix->getDimensionSize(0);
			m_SampleCountPerInputBuffer = inputMatrix->getDimensionSize(1);

			m_SamplingRate = m_SignalDecoder.getOutputSamplingRate();
			OV_ERROR_UNLESS_KRZ(m_SamplingRate,
								LogLevel_Error << "Input sampling frequency is equal to 0. Plugin can not process.",
								ErrorType::Internal);

			m_SampleCountPerOutputEpoch = uint32_t(
				ITimeArithmetics::timeToSampleCount(m_SamplingRate, ITimeArithmetics::secondsToTime(m_EpochDurationInSeconds)));

			outputMatrix->setDimensionCount(2);
			outputMatrix->setDimensionSize(0, m_ChannelCount);
			outputMatrix->setDimensionSize(1, m_SampleCountPerOutputEpoch);

			for (uint32_t channel = 0; channel < m_ChannelCount; ++channel)
			{
				outputMatrix->setDimensionLabel(0, channel, inputMatrix->getDimensionLabel(0, channel));
			}
			m_SignalEncoder.encodeHeader();
			dynamicBoxContext.markOutputAsReadyToSend(outputSignalIndex, 0, 0);
		}

		if (m_SignalDecoder.isBufferReceived())
		{
			OV_ERROR_UNLESS_KRF((inputChunkStartTime >= m_LastSignalChunkEndTime),
								"Stimulation Based Epoching can not work on overlapping signal",
								ErrorType::Internal);
			// Cache the signal data
			m_CachedChunks.emplace_back(inputChunkStartTime, inputChunkEndTime, new CMatrix());
			OpenViBEToolkit::Tools::Matrix::copy(*m_CachedChunks.back().matrix, *inputMatrix);

			m_LastSignalChunkEndTime = inputChunkEndTime;
		}

		if (m_SignalDecoder.isEndReceived())
		{
			m_SignalEncoder.encodeEnd();
			dynamicBoxContext.markOutputAsReadyToSend(outputSignalIndex, inputChunkStartTime, inputChunkEndTime);
		}
	}

	for (uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(inputStimulationsIndex); ++chunk)
	{
		m_StimulationDecoder.decode(chunk);
		// We only handle buffers and ignore stimulation headers and ends
		if (m_StimulationDecoder.isBufferReceived())
		{
			for (uint64_t stimulation = 0; stimulation < m_StimulationDecoder.getOutputStimulationSet()->getStimulationCount(); ++stimulation)
			{
				if (m_StimulationDecoder.getOutputStimulationSet()->getStimulationIdentifier(stimulation) == m_StimulationId)
				{
					// Stimulations are put into cache, we ignore stimulations that would produce output chunks with negative start date (after applying the offset)
					uint64_t stimulationDate = m_StimulationDecoder.getOutputStimulationSet()->getStimulationDate(stimulation);
					if (stimulationDate < m_LastReceivedStimulationDate)
					{
						OV_WARNING_K(
							"Skipping stimulation (received at date " << time64(stimulationDate) << ") that predates an already received stimulation (at date "
							<< time64(m_LastReceivedStimulationDate) << ")");
					}
					else if (int64_t(stimulationDate) + m_EpochOffset >= 0)
					{
						m_ReceivedStimulations.push_back(stimulationDate);
						m_LastReceivedStimulationDate = stimulationDate;
					}
				}
				m_LastStimulationChunkStartTime = dynamicBoxContext.getInputChunkEndTime(inputStimulationsIndex, chunk);
			}
		}
	}

	// Process the received stimulations
	uint64_t lastProcessedStimulationDate = 0;

	for (uint64_t stimulationDate : m_ReceivedStimulations)
	{
		const uint64_t currentEpochStartTime = uint64_t(int64_t(stimulationDate) + m_EpochOffset);

		// No cache available
		if (m_CachedChunks.empty()) { break; }
		// During normal functioning only chunks that will no longer be useful are deprecated, this is to avoid failure in case of a bug
		if (m_CachedChunks.front().startTime > currentEpochStartTime)
		{
			OV_WARNING_K("Skipped creating an epoch on a timespan with no signal. The input signal probably contains non-contiguous chunks.");
			break;
		}

		// We only process stimulations for which we have received enough signal to create an epoch
		if (m_LastSignalChunkEndTime >= currentEpochStartTime + m_EpochDuration)
		{
			auto* outputBuffer                            = m_SignalEncoder.getInputMatrix()->getBuffer();
			unsigned int currentSampleIndexInOutputBuffer = 0;
			unsigned int cachedChunkIndex                 = 0;

			auto chunkStartTime = m_CachedChunks[cachedChunkIndex].startTime;
			auto chunkEndTime   = m_CachedChunks[cachedChunkIndex].endTime;

			// Find the first chunk that contains data interesting for the sent epoch
			while (chunkStartTime > currentEpochStartTime || chunkEndTime < currentEpochStartTime)
			{
				cachedChunkIndex += 1;
				if (cachedChunkIndex == m_CachedChunks.size()) { break; }
				chunkStartTime = m_CachedChunks[cachedChunkIndex].startTime;
				chunkEndTime   = m_CachedChunks[cachedChunkIndex].endTime;
			}

			// If we have found a chunk that contains samples in the current epoch
			if (cachedChunkIndex != m_CachedChunks.size())
			{
				uint64_t currentSampleIndexInInputBuffer = ITimeArithmetics::timeToSampleCount(m_SamplingRate, currentEpochStartTime - chunkStartTime);

				while (currentSampleIndexInOutputBuffer < m_SampleCountPerOutputEpoch)
				{
					const auto currentOutputSampleTime = currentEpochStartTime + ITimeArithmetics::sampleCountToTime(
															 m_SamplingRate, currentSampleIndexInOutputBuffer);

					// If we handle non-dyadic sampling rates then we do not have a guarantee that all chunks will be
					// dated with exact values. We add a bit of wiggle room around the incoming chunks to consider
					// whether a sample is in them or not. This wiggle room will be of half of the sample duration
					// on each side.
					const uint64_t chunkTimeTolerance = ITimeArithmetics::sampleCountToTime(m_SamplingRate, 1) / 2;
					if (currentSampleIndexInInputBuffer == m_SampleCountPerInputBuffer)
					{
						// advance to beginning of the next cached chunk
						cachedChunkIndex += 1;
						if (cachedChunkIndex == m_CachedChunks.size()) { break; }
						chunkStartTime                  = m_CachedChunks[cachedChunkIndex].startTime;
						chunkEndTime                    = m_CachedChunks[cachedChunkIndex].endTime;
						currentSampleIndexInInputBuffer = 0;

						if (chunkStartTime > currentOutputSampleTime + chunkTimeTolerance)
						{
							// Case of non-consecutive chunks
							break;
						}
					}
					else if (chunkStartTime <= currentOutputSampleTime + chunkTimeTolerance && currentOutputSampleTime <= chunkEndTime + chunkTimeTolerance)
					{
						const auto& inputBuffer = m_CachedChunks[cachedChunkIndex].matrix->getBuffer();
						for (uint32_t channel = 0; channel < m_ChannelCount; ++channel)
						{
							outputBuffer[channel * m_SampleCountPerOutputEpoch + currentSampleIndexInOutputBuffer] = inputBuffer[
								channel * m_SampleCountPerInputBuffer + currentSampleIndexInInputBuffer];
						}
						currentSampleIndexInOutputBuffer += 1;
						currentSampleIndexInInputBuffer += 1;
					}
					else { OV_ERROR_KRF("Can not construct the output chunk due to internal error", ErrorType::Internal); }
				}
			}

			// If the epoch is not complete (due to holes in signal)
			if (currentSampleIndexInOutputBuffer == m_SampleCountPerOutputEpoch)
			{
				m_SignalEncoder.encodeBuffer();
				dynamicBoxContext.markOutputAsReadyToSend(outputSignalIndex, currentEpochStartTime, currentEpochStartTime + m_EpochDuration);
			}
			else { OV_WARNING_K("Skipped creating an epoch on a timespan with no signal. The input signal probably contains non-contiguous chunks."); }

			lastProcessedStimulationDate = stimulationDate;
		}
			// We only process stimulations for which we have received enough signal to create an epoch
		else
		{
			// No more complete epochs can be constructed
			break;
		}
	}

	// Remove all stimulations for which the epochs have been constructed and sent
	m_ReceivedStimulations.erase(std::remove_if(m_ReceivedStimulations.begin(), m_ReceivedStimulations.end(),
												[&lastProcessedStimulationDate](const uint64_t& stimulationDate)
												{
													return stimulationDate <= lastProcessedStimulationDate;
												}), m_ReceivedStimulations.end());

	// Deprecate cached chunks which will no longer be used because they are too far back in history compared to received stimulations
	const uint64_t lastUsefulChunkEndTime = m_ReceivedStimulations.empty() ? m_LastStimulationChunkStartTime : m_ReceivedStimulations.front();

	auto cutoffTime = int64_t(lastUsefulChunkEndTime) + m_EpochOffset;
	if (cutoffTime > 0)
	{
		m_CachedChunks.erase(std::remove_if(m_CachedChunks.begin(), m_CachedChunks.end(), [cutoffTime](const CachedChunk& cachedChunk)
		{
			return cachedChunk.endTime < uint64_t(cutoffTime);
		}), m_CachedChunks.end());
	}

	return true;
}
