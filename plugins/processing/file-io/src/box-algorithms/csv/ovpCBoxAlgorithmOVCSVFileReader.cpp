#include "ovpCBoxAlgorithmOVCSVFileReader.h"
#include <sstream>
#include <map>
#include <algorithm>


using namespace OpenViBE;
using namespace CSV;
using namespace /*OpenViBE::*/Plugins;
using namespace FileIO;

bool CBoxAlgorithmOVCSVFileReader::initialize()
{
	m_sampling                = 0;
	m_isHeaderSent            = false;
	m_isStimulationHeaderSent = false;

	this->getStaticBoxContext().getOutputType(0, m_typeID);

	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	OV_ERROR_UNLESS_KRF(m_readerLib->openFile(filename.toASCIIString(), EFileAccessMode::Read),
						(ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: " +
							m_readerLib->getLastErrorString())).c_str(),
						Kernel::ErrorType::Internal);

	m_nSamplePerBuffer = 1;

	if (m_typeID == OV_TypeId_Signal)
	{
		m_algorithmEncoder = new Toolkit::TSignalEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::Signal);
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
	{
		m_algorithmEncoder = new Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::StreamedMatrix);
	}
	else if (m_typeID == OV_TypeId_FeatureVector)
	{
		m_algorithmEncoder = new Toolkit::TFeatureVectorEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::FeatureVector);
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		m_algorithmEncoder = new Toolkit::TSpectrumEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->setFormatType(EStreamType::Spectrum);
	}
	else { OV_ERROR_KRF("Output is a type derived from matrix that the box doesn't recognize support", Kernel::ErrorType::BadInput); }

	OV_ERROR_UNLESS_KRF(m_stimEncoder.initialize(*this, 1), "Error during stimulation encoder initialize", Kernel::ErrorType::Internal);

	const char* msg = (ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? ""
																					   : ". Details: " + m_readerLib->getLastErrorString())).c_str();
	if (m_typeID == OV_TypeId_Signal)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getSignalInformation(m_channelNames, m_sampling, m_nSamplePerBuffer), msg, Kernel::ErrorType::Internal);
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getStreamedMatrixInformation(m_dimSizes, m_channelNames), msg, Kernel::ErrorType::Internal);
	}
	else if (m_typeID == OV_TypeId_FeatureVector)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getFeatureVectorInformation(m_channelNames), msg, Kernel::ErrorType::Internal);
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		OV_ERROR_UNLESS_KRF(m_readerLib->getSpectrumInformation(m_channelNames, m_frequencyAbscissa, m_sampling), msg, Kernel::ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::uninitialize()
{
	m_channelNames.clear();
	m_dimSizes.clear();
	m_frequencyAbscissa.clear();

	m_algorithmEncoder.uninitialize();

	OV_ERROR_UNLESS_KRF(m_stimEncoder.uninitialize(), "Failed to uninitialize stimulation encoder", Kernel::ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processClock(CMessage& /*msg*/)
{
	OV_ERROR_UNLESS_KRF(getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(), "Failed to mark clock algorithm as ready to process",
						Kernel::ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileReader::process()
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();
	IMatrix* matrix            = m_algorithmEncoder.getInputMatrix();

	// encode Header if not already encoded
	if (!m_isHeaderSent)
	{
		if (m_typeID == OV_TypeId_Signal)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(2), "Failed to set dimension count", Kernel::ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, m_channelNames.size()), "Failed to set first dimension size", Kernel::ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(1, m_nSamplePerBuffer), "Failed to set second dimension size", Kernel::ErrorType::Internal);

			size_t index = 0;

			for (const std::string& channelName : m_channelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", Kernel::ErrorType::Internal);
			}

			m_algorithmEncoder.getInputSamplingRate() = m_sampling;
		}
		else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(m_dimSizes.size()), "Failed to set dimension count", Kernel::ErrorType::Internal);
			size_t prevDimSize = 0;

			for (size_t d1 = 0; d1 < m_dimSizes.size(); ++d1)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionSize(d1, m_dimSizes[d1]), "Failed to set dimension size " << d1 + 1, Kernel::ErrorType::Internal);

				for (size_t d2 = 0; d2 < m_dimSizes[d1]; ++d2)
				{
					OV_FATAL_UNLESS_K(matrix->setDimensionLabel(d1, d2, m_channelNames[prevDimSize + d2].c_str()), "Failed to set dimension label",
									  Kernel::ErrorType::Internal);
				}

				prevDimSize += m_dimSizes[d1];
			}
		}
		else if (m_typeID == OV_TypeId_FeatureVector)
		{
			OV_FATAL_UNLESS_K(matrix->setDimensionCount(1), "Failed to set dimension count", Kernel::ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, m_channelNames.size()), "Failed to set first dimension size", Kernel::ErrorType::Internal);

			size_t index = 0;
			for (const std::string& channelName : m_channelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", Kernel::ErrorType::Internal);
			}
		}
		else if (m_typeID == OV_TypeId_Spectrum)
		{
			IMatrix* frequencyAbscissaMatrix = m_algorithmEncoder.getInputFrequencyAbcissa();

			OV_FATAL_UNLESS_K(matrix->setDimensionCount(2), "Failed to set dimension count", Kernel::ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(0, m_channelNames.size()), "Failed to set first dimension size", Kernel::ErrorType::Internal);
			OV_FATAL_UNLESS_K(matrix->setDimensionSize(1, m_frequencyAbscissa.size()), "Failed to set first dimension size", Kernel::ErrorType::Internal);
			OV_FATAL_UNLESS_K(frequencyAbscissaMatrix->setDimensionCount(1), "Failed to set dimension count", Kernel::ErrorType::Internal);
			OV_FATAL_UNLESS_K(frequencyAbscissaMatrix->setDimensionSize(0, m_frequencyAbscissa.size()), "Failed to set first dimension size",
							  Kernel::ErrorType::Internal);

			size_t index = 0;
			for (const std::string& channelName : m_channelNames)
			{
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", Kernel::ErrorType::Internal);
			}

			index = 0;
			for (const double& frequencyAbscissaValue : m_frequencyAbscissa)
			{
				frequencyAbscissaMatrix->getBuffer()[index] = frequencyAbscissaValue;
				OV_FATAL_UNLESS_K(matrix->setDimensionLabel(1, index++, std::to_string(frequencyAbscissaValue).c_str()), "Failed to set dimension label",
								  Kernel::ErrorType::Internal);
			}

			m_algorithmEncoder.getInputSamplingRate() = m_sampling;
		}

		OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeHeader(), "Failed to encode signal header", Kernel::ErrorType::Internal);

		m_isHeaderSent = true;
		OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(0, 0, 0), "Failed to mark signal header as ready to send",
							Kernel::ErrorType::Internal);
	}

	const double currentTime = CTime(this->getPlayerContext().getCurrentTime()).toSeconds();

	if (!m_readerLib->hasDataToRead() && m_savedChunks.empty()) { return true; }

	// Fill the chunk buffer if there is no enough data.
	if ((m_savedChunks.empty() || m_savedChunks.back().startTime < currentTime) && m_readerLib->hasDataToRead())
	{
		do
		{
			std::vector<SMatrixChunk> matrixChunk;
			std::vector<SStimulationChunk> stimulationChunk;

			OV_ERROR_UNLESS_KRF(m_readerLib->readSamplesAndEventsFromFile(1, matrixChunk, stimulationChunk),
								(ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: " +
									m_readerLib->getLastErrorString())).c_str(),
								Kernel::ErrorType::Internal);

			m_savedChunks.insert(m_savedChunks.end(), matrixChunk.begin(), matrixChunk.end());
			m_savedStimulations.insert(m_savedStimulations.end(), stimulationChunk.begin(), stimulationChunk.end());
		} while (!m_savedChunks.empty() && m_savedChunks.back().startTime < currentTime && m_readerLib->hasDataToRead());
	}

	if (!m_savedChunks.empty())
	{
		const double chunkStartTime = m_savedChunks.cbegin()->startTime;
		const double chunkEndTime   = m_savedChunks.back().endTime;

		// send stimulations chunk even if there is no stimulations, chunks have to be continued
		OV_ERROR_UNLESS_KRF(this->processStimulation(chunkStartTime, chunkEndTime), "Error during stimulation process", Kernel::ErrorType::Internal);

		size_t chunksToRemove = 0;

		for (const SMatrixChunk& chunk : m_savedChunks)
		{
			if (currentTime > chunk.startTime)
			{
				// move read matrix into buffer to encode
				std::move(chunk.matrix.begin(), chunk.matrix.end(), matrix->getBuffer());

				OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeBuffer(), "Failed to encode signal buffer", Kernel::ErrorType::Internal);

				OV_ERROR_UNLESS_KRF(
					boxContext.markOutputAsReadyToSend(0, CTime(chunk.startTime).time(), CTime(chunk.endTime).time()),
					"Failed to mark signal output as ready to send",
					Kernel::ErrorType::Internal);

				chunksToRemove++;
			}
			else { break; }
		}

		// If there is no more data to send, we push the end.
		if (m_savedChunks.size() == chunksToRemove && !m_readerLib->hasDataToRead())
		{
			OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeEnd(), "Failed to encode end.", Kernel::ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(
				boxContext.markOutputAsReadyToSend(0, CTime(m_savedChunks.back().startTime).time(), CTime(m_savedChunks.back().endTime).time()),
				"Failed to mark signal output as ready to send",
				Kernel::ErrorType::Internal);
		}

		if (chunksToRemove != 0) { m_savedChunks.erase(m_savedChunks.begin(), m_savedChunks.begin() + chunksToRemove); }
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processStimulation(const double startTime, const double endTime)
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();
	if (!m_isStimulationHeaderSent)
	{
		OV_ERROR_UNLESS_KRF(m_stimEncoder.encodeHeader(), "Failed to encode stimulation header", Kernel::ErrorType::Internal);
		m_isStimulationHeaderSent = true;

		OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, 0, 0), "Failed to mark stimulation header as ready to send", Kernel::ErrorType::Internal);
	}

	CStimulationSet& stimulationSet = *m_stimEncoder.getInputStimulationSet();
	stimulationSet.clear();

	const CTime stimulationChunkStartTime = m_lastStimulationDate;
	const CTime currentTime               = getPlayerContext().getCurrentTime();

	if (m_savedStimulations.empty())
	{
		if (currentTime > m_lastStimulationDate)
		{
			m_lastStimulationDate = currentTime;

			OV_ERROR_UNLESS_KRF(m_stimEncoder.encodeBuffer(), "Failed to encode stimulation buffer", Kernel::ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, stimulationChunkStartTime, currentTime),
								"Failed to mark stimulation output as ready to send", Kernel::ErrorType::Internal);
		}
	}
	else
	{
		auto it = m_savedStimulations.begin();

		for (; it != m_savedStimulations.end(); ++it)
		{
			const double stimulationDate = it->date;

			if (startTime <= stimulationDate && stimulationDate <= endTime)
			{
				stimulationSet.append(it->id, CTime(it->date), CTime(it->duration));
				m_lastStimulationDate = CTime(it->date).time();
			}
			else
			{
				const std::string message = "The stimulation is not synced with the stream and will be ignored: [Value: "
											+ std::to_string(it->id) + " | Date: " + std::to_string(it->date) +
											" | Duration: " + std::to_string(it->duration) + "]";

				OV_WARNING_K(message.c_str());
			}
		}

		if (it != m_savedStimulations.begin()) { m_savedStimulations.erase(m_savedStimulations.begin(), it); }


		OV_ERROR_UNLESS_KRF(m_stimEncoder.encodeBuffer(), "Failed to encode stimulation buffer", Kernel::ErrorType::Internal);

		OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, stimulationChunkStartTime, m_lastStimulationDate),
							"Failed to mark stimulation output as ready to send", Kernel::ErrorType::Internal);

		// If there is no more data to send, we push the end.
		if (m_savedStimulations.empty() && !m_readerLib->hasDataToRead())
		{
			OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeEnd(), "Failed to encode end.", Kernel::ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, stimulationChunkStartTime, currentTime),
								"Failed to mark signal output as ready to send", Kernel::ErrorType::Internal);
		}
	}

	return true;
}
