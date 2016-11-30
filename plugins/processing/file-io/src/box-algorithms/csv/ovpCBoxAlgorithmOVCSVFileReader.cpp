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

namespace
{
	std::vector < std::string > split(const std::string& string, const std::string& sep)
	{
		std::vector < std::string > result;
		std::string::size_type i = 0;
		std::string::size_type j = 0;
		while ((j = string.find(sep, i)) != std::string::npos)
		{
			result.push_back(std::string(string, i, j - i));
			i = j + sep.size();
		}
		//the last element without the \n character
		result.push_back(std::string(string, i, string.size() - 1 - i));

		return result;
	}
};

CBoxAlgorithmOVCSVFileReader::CBoxAlgorithmOVCSVFileReader(void)
	: m_ReaderLib(createCSVLib(), releaseCSVLib)
	, m_SamplingRate(0)
	, m_AlgorithmEncoder(nullptr)
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
		(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : "Details: " + m_ReaderLib->getLastErrorString())).c_str(),
		ErrorType::Internal);

	m_SampleCountPerBuffer = 1;
	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		m_AlgorithmEncoder = new OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmOVCSVFileReader >(*this, 0);
		m_ReaderLib->setFormatType(EStreamType::Signal);
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
		(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : "Details: " + m_ReaderLib->getLastErrorString())).c_str(),
		ErrorType::Internal);

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		OV_ERROR_UNLESS_KRF(m_ReaderLib->getSignalInformation(m_ChannelNames, m_SamplingRate, m_SampleCountPerBuffer),
			(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : "Details: " + m_ReaderLib->getLastErrorString())).c_str(),
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

	OV_ERROR_UNLESS_KRF(m_StimulationEncoder.uninitialize(),
		"Failed to unitialize the stimulation encoder",
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
	OV_ERROR_UNLESS_KRF(processSignal(),
		"Error during signal process",
		ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processSignal(void)
{
	IMatrix* matrix = ((OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmOVCSVFileReader >*)m_AlgorithmEncoder)->getInputMatrix();
	std::vector<SMatrixChunk> matrixChunk;
	std::vector<SMatrixChunk> matrixChunkBuffer;
	std::vector<SStimulationChunk> stimulationChunk;
	SMatrixChunk firstSampleOfSecondEpoch(0.0, 0.0, { 0.0 }, 0);

	// encode Header if not already encoded
	if (!m_IsHeaderSent)
	{
		matrix->setDimensionCount(2);
		matrix->setDimensionSize(0, m_ChannelNames.size());
		matrix->setDimensionSize(1, m_SampleCountPerBuffer);

		unsigned int index = 0;
		for (const std::string& channelName : m_ChannelNames)
		{
			OV_ERROR_UNLESS_KRF(matrix->setDimensionLabel(0, index++, channelName.c_str()),
				"Failed to set dimension label",
				ErrorType::Internal);
		}
		
		((OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmOVCSVFileReader >*)m_AlgorithmEncoder)->getInputSamplingRate() = m_SamplingRate;
		OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder->encodeHeader(),
			"Failed to encode signal header",
			ErrorType::Internal);
		m_IsHeaderSent = true;
		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0),
			"Failed to mark signal header as ready to send",
		ErrorType::Internal);
	}

	OV_ERROR_UNLESS_KRF(m_ReaderLib->readSamplesAndEventsFromFile(1, matrixChunk, stimulationChunk),
		(ICSVLib::getLogError(m_ReaderLib->getLastLogError()) + (m_ReaderLib->getLastErrorString().empty() ? "" : "Details: " + m_ReaderLib->getLastErrorString())).c_str(),
		ErrorType::Internal);

	if (!matrixChunk.empty())
	{
		// copy read matrix into buffer to encode

		std::copy(matrixChunk.back().matrix.begin(), matrixChunk.back().matrix.end(), matrix->getBuffer());

		OV_ERROR_UNLESS_KRF(m_AlgorithmEncoder->encodeBuffer(),
			"Failed to encode signal buffer",
			ErrorType::Internal);
		OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(0,
			ITimeArithmetics::secondsToTime(matrixChunk.back().startTime),
			ITimeArithmetics::secondsToTime(matrixChunk.back().endTime)),
			"Failed to mark signal output as ready to send",
			ErrorType::Internal);

		// send stimulations if there is some
		if (!stimulationChunk.empty())
		{
			OV_ERROR_UNLESS_KRF(processStimulation(matrixChunk, stimulationChunk),
				"Error during stimulation process",
				ErrorType::Internal);
		}
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processStimulation(const std::vector<SMatrixChunk>& matrixChunk, const std::vector<SStimulationChunk>& stimulationChunk)
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

	for (const SStimulationChunk& chunk : stimulationChunk)
	{
		stimulationSet->appendStimulation(chunk.stimulationIdentifier,
			ITimeArithmetics::secondsToTime(chunk.stimulationDate),
			ITimeArithmetics::secondsToTime(chunk.stimulationDuration));
	}

	OV_ERROR_UNLESS_KRF(m_StimulationEncoder.encodeBuffer(),
		"Failed to encode stimulation buffer",
		ErrorType::Internal);
	OV_ERROR_UNLESS_KRF(this->getDynamicBoxContext().markOutputAsReadyToSend(1,
		ITimeArithmetics::secondsToTime(matrixChunk.front().startTime),
		ITimeArithmetics::secondsToTime(matrixChunk.back().endTime)),
		"Failed to mark stimulation output as ready to send",
		ErrorType::Internal);
	return true;
}