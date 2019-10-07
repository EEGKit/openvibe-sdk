#include "ovpCBoxAlgorithmOVCSVFileReader.h"
#include <sstream>
#include <map>
#include <algorithm>

#include <openvibe/ovTimeArithmetics.h>

using namespace OpenViBE;
using namespace CSV;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace FileIO;
using namespace TimeArithmetics;

CBoxAlgorithmOVCSVFileReader::CBoxAlgorithmOVCSVFileReader()
	: m_readerLib(createCSVHandler(), releaseCSVHandler), m_isHeaderSent(false)
	  , m_isStimulationHeaderSent(false) {}

uint64_t CBoxAlgorithmOVCSVFileReader::getClockFrequency()
{
	return 128LL << 32; // the box clock frequency
}

bool CBoxAlgorithmOVCSVFileReader::initialize()
{
	m_samplingRate            = 0;
	m_isHeaderSent            = false;
	m_isStimulationHeaderSent = false;

	this->getStaticBoxContext().getOutputType(0, m_typeID);

	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	OV_ERROR_UNLESS_KRF(m_readerLib->openFile(filename.toASCIIString(), EFileAccessMode::Read),
						(ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: " +
							m_readerLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	m_nSamplePerBuffer = 1;

	if (m_typeID == OV_TypeId_Signal)
	{
		m_algorithmEncoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::Signal);
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
	{
		m_algorithmEncoder = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::StreamedMatrix);
	}
	else if (m_typeID == OV_TypeId_FeatureVector)
	{
		m_algorithmEncoder = new OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::FeatureVector);
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		m_algorithmEncoder = new OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::Spectrum);
	}
	else { OV_ERROR_KRF("Output is a type derived from matrix that the box doesn't recognize support", ErrorType::BadInput); }

	OV_ERROR_UNLESS_KRF(m_stimulationEncoder.initialize(*this, 1), "Error during stimulation encoder initialize", ErrorType::Internal);

	const char *msg = (ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: " + m_readerLib->getLastErrorString())).c_str();
	if (m_typeID == OV_TypeId_Signal)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getSignalInformation(m_channelNames, m_samplingRate, m_nSamplePerBuffer), msg, ErrorType::Internal);
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getStreamedMatrixInformation(m_dimSizes, m_channelNames), msg, ErrorType::Internal);
	}
	else if (m_typeID == OV_TypeId_FeatureVector)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getFeatureVectorInformation(m_channelNames), msg, ErrorType::Internal);
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getSpectrumInformation(m_channelNames, m_frequencyAbscissa, m_samplingRate), msg, ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::uninitialize()
{
	m_channelNames.clear();
	m_dimSizes.clear();
	m_frequencyAbscissa.clear();

	m_algorithmEncoder.uninitialize();

	OV_ERROR_UNLESS_KRF(m_stimulationEncoder.uninitialize(), "Failed to uninitialize stimulation encoder", ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processClock(IMessageClock& /*messageClock*/)
{
	OV_ERROR_UNLESS_KRF(getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(), "Failed to mark clock algorithm as ready to process", ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileReader::process()
{
	IMatrix* matrix = m_algorithmEncoder.getInputMatrix();

	// encode Header if not already encoded
	if (!m_isHeaderSent)
	{
		if (m_typeID == OV_TypeId_Signal)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(2), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, uint32_t(m_channelNames.size())), "Failed to set first dimension size", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(1, m_nSamplePerBuffer), "Failed to set second dimension size", ErrorType::Internal);

			uint32_t index = 0;

			for (const std::string& channelName : m_channelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", ErrorType::Internal);
			}

			m_algorithmEncoder.getInputSamplingRate() = m_samplingRate;
		}
		else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(uint32_t(m_dimSizes.size())), "Failed to set dimension count", ErrorType::Internal);
			uint32_t previousDimensionSize = 0;

			for (size_t index = 0; index < m_dimSizes.size(); ++index)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionSize(uint32_t(index), m_dimSizes[index]), "Failed to set dimension size " << uint32_t(index + 1), ErrorType::Internal);

				for (uint32_t labelIndex = 0; labelIndex < m_dimSizes[index]; ++labelIndex)
				{
					OV_FATAL_UNLESS_K(matrix->setDimensionLabel(uint32_t(index), labelIndex, m_channelNames[previousDimensionSize + labelIndex].c_str()),
									  "Failed to set dimension label", ErrorType::Internal);
				}

				previousDimensionSize += m_dimSizes[index];
			}
		}
		else if (m_typeID == OV_TypeId_FeatureVector)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(1), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, uint32_t(m_channelNames.size())), "Failed to set first dimension size", ErrorType::Internal);

			uint32_t index = 0;
			for (const std::string& channelName : m_channelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", ErrorType::Internal);
			}
		}
		else if (m_typeID == OV_TypeId_Spectrum)
		{
			IMatrix* frequencyAbscissaMatrix = m_algorithmEncoder.getInputFrequencyAbcissa();

			OV_FATAL_UNLESS_K(matrix->setDimensionCount(2), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, uint32_t(m_channelNames.size())), "Failed to set first dimension size", ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(1, uint32_t(m_frequencyAbscissa.size())), "Failed to set first dimension size", ErrorType::Internal);
			OV_FATAL_UNLESS_K(frequencyAbscissaMatrix->setDimensionCount(1), "Failed to set dimension count", ErrorType::Internal);
			OV_FATAL_UNLESS_K(frequencyAbscissaMatrix->setDimensionSize(0, uint32_t(m_frequencyAbscissa.size())), "Failed to set first dimension size", ErrorType::Internal);

			uint32_t index = 0;
			for (const std::string& channelName : m_channelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", ErrorType::Internal);
			}

			index = 0;
			for (const double& frequencyAbscissaValue : m_frequencyAbscissa)
			{
				frequencyAbscissaMatrix->getBuffer()[index] = frequencyAbscissaValue;
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(1, index++, std::to_string(frequencyAbscissaValue).c_str()),
								  "Failed to set dimension label", ErrorType::Internal);
			}

			m_algorithmEncoder.getInputSamplingRate() = m_samplingRate;
		}

		OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeHeader(), "Failed to encode signal header", ErrorType::Internal);

		m_isHeaderSent = true;
		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0), "Failed to mark signal header as ready to send", ErrorType::Internal);
	}

	const double currentTime = timeToSeconds(this->getPlayerContext().getCurrentTime());

	if (!m_readerLib->hasDataToRead() && m_savedChunks.empty()) { return true; }

	// Fill the chunk buffer if there is no enough data.
	if ((m_savedChunks.empty() || m_savedChunks.back().startTime < currentTime) && m_readerLib->hasDataToRead())
	{
		do
		{
			std::vector<SMatrixChunk> matrixChunk;
			std::vector<SStimulationChunk> stimulationChunk;

			OV_ERROR_UNLESS_KRF(m_readerLib->readSamplesAndEventsFromFile(1, matrixChunk, stimulationChunk),
								(ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: " + m_readerLib->getLastErrorString())).c_str(),
								ErrorType::Internal);

			m_savedChunks.insert(m_savedChunks.end(), matrixChunk.begin(), matrixChunk.end());
			m_savedStimulations.insert(m_savedStimulations.end(), stimulationChunk.begin(), stimulationChunk.end());
		} while (!m_savedChunks.empty() && m_savedChunks.back().startTime < currentTime && m_readerLib->hasDataToRead());
	}

	if (!m_savedChunks.empty())
	{
		const double chunkStartTime = m_savedChunks.cbegin()->startTime;
		const double chunkEndTime   = m_savedChunks.back().endTime;

		// send stimulations chunk even if there is no stimulations, chunks have to be continued
		OV_ERROR_UNLESS_KRF(this->processStimulation(chunkStartTime, chunkEndTime), "Error during stimulation process", ErrorType::Internal);

		uint32_t chunksToRemove = 0;

		for (const SMatrixChunk& chunk : m_savedChunks)
		{
			if (currentTime > chunk.startTime)
			{
				// move read matrix into buffer to encode
				std::move(chunk.matrix.begin(), chunk.matrix.end(), matrix->getBuffer());

				OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeBuffer(), "Failed to encode signal buffer", ErrorType::Internal);

				OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0, secondsToTime(chunk.startTime), TimeArithmetics::secondsToTime(chunk.endTime)),
									"Failed to mark signal output as ready to send",
									ErrorType::Internal);

				chunksToRemove++;
			}
			else { break; }
		}

		// If there is no more data to send, we push the end.
		if (m_savedChunks.size() == chunksToRemove && !m_readerLib->hasDataToRead())
		{
			OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeEnd(), "Failed to encode end.", ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0, secondsToTime(m_savedChunks.back().startTime), TimeArithmetics::secondsToTime(m_savedChunks.back().endTime)),
								"Failed to mark signal output as ready to send",
								ErrorType::Internal);
		}

		if (chunksToRemove != 0) { m_savedChunks.erase(m_savedChunks.begin(), m_savedChunks.begin() + chunksToRemove); }
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processStimulation(const double startTime, const double endTime)
{
	if (!m_isStimulationHeaderSent)
	{
		OV_ERROR_UNLESS_KRF(m_stimulationEncoder.encodeHeader(), "Failed to encode stimulation header", ErrorType::Internal);
		m_isStimulationHeaderSent = true;

		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, 0, 0),
							"Failed to mark stimulation header as ready to send", ErrorType::Internal);
	}

	IStimulationSet* stimulationSet = m_stimulationEncoder.getInputStimulationSet();
	stimulationSet->clear();

	const uint64_t stimulationChunkStartTime = m_lastStimulationDate;
	const uint64_t currentTime               = getPlayerContext().getCurrentTime();

	if (m_savedStimulations.empty())
	{
		if (currentTime > m_lastStimulationDate)
		{
			m_lastStimulationDate = currentTime;

			OV_ERROR_UNLESS_KRF(m_stimulationEncoder.encodeBuffer(), "Failed to encode stimulation buffer", ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, stimulationChunkStartTime, currentTime),
								"Failed to mark stimulation output as ready to send",
								ErrorType::Internal);
		}
	}
	else
	{
		auto it = m_savedStimulations.begin();

		for (; it != m_savedStimulations.end(); ++it)
		{
			const double stimulationDate = it->stimulationDate;

			if (startTime <= stimulationDate && stimulationDate <= endTime)
			{
				stimulationSet->appendStimulation(it->stimulationIdentifier, secondsToTime(it->stimulationDate), secondsToTime(it->stimulationDuration));
				m_lastStimulationDate = secondsToTime(it->stimulationDate);
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

		if (it != m_savedStimulations.begin()) { m_savedStimulations.erase(m_savedStimulations.begin(), it); }


		OV_ERROR_UNLESS_KRF(m_stimulationEncoder.encodeBuffer(), "Failed to encode stimulation buffer", ErrorType::Internal);

		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, stimulationChunkStartTime, m_lastStimulationDate),
							"Failed to mark stimulation output as ready to send", ErrorType::Internal);

		// If there is no more data to send, we push the end.
		if (m_savedStimulations.empty() && !m_readerLib->hasDataToRead())
		{
			OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeEnd(), "Failed to encode end.", ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, stimulationChunkStartTime, currentTime),
								"Failed to mark signal output as ready to send", ErrorType::Internal);
		}
	}

	return true;
}
