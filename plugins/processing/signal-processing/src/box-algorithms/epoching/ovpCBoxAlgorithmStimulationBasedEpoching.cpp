#include <cmath>
#include <algorithm>

#include <openvibe/ovTimeArithmetics.h>
#include "ovpCBoxAlgorithmStimulationBasedEpoching.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

namespace
{
	const int INPUT_SIGNAL_IDX       = 0;
	const int INPUT_STIMULATIONS_IDX = 1;
	const int OUTPUT_SIGNAL_IDX      = 0;
}

bool CBoxAlgorithmStimulationBasedEpoching::initialize()
{
	m_epochDurationInSeconds = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const double epochOffset = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_stimulationID          = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	m_epochDuration = TimeArithmetics::secondsToTime(m_epochDurationInSeconds);

	const int epochOffsetSign = (epochOffset > 0) - (epochOffset < 0);
	m_epochOffset             = epochOffsetSign * int64_t(TimeArithmetics::secondsToTime(std::fabs(epochOffset)));

	m_lastReceivedStimulationDate   = 0;
	m_lastStimulationChunkStartTime = 0;
	m_lastSignalChunkEndTime        = 0;

	m_signalDecoder.initialize(*this, 0);
	m_stimulationDecoder.initialize(*this, 1);

	m_signalEncoder.initialize(*this, 0);

	m_signalEncoder.getInputSamplingRate().setReferenceTarget(m_signalDecoder.getOutputSamplingRate());
	m_nChannel = 0;
	m_samplingRate = 0;

	m_cachedChunks.clear();

	OV_ERROR_UNLESS_KRF(m_epochDurationInSeconds > 0,
						"Epocher setting is invalid. Duration (= " << m_epochDurationInSeconds << ") must have a strictly positive value.",
						ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmStimulationBasedEpoching::uninitialize()
{
	m_signalDecoder.uninitialize();
	m_signalEncoder.uninitialize();
	m_stimulationDecoder.uninitialize();
	m_cachedChunks.clear();
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

	for (uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(INPUT_SIGNAL_IDX); ++chunk)
	{
		OV_ERROR_UNLESS_KRF(m_signalDecoder.decode(chunk),
							"Failed to decode chunk",
							ErrorType::Internal);
		IMatrix* inputMatrix         = m_signalDecoder.getOutputMatrix();
		uint64_t inputChunkStartTime = dynamicBoxContext.getInputChunkStartTime(INPUT_SIGNAL_IDX, chunk);
		uint64_t inputChunkEndTime   = dynamicBoxContext.getInputChunkEndTime(INPUT_SIGNAL_IDX, chunk);

		if (m_signalDecoder.isHeaderReceived())
		{
			IMatrix* outputMatrix = m_signalEncoder.getInputMatrix();

			m_nChannel              = inputMatrix->getDimensionSize(0);
			m_nSamplePerInputBuffer = inputMatrix->getDimensionSize(1);

			m_samplingRate = m_signalDecoder.getOutputSamplingRate();
			OV_ERROR_UNLESS_KRZ(m_samplingRate,
								LogLevel_Error << "Input sampling frequency is equal to 0. Plugin can not process.",
								ErrorType::Internal);

			m_nSampleCountOutputEpoch = uint32_t(TimeArithmetics::timeToSampleCount(m_samplingRate, TimeArithmetics::secondsToTime(m_epochDurationInSeconds)));

			outputMatrix->setDimensionCount(2);
			outputMatrix->setDimensionSize(0, m_nChannel);
			outputMatrix->setDimensionSize(1, m_nSampleCountOutputEpoch);

			for (uint32_t channel = 0; channel < m_nChannel; ++channel)
			{
				outputMatrix->setDimensionLabel(0, channel, inputMatrix->getDimensionLabel(0, channel));
			}
			m_signalEncoder.encodeHeader();
			dynamicBoxContext.markOutputAsReadyToSend(OUTPUT_SIGNAL_IDX, 0, 0);
		}

		if (m_signalDecoder.isBufferReceived())
		{
			OV_ERROR_UNLESS_KRF((inputChunkStartTime >= m_lastSignalChunkEndTime),
								"Stimulation Based Epoching can not work on overlapping signal",
								ErrorType::Internal);
			// Cache the signal data
			m_cachedChunks.emplace_back(inputChunkStartTime, inputChunkEndTime, new CMatrix());
			OpenViBEToolkit::Tools::Matrix::copy(*m_cachedChunks.back().matrix, *inputMatrix);

			m_lastSignalChunkEndTime = inputChunkEndTime;
		}

		if (m_signalDecoder.isEndReceived())
		{
			m_signalEncoder.encodeEnd();
			dynamicBoxContext.markOutputAsReadyToSend(OUTPUT_SIGNAL_IDX, inputChunkStartTime, inputChunkEndTime);
		}
	}

	for (uint32_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(INPUT_STIMULATIONS_IDX); ++chunk)
	{
		m_stimulationDecoder.decode(chunk);
		// We only handle buffers and ignore stimulation headers and ends
		if (m_stimulationDecoder.isBufferReceived())
		{
			for (uint64_t stimulation = 0; stimulation < m_stimulationDecoder.getOutputStimulationSet()->getStimulationCount(); ++stimulation)
			{
				if (m_stimulationDecoder.getOutputStimulationSet()->getStimulationIdentifier(stimulation) == m_stimulationID)
				{
					// Stimulations are put into cache, we ignore stimulations that would produce output chunks with negative start date (after applying the offset)
					uint64_t stimulationDate = m_stimulationDecoder.getOutputStimulationSet()->getStimulationDate(stimulation);
					if (stimulationDate < m_lastReceivedStimulationDate)
					{
						OV_WARNING_K(
							"Skipping stimulation (received at date " << time64(stimulationDate) << ") that predates an already received stimulation (at date "
							<< time64(m_lastReceivedStimulationDate) << ")");
					}
					else if (int64_t(stimulationDate) + m_epochOffset >= 0)
					{
						m_receivedStimulations.push_back(stimulationDate);
						m_lastReceivedStimulationDate = stimulationDate;
					}
				}
				m_lastStimulationChunkStartTime = dynamicBoxContext.getInputChunkEndTime(INPUT_STIMULATIONS_IDX, chunk);
			}
		}
	}

	// Process the received stimulations
	uint64_t lastProcessedStimulationDate = 0;

	for (uint64_t stimulationDate : m_receivedStimulations)
	{
		const uint64_t currentEpochStartTime = uint64_t(int64_t(stimulationDate) + m_epochOffset);

		// No cache available
		if (m_cachedChunks.empty()) { break; }
		// During normal functioning only chunks that will no longer be useful are deprecated, this is to avoid failure in case of a bug
		if (m_cachedChunks.front().startTime > currentEpochStartTime)
		{
			OV_WARNING_K("Skipped creating an epoch on a timespan with no signal. The input signal probably contains non-contiguous chunks.");
			break;
		}

		// We only process stimulations for which we have received enough signal to create an epoch
		if (m_lastSignalChunkEndTime >= currentEpochStartTime + m_epochDuration)
		{
			auto* outputBuffer                            = m_signalEncoder.getInputMatrix()->getBuffer();
			uint32_t currentSampleIndexInOutputBuffer = 0;
			uint32_t cachedChunkIndex                 = 0;

			auto chunkStartTime = m_cachedChunks[cachedChunkIndex].startTime;
			auto chunkEndTime   = m_cachedChunks[cachedChunkIndex].endTime;

			// Find the first chunk that contains data interesting for the sent epoch
			while (chunkStartTime > currentEpochStartTime || chunkEndTime < currentEpochStartTime)
			{
				cachedChunkIndex += 1;
				if (cachedChunkIndex == m_cachedChunks.size()) { break; }
				chunkStartTime = m_cachedChunks[cachedChunkIndex].startTime;
				chunkEndTime   = m_cachedChunks[cachedChunkIndex].endTime;
			}

			// If we have found a chunk that contains samples in the current epoch
			if (cachedChunkIndex != m_cachedChunks.size())
			{
				uint64_t currentSampleIndexInInputBuffer = TimeArithmetics::timeToSampleCount(m_samplingRate, currentEpochStartTime - chunkStartTime);

				while (currentSampleIndexInOutputBuffer < m_nSampleCountOutputEpoch)
				{
					const auto currentOutputSampleTime = currentEpochStartTime + TimeArithmetics::sampleCountToTime(
															 m_samplingRate, currentSampleIndexInOutputBuffer);

					// If we handle non-dyadic sampling rates then we do not have a guarantee that all chunks will be
					// dated with exact values. We add a bit of wiggle room around the incoming chunks to consider
					// whether a sample is in them or not. This wiggle room will be of half of the sample duration
					// on each side.
					const uint64_t chunkTimeTolerance = TimeArithmetics::sampleCountToTime(m_samplingRate, 1) / 2;
					if (currentSampleIndexInInputBuffer == m_nSamplePerInputBuffer)
					{
						// advance to beginning of the next cached chunk
						cachedChunkIndex += 1;
						if (cachedChunkIndex == m_cachedChunks.size()) { break; }
						chunkStartTime                  = m_cachedChunks[cachedChunkIndex].startTime;
						chunkEndTime                    = m_cachedChunks[cachedChunkIndex].endTime;
						currentSampleIndexInInputBuffer = 0;

						if (chunkStartTime > currentOutputSampleTime + chunkTimeTolerance)
						{
							// Case of non-consecutive chunks
							break;
						}
					}
					else if (chunkStartTime <= currentOutputSampleTime + chunkTimeTolerance && currentOutputSampleTime <= chunkEndTime + chunkTimeTolerance)
					{
						const auto& inputBuffer = m_cachedChunks[cachedChunkIndex].matrix->getBuffer();
						for (uint32_t channel = 0; channel < m_nChannel; ++channel)
						{
							outputBuffer[channel * m_nSampleCountOutputEpoch + currentSampleIndexInOutputBuffer] = inputBuffer[
								channel * m_nSamplePerInputBuffer + currentSampleIndexInInputBuffer];
						}
						currentSampleIndexInOutputBuffer += 1;
						currentSampleIndexInInputBuffer += 1;
					}
					else { OV_ERROR_KRF("Can not construct the output chunk due to internal error", ErrorType::Internal); }
				}
			}

			// If the epoch is not complete (due to holes in signal)
			if (currentSampleIndexInOutputBuffer == m_nSampleCountOutputEpoch)
			{
				m_signalEncoder.encodeBuffer();
				dynamicBoxContext.markOutputAsReadyToSend(OUTPUT_SIGNAL_IDX, currentEpochStartTime, currentEpochStartTime + m_epochDuration);
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
	m_receivedStimulations.erase(std::remove_if(m_receivedStimulations.begin(), m_receivedStimulations.end(),
												[&lastProcessedStimulationDate](const uint64_t& stimulationDate)
												{
													return stimulationDate <= lastProcessedStimulationDate;
												}), m_receivedStimulations.end());

	// Deprecate cached chunks which will no longer be used because they are too far back in history compared to received stimulations
	const uint64_t lastUsefulChunkEndTime = m_receivedStimulations.empty() ? m_lastStimulationChunkStartTime : m_receivedStimulations.front();

	auto cutoffTime = int64_t(lastUsefulChunkEndTime) + m_epochOffset;
	if (cutoffTime > 0)
	{
		m_cachedChunks.erase(std::remove_if(m_cachedChunks.begin(), m_cachedChunks.end(), [cutoffTime](const CachedChunk& cachedChunk)
		{
			return cachedChunk.endTime < uint64_t(cutoffTime);
		}), m_cachedChunks.end());
	}

	return true;
}
