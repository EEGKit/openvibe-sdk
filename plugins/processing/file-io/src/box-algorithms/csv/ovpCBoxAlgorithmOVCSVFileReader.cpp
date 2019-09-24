#include "ovpCBoxAlgorithmOVCSVFileReader.h"
#include <iostream>
#include <sstream>
#include <map>
#include <cmath>  // std::ceil() on Linux
#include <algorithm>

#include <openvibe/ovTimeArithmetics.h>

using namespace OpenViBE;
using namespace CSV;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace FileIO;

CBoxAlgorithmOVCSVFileReader::CBoxAlgorithmOVCSVFileReader()
	: m_ReaderLib(createCSVHandler(), releaseCSVHandler), m_IsHeaderSent(false)
	  , m_IsStimulationHeaderSent(false) {}

uint64_t CBoxAlgorithmOVCSVFileReader::getClockFrequency()
{
	return 128LL << 32; // the box clock frequency
}

bool CBoxAlgorithmOVCSVFileReader::initialize()
{
	m_SamplingRate            = 0;
	m_IsHeaderSent            = false;
	m_IsStimulationHeaderSent = false;

	this->getStaticBoxContext().getOutputType(0, m_TypeIdentifier);

	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	OV_ERROR_UNLESS_KRF(m_ReaderLib->openFile(filename.toASCIIString(), EFileAccessMode::Read),
						(ICSVHandler::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " +
							m_ReaderLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	m_SampleCountPerBuffer = 1;

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::Signal);
	}
	else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::StreamedMatrix);
	}
	else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::FeatureVector);
	}
	else if (m_TypeIdentifier == OV_TypeId_Spectrum)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::Spectrum);
	}
	else { OV_ERROR_KRF("Output is a type derived from matrix that the box doesn't recognize support", ErrorType::BadInput); }

	OV_ERROR_UNLESS_KRF(m_StimulationEncoder.initialize(*this, 1), "Error during stimulation encoder initialize", ErrorType::Internal);

	const char *msg = (ICSVHandler::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str();
	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getSignalInformation(m_ChannelNames, m_SamplingRate, m_SampleCountPerBuffer), msg, ErrorType::Internal);
	}
	else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getStreamedMatrixInformation(m_DimensionSizes, m_ChannelNames), msg, ErrorType::Internal);
	}
	else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getFeatureVectorInformation(m_ChannelNames), msg, ErrorType::Internal);
	}
	else if (m_TypeIdentifier == OV_TypeId_Spectrum)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getSpectrumInformation(m_ChannelNames, m_FrequencyAbscissa, m_SamplingRate), msg, ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::uninitialize()
{
	m_ChannelNames.clear();
	m_DimensionSizes.clear();
	m_FrequencyAbscissa.clear();

	m_AlgorithmEncoder.uninitialize();

	OV_ERROR_UNLESS_KRF(m_StimulationEncoder.uninitialize(), "Failed to uninitialize stimulation encoder", ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processClock(IMessageClock& /*messageClock*/)
{
	OV_ERROR_UNLESS_KRF(getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(), "Failed to mark clock algorithm as ready to process", ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileReader::process()
{
	IMatrix* matrix = m_AlgorithmEncoder.getInputMatrix();

	// encode Header if not already encoded
	if (!m_IsHeaderSent)
	{
		if (m_TypeIdentifier == OV_TypeId_Signal)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(2), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, uint32_t(m_ChannelNames.size())), "Failed to set first dimension size", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(1, m_SampleCountPerBuffer), "Failed to set second dimension size", ErrorType::Internal);

			uint32_t index = 0;

			for (const std::string& channelName : m_ChannelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", ErrorType::Internal);
			}

			m_AlgorithmEncoder.getInputSamplingRate() = m_SamplingRate;
		}
		else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(uint32_t(m_DimensionSizes.size())), "Failed to set dimension count", ErrorType::Internal);
			uint32_t previousDimensionSize = 0;

			for (size_t index = 0; index < m_DimensionSizes.size(); index++)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionSize(uint32_t(index), m_DimensionSizes[index]), "Failed to set dimension size " << uint32_t(index + 1), ErrorType::Internal);

				for (uint32_t labelIndex = 0; labelIndex < m_DimensionSizes[index]; labelIndex++)
				{
					OV_FATAL_UNLESS_K(matrix->setDimensionLabel(uint32_t(index), labelIndex, m_ChannelNames[previousDimensionSize + labelIndex].c_str()),
									  "Failed to set dimension label", ErrorType::Internal);
				}

				previousDimensionSize += m_DimensionSizes[index];
			}
		}
		else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(1), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, uint32_t(m_ChannelNames.size())), "Failed to set first dimension size", ErrorType::Internal);

			uint32_t index = 0;
			for (const std::string& channelName : m_ChannelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", ErrorType::Internal);
			}
		}
		else if (m_TypeIdentifier == OV_TypeId_Spectrum)
		{
			IMatrix* frequencyAbscissaMatrix = m_AlgorithmEncoder.getInputFrequencyAbcissa();

			OV_FATAL_UNLESS_K(matrix->setDimensionCount(2), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, uint32_t(m_ChannelNames.size())), "Failed to set first dimension size", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(1, uint32_t(m_FrequencyAbscissa.size())), "Failed to set first dimension size", ErrorType::Internal);
			OV_FATAL_UNLESS_K(frequencyAbscissaMatrix->setDimensionCount(1), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(frequencyAbscissaMatrix->setDimensionSize(0, uint32_t(m_FrequencyAbscissa.size())), "Failed to set first dimension size", ErrorType::Internal);

			uint32_t index = 0;
			for (const std::string& channelName : m_ChannelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", ErrorType::Internal);
			}

			index = 0;
			for (const double& frequencyAbscissaValue : m_FrequencyAbscissa)
			{
				frequencyAbscissaMatrix->getBuffer()[index] = frequencyAbscissaValue;
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(1, index++, std::to_string(frequencyAbscissaValue).c_str()),
								  "Failed to set dimension label", ErrorType::Internal);
			}

			m_AlgorithmEncoder.getInputSamplingRate() = m_SamplingRate;
		}

		OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder.encodeHeader(), "Failed to encode signal header", ErrorType::Internal);

		m_IsHeaderSent = true;
		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0), "Failed to mark signal header as ready to send", ErrorType::Internal);
	}

	const double currentTime = TimeArithmetics::timeToSeconds(this->getPlayerContext().getCurrentTime());

	if (!m_ReaderLib->hasDataToRead() && m_SavedChunks.empty()) { return true; }

	// Fill the chunk buffer if there is no enough data.
	if ((m_SavedChunks.empty() || m_SavedChunks.back().startTime < currentTime) && m_ReaderLib->hasDataToRead())
	{
		do
		{
			std::vector<SMatrixChunk> matrixChunk;
			std::vector<SStimulationChunk> stimulationChunk;

			OV_ERROR_UNLESS_KRF(m_ReaderLib->readSamplesAndEventsFromFile(1, matrixChunk, stimulationChunk),
								(ICSVHandler::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str(),
								ErrorType::Internal);

			m_SavedChunks.insert(m_SavedChunks.end(), matrixChunk.begin(), matrixChunk.end());
			m_SavedStimulations.insert(m_SavedStimulations.end(), stimulationChunk.begin(), stimulationChunk.end());
		} while (!m_SavedChunks.empty() && m_SavedChunks.back().startTime < currentTime && m_ReaderLib->hasDataToRead());
	}

	if (!m_SavedChunks.empty())
	{
		double chunkStartTime = m_SavedChunks.cbegin()->startTime;
		double chunkEndTime   = m_SavedChunks.back().endTime;

		// send stimulations chunk even if there is no stimulations, chunks have to be continued
		OV_ERROR_UNLESS_KRF(this->processStimulation(chunkStartTime, chunkEndTime), "Error during stimulation process", ErrorType::Internal);

		uint32_t chunksToRemove = 0;

		for (const SMatrixChunk& chunk : m_SavedChunks)
		{
			if (currentTime > chunk.startTime)
			{
				// move read matrix into buffer to encode
				std::move(chunk.matrix.begin(), chunk.matrix.end(), matrix->getBuffer());

				OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder.encodeBuffer(), "Failed to encode signal buffer", ErrorType::Internal);

				OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0,
										TimeArithmetics::secondsToTime(chunk.startTime),
										TimeArithmetics::secondsToTime(chunk.endTime)),
									"Failed to mark signal output as ready to send",
									ErrorType::Internal);

				chunksToRemove++;
			}
			else { break; }
		}

		// If there is no more data to send, we push the end.
		if (m_SavedChunks.size() == chunksToRemove && !m_ReaderLib->hasDataToRead())
		{
			OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder.encodeEnd(), "Failed to encode end.", ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0,
									TimeArithmetics::secondsToTime(m_SavedChunks.back().startTime),
									TimeArithmetics::secondsToTime(m_SavedChunks.back().endTime)),
								"Failed to mark signal output as ready to send",
								ErrorType::Internal);
		}

		if (chunksToRemove != 0) { m_SavedChunks.erase(m_SavedChunks.begin(), m_SavedChunks.begin() + chunksToRemove); }
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processStimulation(double startTime, double endTime)
{
	if (!m_IsStimulationHeaderSent)
	{
		OV_ERROR_UNLESS_KRF(m_StimulationEncoder.encodeHeader(), "Failed to encode stimulation header", ErrorType::Internal);
		m_IsStimulationHeaderSent = true;

		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, 0, 0),
							"Failed to mark stimulation header as ready to send", ErrorType::Internal);
	}

	IStimulationSet* stimulationSet = m_StimulationEncoder.getInputStimulationSet();
	stimulationSet->clear();

	const uint64_t stimulationChunkStartTime = m_LastStimulationDate;
	const uint64_t currentTime               = getPlayerContext().getCurrentTime();

	if (m_SavedStimulations.empty())
	{
		if (currentTime > m_LastStimulationDate)
		{
			m_LastStimulationDate = currentTime;

			OV_ERROR_UNLESS_KRF(m_StimulationEncoder.encodeBuffer(), "Failed to encode stimulation buffer", ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, stimulationChunkStartTime, currentTime),
								"Failed to mark stimulation output as ready to send",
								ErrorType::Internal);
		}
	}
	else
	{
		auto it = m_SavedStimulations.begin();

		for (; it != m_SavedStimulations.end(); ++it)
		{
			const double stimulationDate = it->stimulationDate;

			if (startTime <= stimulationDate && stimulationDate <= endTime)
			{
				stimulationSet->appendStimulation(it->stimulationIdentifier, TimeArithmetics::secondsToTime(it->stimulationDate), TimeArithmetics::secondsToTime(it->stimulationDuration));
				m_LastStimulationDate = TimeArithmetics::secondsToTime(it->stimulationDate);
			}
			else
			{
				const std::string message = "The stimulation is not synced with the stream and will be ignored:"
											" [Value: " + std::to_string(it->stimulationIdentifier) +
											" | Date: " + std::to_string(it->stimulationDate) +
											" | Duration: " + std::to_string(it->stimulationDuration) + "]";

				OV_WARNING_K(message.c_str());
			}
		}

		if (it != m_SavedStimulations.begin()) { m_SavedStimulations.erase(m_SavedStimulations.begin(), it); }


		OV_ERROR_UNLESS_KRF(m_StimulationEncoder.encodeBuffer(), "Failed to encode stimulation buffer", ErrorType::Internal);

		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, stimulationChunkStartTime, m_LastStimulationDate),
							"Failed to mark stimulation output as ready to send", ErrorType::Internal);

		// If there is no more data to send, we push the end.
		if (m_SavedStimulations.empty() && !m_ReaderLib->hasDataToRead())
		{
			OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder.encodeEnd(), "Failed to encode end.", ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, stimulationChunkStartTime, currentTime),
								"Failed to mark signal output as ready to send", ErrorType::Internal);
		}
	}

	return true;
}
