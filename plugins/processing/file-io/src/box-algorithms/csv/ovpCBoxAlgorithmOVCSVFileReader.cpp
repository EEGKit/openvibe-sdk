#include "ovpCBoxAlgorithmOVCSVFileReader.h"
#include <iostream>
#include <sstream>
#include <map>
#include <cmath>  // std::ceil() on Linux

#include <openvibe/ovITimeArithmetics.h>

using namespace OpenViBE;
using namespace OpenViBE::CSV;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

CBoxAlgorithmOVCSVFileReader::CBoxAlgorithmOVCSVFileReader(void)
	: m_ReaderLib(createCSVLib(), releaseCSVLib)
	, m_AlgorithmEncoder(nullptr)
	, m_lastStimulationDate(0)
	, m_SamplingRate(0)
	, m_IsHeaderSent(false)
	, m_IsStimulationHeaderSent(false)
{
}

unsigned long long CBoxAlgorithmOVCSVFileReader::getClockFrequency(void)
{
	return 128LL << 32; // the box clock frequency
}

bool CBoxAlgorithmOVCSVFileReader::initialize(void)
{
	m_SamplingRate = 0;
	m_AlgorithmEncoder = nullptr;
	m_IsHeaderSent = false;
	m_IsStimulationHeaderSent = false;

	this->getStaticBoxContext().getOutputType(0, m_TypeIdentifier);

	const OpenViBE::CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	OV_ERROR_UNLESS_KRF(m_ReaderLib->openFile(filename.toASCIIString(), EFileAccessMode::Read),
		(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str(),
		ErrorType::Internal);

	m_SampleCountPerBuffer = 1;
	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmOVCSVFileReader >(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::Signal);
	}
	else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TStreamedMatrixEncoder < CBoxAlgorithmOVCSVFileReader >(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::StreamedMatrix);
	}
	else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TFeatureVectorEncoder < CBoxAlgorithmOVCSVFileReader >(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::FeatureVector);
	}
	else
	{
		OV_ERROR_KRF("Output is a type derived from matrix that the box doesn't recognize support", ErrorType::BadInput);
	}

	OV_ERROR_UNLESS_KRF(m_StimulationEncoder.initialize(*this, 1),
		"Error during stimulation encoder initialize",
		ErrorType::Internal);
	std::vector<SMatrixChunk> matrixChunks;
	std::vector<SStimulationChunk> stimulationChunks;
	OV_ERROR_UNLESS_KRF(m_ReaderLib->readSamplesAndEventsFromFile(0, matrixChunks, stimulationChunks),
		(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str(),
		ErrorType::Internal);

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getSignalInformation(m_ChannelNames, m_SamplingRate, m_SampleCountPerBuffer),
			(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str(),
			ErrorType::Internal);
	}
	else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getStreamedMatrixInformation(m_DimensionSizes, m_ChannelNames),
			(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str(),
			ErrorType::Internal);
	}
	else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getFeatureVectorInformation(m_ChannelNames),
			(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str(),
			ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::uninitialize(void)
{
	if (m_AlgorithmEncoder)
	{
		OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder->uninitialize(),
			"Failed to uninitialize algorithm encoder",
			ErrorType::Internal);
		delete m_AlgorithmEncoder;
		m_AlgorithmEncoder = nullptr;
	}

	m_ChannelNames.clear();
	m_DimensionSizes.clear();
	OV_ERROR_UNLESS_KRF(m_StimulationEncoder.uninitialize(),
		"Failed to uninitialize stimulation encoder",
		ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processClock(IMessageClock& rMessageClock)
{
	OV_ERROR_UNLESS_KRF(getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(),
		"Failed to mark clock algorithm as ready to process",
		ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileReader::process(void)
{
	IMatrix* matrix(nullptr);

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		OV_ERROR_UNLESS_KRF(matrix = (dynamic_cast<OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmOVCSVFileReader >*>(m_AlgorithmEncoder))->getInputMatrix(),
			"Failed to get input matrix",
			ErrorType::Internal);
	}
	else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
	{
		OV_ERROR_UNLESS_KRF(matrix = (dynamic_cast<OpenViBEToolkit::TStreamedMatrixEncoder < CBoxAlgorithmOVCSVFileReader >*>(m_AlgorithmEncoder))->getInputMatrix(),
			"Failed to get input matrix",
			ErrorType::Internal);
	}
	else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
	{
		OV_ERROR_UNLESS_KRF(matrix = (dynamic_cast<OpenViBEToolkit::TFeatureVectorEncoder < CBoxAlgorithmOVCSVFileReader >*>(m_AlgorithmEncoder))->getInputMatrix(),
			"Failed to get input matrix",
			ErrorType::Internal);
	}
	// encode Header if not already encoded
	if (!m_IsHeaderSent)
	{
		if (m_TypeIdentifier == OV_TypeId_Signal)
		{
			OV_ERROR_UNLESS_KRF(matrix->setDimensionCount(2),
				"Failed to set dimension count",
				ErrorType::Internal);
			OV_ERROR_UNLESS_KRF(matrix->setDimensionSize(0, static_cast<uint32>(m_ChannelNames.size())),
				"Failed to set first dimension size",
				ErrorType::Internal);
			OV_ERROR_UNLESS_KRF(matrix->setDimensionSize(1, m_SampleCountPerBuffer),
				"Failed to set second dimension size",
				ErrorType::Internal);

			unsigned int index = 0;
			for (const std::string& channelName : m_ChannelNames)
			{
				OV_ERROR_UNLESS_KRF(matrix->setDimensionLabel(0, index++, channelName.c_str()),
					"Failed to set dimension label",
					ErrorType::Internal);
			}

			dynamic_cast<OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmOVCSVFileReader >*>(m_AlgorithmEncoder)->getInputSamplingRate() = m_SamplingRate;
		}
		else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
		{
			OV_ERROR_UNLESS_KRF(matrix->setDimensionCount(static_cast<uint32>(m_DimensionSizes.size())),
				"Failed to set dimension count",
				ErrorType::Internal);
			unsigned int previousDimensionSize = 0;
			for (size_t index = 0; index < m_DimensionSizes.size(); index++)
			{
				OV_ERROR_UNLESS_KRF(matrix->setDimensionSize(static_cast<uint32>(index), m_DimensionSizes[index]),
					"Failed to set dimension size " << static_cast<uint32>(index + 1),
					ErrorType::Internal);
				for (unsigned int labelIndex = 0; labelIndex < m_DimensionSizes[index]; labelIndex++)
				{
					OV_ERROR_UNLESS_KRF(matrix->setDimensionLabel(static_cast<uint32>(index), labelIndex, m_ChannelNames[previousDimensionSize + labelIndex].c_str()),
						"Failed to set dimension label",
						ErrorType::Internal);
				}

				previousDimensionSize += m_DimensionSizes[index];
			}
		}
		else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
		{
			OV_ERROR_UNLESS_KRF(matrix->setDimensionCount(1),
				"Failed to set dimension count",
				ErrorType::Internal);
			OV_ERROR_UNLESS_KRF(matrix->setDimensionSize(0, static_cast<uint32>(m_ChannelNames.size())),
				"Failed to set first dimension size",
				ErrorType::Internal);

			unsigned int index = 0;
			for (const std::string& channelName : m_ChannelNames)
			{
				OV_ERROR_UNLESS_KRF(matrix->setDimensionLabel(0, index++, channelName.c_str()),
					"Failed to set dimension label",
					ErrorType::Internal);
			}
		}

		OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder->encodeHeader(),
			"Failed to encode signal header",
			ErrorType::Internal);
		m_IsHeaderSent = true;
		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0),
			"Failed to mark signal header as ready to send",
			ErrorType::Internal);
	}

	std::vector<SMatrixChunk> matrixChunk;
	std::vector<SStimulationChunk> stimulationChunk;
	do
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->readSamplesAndEventsFromFile(1, matrixChunk, stimulationChunk),
			(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : ". Details: " + m_ReaderLib->getLastErrorString())).c_str(),
			ErrorType::Internal);
		m_SavedChunks.insert(m_SavedChunks.end(), matrixChunk.begin(), matrixChunk.end());
		m_SavedStimulations.insert(m_SavedStimulations.end(), stimulationChunk.begin(), stimulationChunk.end());
	} while (!matrixChunk.empty()
		&& matrixChunk.begin()->startTime < ITimeArithmetics::timeToSeconds(this->getPlayerContext().getCurrentTime()));


	if (!m_SavedChunks.empty())
	{
		unsigned int chunksToRemove = 0;
		for (const SMatrixChunk& chunk : m_SavedChunks)
		{
			if (ITimeArithmetics::timeToSeconds(getPlayerContext().getCurrentTime()) > chunk.startTime)
			{
				// copy read matrix into buffer to encode
				std::copy(chunk.matrix.begin(), chunk.matrix.end(), matrix->getBuffer());

				OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder->encodeBuffer(),
					"Failed to encode signal buffer",
					ErrorType::Internal);
				OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0,
					ITimeArithmetics::secondsToTime(chunk.startTime),
					ITimeArithmetics::secondsToTime(chunk.endTime)),
					"Failed to mark signal output as ready to send",
					ErrorType::Internal);
				chunksToRemove++;
			}
		}

		while (chunksToRemove != 0)
		{
			m_SavedChunks.pop_front();
			chunksToRemove--;
		}
		// send stimulations chunk even if there is no stimulations, chunks have to be continued
		OV_ERROR_UNLESS_KRF(processStimulation(),
			"Error during stimulation process",
			ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processStimulation()
{
	if (!m_IsStimulationHeaderSent)
	{
		OV_ERROR_UNLESS_KRF(m_StimulationEncoder.encodeHeader(),
			"Failed to encode stimulation header",
			ErrorType::Internal);
		m_IsStimulationHeaderSent = true;

		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1, 0, 0),
			"Failed to mark stimulation header as ready to send",
			ErrorType::Internal);
	}

	IStimulationSet* stimulationSet = m_StimulationEncoder.getInputStimulationSet();
	stimulationSet->clear();

	unsigned long long stimulationChunkStartTime = m_lastStimulationDate;
	unsigned long long currentTime = getPlayerContext().getCurrentTime();
	if (m_SavedStimulations.empty())
	{
		if (currentTime > m_lastStimulationDate)
		{
			m_lastStimulationDate = currentTime;
			OV_ERROR_UNLESS_KRF(m_StimulationEncoder.encodeBuffer(),
				"Failed to encode stimulation buffer",
				ErrorType::Internal);
			OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1,
				stimulationChunkStartTime,
				currentTime),
				"Failed to mark stimulation output as ready to send",
				ErrorType::Internal);
		}
	}
	else
	{
		unsigned int chunksToRemove = 0;
		for (const SStimulationChunk& chunk : m_SavedStimulations)
		{
			if (currentTime > ITimeArithmetics::secondsToTime(chunk.stimulationDate))
			{
				stimulationSet->appendStimulation(chunk.stimulationIdentifier,
					ITimeArithmetics::secondsToTime(chunk.stimulationDate),
					ITimeArithmetics::secondsToTime(chunk.stimulationDuration));
				chunksToRemove++;
			}
		}

		while (chunksToRemove != 0)
		{
			m_SavedStimulations.pop_front();
			chunksToRemove--;
		}

		m_lastStimulationDate = currentTime;
		OV_ERROR_UNLESS_KRF(m_StimulationEncoder.encodeBuffer(),
			"Failed to encode stimulation buffer",
			ErrorType::Internal);
		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1,
			stimulationChunkStartTime,
			currentTime),
			"Failed to mark stimulation output as ready to send",
			ErrorType::Internal);
	}
	return true;
}
