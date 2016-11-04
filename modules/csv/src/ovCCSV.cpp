/*
* MENSIA TECHNOLOGIES CONFIDENTIAL
* ________________________________
*
*  [2012] - [2013] Mensia Technologies SA
*  Copyright, All Rights Reserved.
*
* NOTICE: All information contained herein is, and remains
* the property of Mensia Technologies SA.
* The intellectual and technical concepts contained
* herein are proprietary to Mensia Technologies SA
* and are covered copyright law.
* Dissemination of this information or reproduction of this material
* is strictly forbidden unless prior written permission is obtained
* from Mensia Technologies SA.
*/

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#include <cmath>
#include <limits>
#endif

#include <sstream>

#include "ovCCSV.hpp"

using namespace OpenViBE;
using namespace OpenViBE::CSV;

namespace {
	static const int s_StimulationDateColumnNbr = 1;
	static const int s_ColumnStartMatrixIndex = 2;
	static const int s_StimulationIdentifierColumnNbr = 2;
	static const int s_StimulationDateColumn = 2;
	static const int s_ColumnEndMatrixIndex = 3;
	static const std::string s_EventIdentifierColumn = "Event Id";
	static const std::string s_EventDateColumn = "Event Date";
	static const std::string s_EventDurationColumn = "Event Duration";
	void split(const std::string& string, char delimitor, std::vector<std::string>& element)
	{
		std::stringstream stringStream(string);
		std::string item;

		while (std::getline(stringStream, item, delimitor))
		{
			element.push_back(item);
		}

		// if the list of channels ends with a trailing ; then we have to count the empty channel
		if (string.empty() || *string.rbegin() == delimitor)
		{
			element.push_back("");
		}
	}
}

CCSVLib::CCSVLib(void)
	: m_LastStringError("No more information"),
	m_Separator(','),
	m_InternalDataSeparator(':'),
	m_DimensionCount(0),
	m_DimensionSizes({}),
	m_NoEventSince(0),
	m_SamplingRate(0),
	m_ColumnCount(0),
	m_IsSetInputType(false),
	m_IsFirstLineWritten(false),
	m_IsHeaderRead(false),
	m_IsSetInfoCalled(false),
	m_HasEpoch(false)
{
}

CCSVLib::~CCSVLib(void)
{
	closeFile();
}

void CCSVLib::setFormatType(EStreamType typeIdentifier)
{
	m_InputTypeIdentifier = typeIdentifier;
	m_IsSetInputType = true;
}

EStreamType CCSVLib::getFormatType(void)
{
	return m_InputTypeIdentifier;
}

bool CCSVLib::setSignalInformation(const std::vector<std::string>& channelNames, unsigned int samplingFrequency, unsigned int sampleCountPerBuffer)
{
	if (m_InputTypeIdentifier != EStreamType::Signal)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (m_IsSetInfoCalled)
	{
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}

	m_IsSetInfoCalled = true;
	if (channelNames.empty())
	{
		m_LogError = LogErrorCodes_NoChannelsName;
		return false;
	}

	m_SamplingRate = samplingFrequency;
	m_DimensionLabels = channelNames;
	m_SampleCountPerBuffer = sampleCountPerBuffer;
	return true;
}

bool CCSVLib::getSignalInformation(std::vector<std::string>& channelNames, unsigned int& samplingFrequency)
{
	if (m_InputTypeIdentifier != EStreamType::Signal)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (!m_IsHeaderRead)
	{
		m_LastStringError = "No header read yet";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (!channelNames.empty())
	{
		m_LastStringError = "channelNames already filled";
		m_LogError = LogErrorCodes_WrongParameters;
		return false;
	}
	else if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (samplingFrequency == 0)
	{
		m_LastStringError = "Sampling rate isn't recuparated";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	samplingFrequency = m_SamplingRate;
	return true;
}

bool CCSVLib::setSpectrumInformation(const std::vector<std::string>& channelNames, std::vector<std::array<double, 2>> frequencyBands)
{
	if (m_InputTypeIdentifier != EStreamType::Spectrum)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (m_IsSetInfoCalled)
	{
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}
	m_IsSetInfoCalled = true;

	if (channelNames.empty())
	{
		m_LogError = LogErrorCodes_NoChannelsName;
		return false;
	}

	m_DimensionLabels = channelNames;
	m_DimensionSizes = { static_cast<unsigned int>(channelNames.size()), static_cast<unsigned int>(frequencyBands.size()) };
	m_FrequencyBands = frequencyBands;
	return true;
}

bool CCSVLib::getSpectrumInformation(std::vector<std::string>& channelNames, std::vector<double>& frequencyBands)
{
	if (m_InputTypeIdentifier != EStreamType::Spectrum)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (!m_IsHeaderRead)
	{
		m_LastStringError = "No header read yet";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (!channelNames.empty())
	{
		m_LastStringError = "channelNames already filled";
		m_LogError = LogErrorCodes_WrongParameters;
		return false;
	}
	else if (!frequencyBands.empty())
	{
		m_LastStringError = "frequencyBands already filled";
		m_LogError = LogErrorCodes_WrongParameters;
		return false;
	}
	else if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (m_FrequenceBandsBuffer.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	frequencyBands = m_FrequenceBandsBuffer;
	return true;
}

bool CCSVLib::setFeatureVectorInformation(const std::vector<std::string>& channelNames)
{
	if (m_InputTypeIdentifier != EStreamType::FeatureVector)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (m_IsSetInfoCalled)
	{
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}
	m_IsSetInfoCalled = true;
	if (channelNames.empty())
	{
		m_LogError = LogErrorCodes_NoChannelsName;
		return false;
	}
	m_DimensionLabels = channelNames;
	return true;
}

bool CCSVLib::getFeatureVectorInformation(std::vector<std::string>& channelNames)
{
	if (m_InputTypeIdentifier != EStreamType::FeatureVector)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (!m_IsHeaderRead)
	{
		m_LastStringError = "No header read yet";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (!channelNames.empty())
	{
		m_LastStringError = "channelNames already filled";
		m_LogError = LogErrorCodes_WrongParameters;
		return false;
	}
	else if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	return true;
}

bool CCSVLib::setCovarianceMatrixInformation(const std::vector<std::string>& channelNames)
{
	if (m_InputTypeIdentifier != EStreamType::CovarianceMatrix)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (m_IsSetInfoCalled)
	{
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}

	m_IsSetInfoCalled = true;
	if (channelNames.empty())
	{
		m_LogError = LogErrorCodes_NoChannelsName;
		return false;
	}
	m_DimensionLabels = channelNames;
	return true;
}

bool CCSVLib::getCovarianceMatrixInformation(std::vector<std::string>& channelNames)
{
	if (m_InputTypeIdentifier != EStreamType::CovarianceMatrix)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (!m_IsHeaderRead)
	{
		m_LastStringError = "No header read yet";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (!channelNames.empty())
	{
		m_LastStringError = "channelNames already filled";
		m_LogError = LogErrorCodes_WrongParameters;
		return false;
	}
	else if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	return true;
}

bool CCSVLib::setStreamedMatrixInformation(const std::vector<unsigned int>& dimensionSizes, const std::vector<std::string>& labels)
{
	if (m_IsSetInfoCalled)
	{
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}

	if (m_InputTypeIdentifier != EStreamType::StreamedMatrix)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (dimensionSizes.empty())
	{
		m_LogError = LogErrorCodes_DimensionSizeEmpty;
		return false;
	}
	unsigned int size = 0;
	for (size_t index = 0; index < dimensionSizes.size(); index++)
	{
		size += dimensionSizes[index];
	}

	if (size != labels.size())
	{
		m_LogError = LogErrorCodes_WrongDimensionSize;
		return false;
	}
	m_IsSetInfoCalled = true;
	m_DimensionSizes = dimensionSizes;
	m_DimensionCount = m_DimensionSizes.size();
	m_DimensionLabels = labels;
	return true;
}

bool CCSVLib::getStreamedMatrixInformation(std::vector<unsigned int>& dimensionSizes, std::vector<std::string>& labels)
{
	if (m_InputTypeIdentifier != EStreamType::StreamedMatrix)
	{
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (!m_IsHeaderRead)
	{
		m_LastStringError = "No header read yet";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (!dimensionSizes.empty())
	{
		m_LastStringError = "dimensionSizes already filled";
		m_LogError = LogErrorCodes_WrongParameters;
		return false;
	}
	else if (!labels.empty())
	{
		m_LastStringError = "channelNames already filled";
		m_LogError = LogErrorCodes_WrongParameters;
		return false;
	}
	else if (m_DimensionSizes.empty())
	{
		m_LastStringError = "no dimension sizes, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}
	else if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	dimensionSizes = m_DimensionSizes;
	labels = m_DimensionLabels;
	return true;
}

bool CCSVLib::openFile(const std::string& fileName, EFileAccessMode mode)
{
	if (m_Fs.is_open())
	{
		m_LogError = LogErrorCodes_CantOpenFile;
		return false;
	}

	m_Filename = fileName;
	if (mode == EFileAccessMode::Write)
	{
		FILE *file = fopen(m_Filename.c_str(), "w");
		if (!file)
		{
			m_LogError = LogErrorCodes_CantOpenFile;
			return false;
		}

		if (fclose(file) != 0)
		{
			m_LogError = LogErrorCodes_ErrorWhileClosing;
			return false;
		}
	}

	try
	{
		m_Fs.open(m_Filename, std::ios::in | std::ios::out);
	}
	catch (const std::ios_base::failure& fail)
	{
		m_LastStringError = "Error while opening file " + m_Filename + ": " + fail.what();
		m_LogError = LogErrorCodes_CantOpenFile;
		return false;
	}

	if (!m_Fs.is_open())
	{
		m_LogError = LogErrorCodes_CantOpenFile;
		return false;
	}

	if (mode == EFileAccessMode::Append)
	{
		m_Fs.seekg(0, m_Fs.end);
	}

	m_IsHeaderRead = false;
	m_IsFirstLineWritten = false;
	m_IsSetInfoCalled = false;
	m_IsSetInputType = false;
	m_HasEpoch = false;
	return true;
}

bool CCSVLib::readSamplesAndEventsFromFile(unsigned long long linesToRead, std::vector<SMatrixChunk>& samples, std::vector<SStimulationChunk>& stimulations, unsigned long long& linesRead)
{
	if (!m_IsHeaderRead)
	{
		m_Fs.seekg(0);
		std::getline(m_Fs, m_Header);
		m_IsHeaderRead = true;
	}

	if (!parseHeader()) // m_LogError set in this function
	{
		return false;
	}

	samples.clear();
	unsigned long long line = 0;
	linesRead = line;
	while (line < linesToRead)
	{
		if (!std::getline(m_Fs, m_Buffer))
		{
			linesRead = line;
			return true;
		}

		unsigned int columnCounter = 1;
		for (unsigned int index = 0; index < m_Buffer.size(); index++)
		{
			if (m_Buffer[index] == m_Separator)
			{
				columnCounter++;
			}
		}

		if (columnCounter != m_ColumnCount)
		{
			m_LastStringError = "There is " + std::to_string(columnCounter)
				+ " columns in the Header instead of " + std::to_string(m_ColumnCount)
				+ " on line " + std::to_string(line + 1);
			m_LogError = LogErrorCodes_WrongLineSize;
			return false;
		}

		std::istringstream iss(m_Buffer);
		for (unsigned int index = 0; index < (m_ColumnCount - 3); index++)
		{
			std::string column;
			std::getline(iss, column, m_Separator);
			if (column.empty())
			{
				m_LogError = LogErrorCodes_EmptyColumn;
				return false;
			}

			m_LineColumns.push_back(column);
		}
		// get Matrix chunk, LogError set in the function
		SMatrixChunk sample(0, 0, {}, 0);
		if (!readSampleChunk(sample))
		{
			return false;
		}
		// get stimulations chunk, LogError set in the function
		if (!readStimulationChunk(stimulations, line + 1))
		{
			return false;
		}
		m_LineColumns.clear();
		samples.push_back(sample);
		sample.matrix.clear();
		line++;
	}

	linesRead = line;
	return true;
}

bool CCSVLib::writeHeaderToFile(void)
{
	if (!m_Fs.is_open())
	{
		m_LogError = LogErrorCodes_NoFileDefined;
		return false;
	}

	if (m_IsFirstLineWritten)
	{
		m_LogError = LogErrorCodes_CantWriteHeader;
		return false;
	}
	// set header (in case of error, logError set in function)
	if (!createHeaderString())
	{
		return false;
	}
	m_IsFirstLineWritten = true;
	try
	{
		m_Fs.write(m_Header.c_str(), m_Header.size());
	}
	catch (std::ios_base::failure &fail)
	{
		m_LastStringError = "Error occured while writing: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileWriting;
		return false;
	}
	return true;
}

bool CCSVLib::writeDataToFile(void)
{
	if (!m_Fs.is_open())
	{
		m_LogError = LogErrorCodes_NoFileDefined;
		return false;
	}

	// set matrix (in case of error, logError set in the function)
	if (!createCSVStringFromData(false))
	{
		return false;
	}
	try
	{
		m_Fs.write(m_Buffer.c_str(), m_Buffer.size());
	}
	catch (std::ios_base::failure &fail)
	{
		m_LastStringError = "Error occured while writing: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileWriting;
		return false;
	}
	m_Buffer.clear();
	return true;
}

bool CCSVLib::writeAllDataToFile(void)
{
	// in case of error, logError set in the function
	if (!createCSVStringFromData(true))
	{
		return false;
	}

	try
	{
		m_Fs.write(m_Buffer.c_str(), m_Buffer.size());
	}
	catch (std::ios_base::failure fail)
	{
		m_LogError = LogErrorCodes_ErrorWhileWriting;
		return false;
	}
	m_Buffer.clear();
	return true;
}

bool CCSVLib::closeFile()
{
	m_Stimulations.clear();
	m_Chunks.clear();
	m_Header.clear();
	m_Buffer.clear();
	m_DimensionSizes.clear();
	m_DimensionLabels.clear();
	m_FrequencyBands.clear();
	m_DimensionCount = 0;
	m_ColumnCount = 0;
	m_SamplingRate = 0;
	m_IsSetInputType = false;
	m_IsFirstLineWritten = false;
	m_IsSetInfoCalled = false;
	try
	{
		m_Fs.close();
	}
	catch (const std::ios_base::failure& fail)
	{
		m_LastStringError = "Error while closing file: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileClosing;
		return false;
	}
	return true;
}

bool CCSVLib::addSample(const SMatrixChunk& sample)
{
	if (sample.matrix.empty())
	{
		m_LogError = LogErrorCodes_MatrixEmpty;
		return false;
	}
	else if (m_InputTypeIdentifier == EStreamType::Signal
		&& sample.matrix.size() != m_DimensionLabels.size())
	{
		m_LogError = LogErrorCodes_WrongMatrixSize;
		return false;
	}
	else if (m_InputTypeIdentifier == EStreamType::Spectrum
		&& sample.matrix.size() != (m_DimensionLabels.size() * m_FrequencyBands.size()))
	{
		m_LogError = LogErrorCodes_WrongMatrixSize;
		return false;
	}
	else if (m_InputTypeIdentifier != EStreamType::Signal
		&& m_InputTypeIdentifier != EStreamType::Spectrum)
	{
		unsigned int columnsToHave = 1;
		for (size_t columnNbrToHave : m_DimensionSizes)
		{
			columnsToHave *= columnNbrToHave;
		}

		if (sample.matrix.size() != columnsToHave)
		{
			m_LogError = LogErrorCodes_WrongMatrixSize;
			return false;
		}
	}
	for (size_t index = 0; index < sample.matrix.size(); index++)
	{
		if (std::signbit(sample.startTime)
			|| std::signbit(sample.endTime)
			|| sample.endTime < sample.startTime)
		{
			m_LogError = LogErrorCodes_WrongSampleDate;
			return false;
		}
	}
	m_Chunks.push_back(sample);
	return true;
}

bool CCSVLib::addBuffer(const std::vector<SMatrixChunk>& samples)
{
	if (samples.empty())
	{
		m_LogError = LogErrorCodes_NoSample;
		return false;
	}

	for (const SMatrixChunk& sample : samples)
	{
		if (sample.matrix.empty())
		{
			m_LogError = LogErrorCodes_MatrixEmpty;
			return false;
		}
		else if (sample.matrix.size() != m_DimensionLabels.size())
		{
			m_LogError = LogErrorCodes_WrongMatrixSize;
			return false;
		}

		for (unsigned int matrixIndex = 0; matrixIndex < sample.matrix[matrixIndex]; matrixIndex++)
		{
			if (std::signbit(sample.startTime)
				|| std::signbit(sample.endTime)
				|| sample.endTime < sample.startTime)
			{
				m_LogError = LogErrorCodes_WrongSampleDate;
				return false;
			}
		}
	}
	m_Chunks.insert(m_Chunks.end(), samples.begin(), samples.end());
	return true;
}

bool CCSVLib::addEvent(unsigned long long code, double date, double duration)
{
	if (std::signbit(date))
	{
		m_LogError = LogErrorCodes_DateError;
		return false;
	}

	if (std::signbit(duration))
	{
		m_LogError = LogErrorCodes_DurationError;
		return false;
	}

	m_Stimulations.emplace_back(code, date, duration);
	return true;
}

bool CCSVLib::addEvent(const SStimulationChunk& stimulation)
{
	if (std::signbit(stimulation.stimulationDate))
	{
		m_LogError = LogErrorCodes_DateError;
		return false;
	}

	if (std::signbit(stimulation.stimulationDuration))
	{
		m_LogError = LogErrorCodes_DurationError;
		return false;
	}
	m_Stimulations.push_back(stimulation);
	return true;
}

bool CCSVLib::noEventsUntilDate(double date)
{
	if (std::signbit(date))
	{
		m_LogError = LogErrorCodes_WrongSampleDate;
		return false;
	}

	m_NoEventSince = date;
	return true;
}

ELogErrorCodes CCSVLib::getLastLogError()
{
	return m_LogError;
}

std::string CCSVLib::getLastErrorString()
{
	return m_LastStringError;
}

std::string CCSVLib::writeStimulations(const std::vector<SStimulationChunk>& stimulationToPrint)
{
	std::string stimulations;
	for (size_t index = 0; index < stimulationToPrint.size(); index++)
	{
		stimulations += std::to_string(stimulationToPrint[index].stimulationIdentifier);
		if ((index + 1) < stimulationToPrint.size())
		{
			stimulations += m_InternalDataSeparator;
		}
	}
	stimulations += m_Separator;

	char buffer[1024];
	for (size_t index = 0; index < stimulationToPrint.size(); index++)
	{
		sprintf(buffer, "%.10f", stimulationToPrint[index].stimulationDate);
		stimulations += buffer;
		if ((index + 1) < stimulationToPrint.size())
		{
			stimulations += m_InternalDataSeparator;
		}
	}
	stimulations += m_Separator;

	for (size_t index = 0; index < stimulationToPrint.size(); index++)
	{
		sprintf(buffer, "%.10f", stimulationToPrint[index].stimulationDuration);
		stimulations += buffer;
		if ((index + 1) < stimulationToPrint.size())
		{
			stimulations += m_InternalDataSeparator;
		}
	}
	return stimulations;
}

bool CCSVLib::createHeaderString(void)
{
	// add Time Header
	m_Header = "Time";
	if (m_InputTypeIdentifier == EStreamType::Signal)
	{
		m_Header += m_InternalDataSeparator + std::to_string(m_SamplingRate) + "Hz";
	}
	else if (m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		if (m_DimensionSizes.size() != 2)
		{
			m_LastStringError = "Channel names and number of frequency are needed to write time column";
			return false;
		}

		m_Header += m_InternalDataSeparator;
		m_Header += std::to_string(m_DimensionSizes[0]);
		m_Header += "x";
		m_Header += std::to_string(m_DimensionSizes[1]);
	}
	else if (m_InputTypeIdentifier == EStreamType::StreamedMatrix)
	{
		m_Header += m_InternalDataSeparator;
		if (m_DimensionCount == 0)
		{
			m_LogError = LogErrorCodes_DimensionCountZero;
			return false;
		}

		for (unsigned int index = 0; index < m_DimensionCount; index++)
		{
			m_Header += std::to_string(m_DimensionSizes[index]);
			if ((index + 1) < m_DimensionCount)
			{
				m_Header += "x";
			}
		}
	}
	m_Header += m_Separator;
	m_ColumnCount++;

	// add Epoch Header or not
	if (m_InputTypeIdentifier == EStreamType::Signal
		|| m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		m_Header += "Epoch";
		m_Header += m_Separator;
		m_ColumnCount++;
	}
	// add matrix columns names

	if (m_DimensionLabels.empty())
	{
		m_LogError = LogErrorCodes_NoMatrixLabels;
		return false;
	}

	if (m_InputTypeIdentifier == EStreamType::Signal
		|| m_InputTypeIdentifier == EStreamType::CovarianceMatrix
		|| m_InputTypeIdentifier == EStreamType::FeatureVector)
	{
		for (size_t labelIndex = 0; labelIndex < m_DimensionLabels.size(); labelIndex++)
		{
			m_Header += m_DimensionLabels[labelIndex] + m_Separator;
			m_ColumnCount++;
		}
	}
	else if (m_InputTypeIdentifier == EStreamType::StreamedMatrix)
	{
		for (unsigned int size : m_DimensionSizes)
		{
			if (size == 0)
			{
				m_LogError = LogErrorCodes_DimensionSizeZero;
				return false;
			}
		}
		std::vector<unsigned int> position(m_DimensionCount, 0);

		while (updateIteratedPosition(position))
		{
			for (size_t index = 0; index < position.size(); index++)
			{
				m_Header += m_DimensionLabels[(index * m_DimensionSizes[index]) + position[index]];
				if ((index + 1) < position.size())
				{
					m_Header += m_InternalDataSeparator;
				}
			}
			m_Header += m_Separator;
			position.back()++;
			m_ColumnCount++;
		}
	}
	else if (m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		for (size_t channel = 0; channel < m_DimensionLabels.size(); channel++)
		{
			for (unsigned int column = 0; column < m_FrequencyBands.size(); column++)
			{
				m_Header += m_DimensionLabels[channel] + m_InternalDataSeparator + std::to_string(m_FrequencyBands[column].front()) + m_Separator;
				m_ColumnCount++;
			}
		}
	}
	m_Header += "Event Id";
	m_Header += m_Separator;
	m_Header += "Event Date";
	m_Header += m_Separator;
	m_Header += "Event Duration\n";
	m_ColumnCount += 3;
	return true;
}

bool CCSVLib::createCSVStringFromData(bool canWriteAll)
{
	std::vector<SStimulationChunk> stimulationsToWrite;
	// if chunks is empty, their is no more lines to write
	if (m_Chunks.empty())
	{
		return true;
	}
	// if header isn't written, size of first line to write will be the reference
	if (m_ColumnCount == 0)
	{
		m_ColumnCount = m_Chunks[0].matrix.size();
		if (m_InputTypeIdentifier == EStreamType::Signal
			|| m_InputTypeIdentifier == EStreamType::Spectrum)
		{
			m_ColumnCount += 5;
		}
		else
		{
			m_ColumnCount += 4;
		}
	}
	// loop will add a line to the buffer while the last stimulation date registered is greater than the end of the current chunk or until their is an event
	unsigned long long linesWritten = 0;
	while (!m_Chunks.empty()
		&& ((m_Stimulations.empty() && m_Chunks.front().endTime <= m_NoEventSince)
		|| (!m_Stimulations.empty() && m_Chunks.front().endTime <= m_Stimulations.back().stimulationDate)))
	{
		// Signal data must be written as sampleCounterPerBuffer th lines;
		if (m_InputTypeIdentifier == EStreamType::Signal
			&& canWriteAll == false
			&& linesWritten != 0
			&& linesWritten % m_SampleCountPerBuffer == 0)
		{
			break;
		}
		// check line size

		if ((m_InputTypeIdentifier == EStreamType::Signal || m_InputTypeIdentifier == EStreamType::Spectrum)
			&& (m_Chunks.front().matrix.size() + 5) != m_ColumnCount)
		{
			m_LogError = LogErrorCodes_WrongLineSize;
			return false;
		}

		if (m_InputTypeIdentifier == EStreamType::FeatureVector
			|| m_InputTypeIdentifier == EStreamType::CovarianceMatrix
			|| m_InputTypeIdentifier == EStreamType::StreamedMatrix)
		{
			unsigned int columnstoHave = 1;
			for (unsigned int size : m_DimensionSizes)
			{
				columnstoHave *= size;
			}

			if (columnstoHave + 4 != m_ColumnCount)
			{
				m_LogError = LogErrorCodes_WrongLineSize;
				return false;
			}
		}
		// time and epoch

		std::pair<double, double> currentTime = { m_Chunks.front().startTime, m_Chunks.front().endTime };
		char buffer[1024] = {};

		sprintf(buffer, "%.10f", currentTime.first);
		m_Buffer += buffer;
		if (m_InputTypeIdentifier == EStreamType::Spectrum
			|| m_InputTypeIdentifier == EStreamType::StreamedMatrix
			|| m_InputTypeIdentifier == EStreamType::CovarianceMatrix
			|| m_InputTypeIdentifier == EStreamType::FeatureVector)
		{
			sprintf(buffer, "%.10f", m_Chunks.front().endTime);
			m_Buffer += m_InternalDataSeparator;
			m_Buffer += buffer;
		}

		m_Buffer += m_Separator;
		if (m_InputTypeIdentifier == EStreamType::Signal
			|| m_InputTypeIdentifier == EStreamType::Spectrum)
		{
			m_Buffer += std::to_string(m_Chunks.front().epoch) + m_Separator;
		}

		// matrix
		while (!m_Chunks.front().matrix.empty())
		{
			sprintf(buffer, "%.10f", m_Chunks.front().matrix.front());
			m_Buffer += buffer;
			m_Buffer += m_Separator;
			m_Chunks.front().matrix.erase(m_Chunks.front().matrix.begin());
		}

		// stimulations
		if (!m_Stimulations.empty())
		{
			double stimulationTime = m_Stimulations[0].stimulationDate;
			while ((stimulationTime - currentTime.first) < (currentTime.second - currentTime.first))
			{
				stimulationsToWrite.push_back(m_Stimulations[0]);
				m_Stimulations.erase(m_Stimulations.begin());
				if (m_Stimulations.empty())
				{
					break;
				}
				stimulationTime = m_Stimulations[0].stimulationDate;
			}
			m_Buffer += writeStimulations(stimulationsToWrite);
			stimulationsToWrite.clear();
		}
		else
		{
			m_Buffer.push_back(m_Separator);
			m_Buffer.push_back(m_Separator);
		}
		m_Buffer += "\n";
		linesWritten++;
		m_Chunks.erase(m_Chunks.begin());
	}
	return true;
}

bool CCSVLib::parseHeader(void)
{
	::split(m_Header, m_Separator, m_LineColumns);
	if (m_LineColumns.empty())
	{
		m_LastStringError = "No header in the file or file empty";
		m_LogError = LogErrorCodes_EmptyColumn;
		return false;
	}

	for (std::string columnPart : m_LineColumns)
	{
		if (columnPart.empty())
		{
			m_LogError = LogErrorCodes_EmptyColumn;
			return false;
		}
	}

	if (m_InputTypeIdentifier != EStreamType::Signal
		&& m_InputTypeIdentifier != EStreamType::Spectrum
		&& m_InputTypeIdentifier != EStreamType::StreamedMatrix
		&& m_InputTypeIdentifier != EStreamType::CovarianceMatrix
		&& m_InputTypeIdentifier != EStreamType::FeatureVector)
	{
		m_LastStringError = "Cannot read header without input type";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	switch (m_InputTypeIdentifier)
	{
		case EStreamType::Signal:
			if (!parseSignalHeader())
			{
				m_LogError = LogErrorCodes_WrongHeader;
				return false;
			}
			break;
		case EStreamType::Spectrum:
			if (!parseSpectrumHeader())
			{
				m_LogError = LogErrorCodes_WrongHeader;
				return false;
			}
			break;
		case EStreamType::StreamedMatrix:
		case EStreamType::CovarianceMatrix:
		case EStreamType::FeatureVector:
			if (!parseMatrixHeader())
			{
				m_LogError = LogErrorCodes_WrongHeader;
				return false;
			}
			break;
	}

	// check stimulations
	if (m_LineColumns[m_LineColumns.size() - s_ColumnEndMatrixIndex] != s_EventIdentifierColumn
		|| m_LineColumns[m_LineColumns.size() - s_ColumnStartMatrixIndex] != s_EventDateColumn
		|| m_LineColumns.back() != s_EventDurationColumn)
	{
		m_LastStringError = "Three last column must be \"Even Id\", \"Event Date\" and \"Event Duration\"";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	m_ColumnCount = m_LineColumns.size();
	m_LineColumns.clear();
	return true;
}

bool CCSVLib::parseSignalHeader(void)
{
	//check time
	if (m_LineColumns[0].substr(0, 5) != "Time:"
		|| m_LineColumns[0].substr(m_LineColumns[0].size() - 2) != "Hz"
		|| m_LineColumns[0].size() <= 7)
	{
		m_LastStringError = "First column (" + m_LineColumns[0] + ") is not well formed";
		return false;
	}

	// get sampling rate
	try
	{
		m_SamplingRate = stoul(m_LineColumns[0].substr(5, m_LineColumns[0].size() - 3));
	}
	catch (const std::exception& ia)
	{
		m_LastStringError = "On entry \"" + m_LineColumns[0].substr(5, m_LineColumns[0].size() - 3) + "\", exception have been thrown: ";
		m_LastStringError += ia.what();
		return false;
	}

	// check epoch
	if (m_LineColumns[1] != "Epoch")
	{
		m_LastStringError = "Second column (" + m_LineColumns[1] + ") must be Epoch column";
		return false;
	}

	// get labels
	for (unsigned int index = 2; index < (m_LineColumns.size() - s_ColumnEndMatrixIndex); index++)
	{
		m_DimensionLabels.push_back(m_LineColumns[index]);
	}
	return true;
}

bool CCSVLib::parseSpectrumHeader(void)
{
	m_DimensionSizes.clear();
	// check time column
	std::string buffer;
	std::istringstream iss(m_LineColumns[0]);
	if (!std::getline(iss, buffer, m_InternalDataSeparator))
	{
		m_LastStringError = "First column (" + m_LineColumns[0] + ") is empty";
		return false;
	}

	if (buffer != "Time")
	{
		m_LastStringError = "First column (" + m_LineColumns[0] + ") is not well formed, must have \"Time\" written before the first internal data separator (\':\' as default)";
		return false;
	}

	if (!std::getline(iss, buffer, 'x'))
	{
		m_LastStringError = "First column (" + m_LineColumns[0] + ") is not well formed, missing channels number";
		return false;
	}
	unsigned int dimensionSize = 0;
	try
	{
		dimensionSize = std::stoul(buffer);
	}
	catch (std::exception& e)
	{
		m_LastStringError = "On entry \"" + buffer + "\", exception have been thrown: ";
		m_LastStringError += e.what();
		return false;
	}

	m_DimensionSizes.push_back(dimensionSize);
	if (!std::getline(iss, buffer, 'x'))
	{
		m_LastStringError = "First column (" + m_LineColumns[0] + ") is not well formed, missing number frequencies per channel";
		return false;
	}

	try
	{
		dimensionSize = std::stoul(buffer);
	}
	catch (std::exception& e)
	{
		m_LastStringError = "On entry \"" + buffer + "\", exception have been thrown: ";
		m_LastStringError += e.what();
		return false;
	}

	m_DimensionSizes.push_back(dimensionSize);
	if (m_DimensionSizes[0] == 0
		|| m_DimensionSizes[1] == 0)
	{
		m_LastStringError = "Dimension size must be only positive";
		return false;
	}
	//check Epoch column
	if (m_LineColumns[1] != "Epoch")
	{
		m_LastStringError = "Second column (" + m_LineColumns[1] + ") must be Epoch column";
		return false;
	}

	// add every new label

	std::string channelLabel;
	m_DimensionLabels.clear();
	for (size_t labelCounter = 0; labelCounter < m_DimensionSizes[0]; labelCounter++)
	{
		double lastFrequency = 0.0;
		for (size_t labelSizeCounter = 0; labelSizeCounter < m_DimensionSizes[1]; labelSizeCounter++)
		{
			std::string dimensionData;
			// get channel name and check that it's the same one for all the frequency dimension size
			std::istringstream iss(m_LineColumns.at((labelCounter * m_DimensionSizes[1]) + labelSizeCounter + s_ColumnStartMatrixIndex));
			if (!std::getline(iss, dimensionData, m_InternalDataSeparator))
			{
				m_LastStringError = "Line don't have enough parameters";
				return false;
			}

			if (labelSizeCounter == 0)
			{
				channelLabel = dimensionData;
				m_DimensionLabels.push_back(channelLabel);
			}
			else if (channelLabel != dimensionData)
			{
				m_LastStringError = "Channel name must be the same during " + std::to_string(m_DimensionSizes[1]) + "columns (number of frequencies per channel)";
				return false;
			}

			// get all frequency and check that they're the same for all labels
			if (!std::getline(iss, dimensionData, m_InternalDataSeparator))
			{
				m_LastStringError = "Line don't have enough parameters : must have channel name and frequency";
				return false;
			}

			if (labelCounter == 0)
			{
				double frequency;
				try
				{
					frequency = std::stod(dimensionData);
				}
				catch (std::exception& e)
				{
					m_LastStringError = "On entry \"" + dimensionData + "\", exception have been thrown: ";
					m_LastStringError += e.what();
					return false;
				}

				m_FrequenceBandsBuffer.push_back(frequency);
			}
			else
			{
				double frequency;
				try
				{
					frequency = std::stod(dimensionData);
				}
				catch (std::exception& e)
				{
					m_LastStringError = "On entry \"" + dimensionData + "\", exception have been thrown: ";
					m_LastStringError += e.what();
					return false;
				}

				if (labelSizeCounter == 0)
				{
					lastFrequency = frequency;
				}
				else if (frequency < lastFrequency)
				{
					m_LastStringError = "Frequencies must be in ascending order";
					return false;
				}

				if (frequency != m_FrequenceBandsBuffer[labelSizeCounter])
				{
					m_LastStringError = "Channels must have the same frequency bands";
					return false;
				}
			}
			// check that their is only label and frequency on the column
			if (std::getline(iss, dimensionData, m_InternalDataSeparator))
			{
				m_LastStringError = "Error, too much data on the column " + std::to_string((labelCounter * m_DimensionSizes[1]) + labelSizeCounter + s_ColumnStartMatrixIndex + 1);
				return false;
			}
		}
	}
	return true;
}

bool CCSVLib::parseMatrixHeader(void)
{
	// check time column
	std::istringstream iss(m_LineColumns[0]);
	std::string linePart;
	// check Time is written
	if (!std::getline(iss, linePart, m_InternalDataSeparator))
	{
		m_LastStringError = "First column is empty";
		return false;
	}
	else if (linePart != "Time")
	{
		m_LastStringError = "First column " + m_LineColumns[0] + " is not well formed";
		return false;
	}

	// check if at least one dimension
	if (!std::getline(iss, linePart, m_InternalDataSeparator)
		|| linePart == "")
	{
		m_LastStringError = "First column must indicate at least one dimension size";
		return false;
	}

	std::istringstream dimensionPart(linePart);
	std::string dimensionSize;
	unsigned int size = 0;
	m_DimensionSizes.clear();
	m_DimensionCount = 0;
	while (std::getline(dimensionPart, dimensionSize, 'x'))
	{
		try
		{
			size = stoul(dimensionSize);
		}
		catch (std::exception& e)
		{
			m_LastStringError = "Error on a dimension size, exception have been thrown: ";
			m_LastStringError += e.what();
			return false;
		}

		if (size == 0)
		{
			m_LastStringError = "A dimension size must be positive";
			return false;
		}

		m_DimensionSizes.push_back(size);
		m_DimensionCount++;
	}

	// check columnLabels number according to dimension sizes
	unsigned int matrixColumnCount = 1;
	for (const unsigned int &columnNbr : m_DimensionSizes)
	{
		matrixColumnCount *= columnNbr;
	}

	if ((matrixColumnCount + 4) != m_LineColumns.size())
	{
		m_LastStringError = "Every line must have " + std::to_string(matrixColumnCount + 4) + " columnLabels";
		return false;
	}

	m_ColumnCount = matrixColumnCount;
	//get matrix labels
	std::vector<std::vector<std::string>> labelsInDimensions(m_DimensionCount);
	std::vector<std::vector<bool>> filledLabel(m_DimensionCount);

	for (size_t index = 0; index < m_DimensionSizes.size(); index++)
	{
		labelsInDimensions[index].resize(m_DimensionSizes[index]);
		filledLabel[index].resize(m_DimensionSizes[index], false);
	}
	std::vector<unsigned int> positionInDimensions(m_DimensionCount, 0);

	for (size_t columnIndex = 0; columnIndex < m_ColumnCount; columnIndex++)
	{
		std::vector<std::string> columnLabels;
		// get all column part
		::split(m_LineColumns[columnIndex + 1], m_InternalDataSeparator, columnLabels);
		if (columnLabels.size() != m_DimensionCount)
		{
			m_LastStringError = "Label names must be " + std::to_string(m_DimensionCount) + " per column(empty labels autorized)";
			m_LogError = LogErrorCodes_WrongHeader;
			return false;
		}
		// check column labels

		for (long index = positionInDimensions.size() - 1; index >= 0; index--)
		{
			std::vector<std::string> currentDimension = labelsInDimensions[index];
			unsigned int currentPosition = positionInDimensions[index];
			// two type of label, empty label or not
			if (columnLabels[index].empty())
			{
				// check if label is the good one
				if (!currentDimension[currentPosition].empty())
				{
					m_LastStringError = "Error at column " + std::to_string(columnIndex)
						+ " for the label \"" + columnLabels[index]
						+ "\" in dimension number " + std::to_string(index) +
						" is trying to reset label that have been already set";
					m_LogError = LogErrorCodes_WrongHeader;
					return false;
				}
				else
				{
					filledLabel[index][currentPosition] = true;
				}
			}
			else
			{
				// position no fill, fill it with new label
				if (columnLabels[index] != currentDimension[currentPosition]
					&& !filledLabel[index][currentPosition])
				{
					currentDimension[currentPosition] = columnLabels[index];
					filledLabel[index][currentPosition] = true;
				}
				// error if position already fill
				else if (columnLabels[index] != currentDimension[currentPosition]
					&& filledLabel[index][currentPosition])
				{
					m_LastStringError = "Error at column " + std::to_string(columnIndex)
						+ " for the label \"" + columnLabels[index]
						+ "\" in dimension number " + std::to_string(index) +
						" that is trying to reset label that have been already set";
					m_LogError = LogErrorCodes_WrongHeader;
					return false;
				}
			}
		}
		positionInDimensions.back()++;
		if (!updateIteratedPosition(positionInDimensions))
		{
			break;
		}
	}

	for (const std::vector<std::string>& dimensionIndex : labelsInDimensions)
	{
		for (const std::string& label : dimensionIndex)
		{
			m_DimensionLabels.push_back(label);
		}
	}
	return true;
}

bool CCSVLib::readSampleChunk(SMatrixChunk& sample)
{
	std::istringstream ssTime(m_LineColumns[0]);
	std::string column;
	std::getline(ssTime, column, m_InternalDataSeparator);
	double startTime;
	try
	{
		startTime = std::stod(column);
	}
	catch (const std::invalid_argument& ia)
	{
		m_LastStringError = "Invalid argument error on start time: ";
		m_LastStringError += ia.what();
		m_LogError = LogErrorCodes_InvalidArgumentException;
		return false;
	}
	catch (const std::out_of_range& oor)
	{
		m_LastStringError = "Out of range error: ";
		m_LastStringError += oor.what();
		m_LogError = LogErrorCodes_OutOfRangeException;
		return false;
	}

	std::getline(ssTime, column, m_InternalDataSeparator);
	double endTime;
	try
	{
		endTime = std::stod(column);
	}
	catch (const std::invalid_argument& ia)
	{
		m_LastStringError = "Invalid argument error on end time: ";
		m_LastStringError += ia.what();
		m_LogError = LogErrorCodes_InvalidArgumentException;
		return false;
	}
	catch (const std::out_of_range& oor)
	{
		m_LastStringError = "Out of range error: ";
		m_LastStringError += oor.what();
		m_LogError = LogErrorCodes_OutOfRangeException;
		return false;
	}

	std::istringstream ssEpoch(m_LineColumns[1]);
	std::getline(ssEpoch, column, '\0');
	std::vector<double> matrix;
	unsigned long long epoch;

	if (m_InputTypeIdentifier == EStreamType::Signal
		|| m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		try
		{
			epoch = std::stoull(column.c_str());
		}
		catch (const std::invalid_argument& ia)
		{
			m_LastStringError = "Invalid argument error on epoch column: ";
			m_LastStringError += ia.what();
			m_LogError = LogErrorCodes_InvalidArgumentException;
			return false;
		}
		catch (const std::out_of_range& oor)
		{
			m_LastStringError = "Out of range error: ";
			m_LastStringError += oor.what();
			m_LogError = LogErrorCodes_OutOfRangeException;
			return false;
		}
	}
	else
	{
		epoch = std::numeric_limits<unsigned long long>::max();
	}

	if (m_InputTypeIdentifier == EStreamType::Signal
		|| m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		for (unsigned int index = s_ColumnStartMatrixIndex; index < (m_LineColumns.size() - s_ColumnEndMatrixIndex); index++)
		{
			if (!m_LineColumns[index].empty())
			{
				double matrixValue;
				try
				{
					matrixValue = std::stod(m_LineColumns[index]);
				}
				catch (const std::invalid_argument& ia)
				{
					m_LastStringError = "Invalid argument error on matrix value on column " + std::to_string(index) + " : " + ia.what();
					m_LogError = LogErrorCodes_InvalidArgumentException;
					return false;
				}
				catch (const std::out_of_range& oor)
				{
					m_LastStringError = "Out of range error: ";
					m_LastStringError += oor.what();
					m_LogError = LogErrorCodes_OutOfRangeException;
					return false;
				}
				matrix.push_back(matrixValue);
			}
		}
	}
	else
	{
		for (unsigned int index = 1; index < (m_LineColumns.size() - s_ColumnEndMatrixIndex); index++)
		{
			double matrixValue;

			try
			{
				matrixValue = std::stod(m_LineColumns[index]);
			}
			catch (const std::invalid_argument& ia)
			{
				m_LastStringError = "Invalid argument error on matrix value on column " + std::to_string(index) + " : " + ia.what();
				m_LogError = LogErrorCodes_InvalidArgumentException;
				return false;
			}
			catch (const std::out_of_range& oor)
			{
				m_LastStringError = "Out of range error: ";
				m_LastStringError += oor.what();
				m_LogError = LogErrorCodes_OutOfRangeException;
				return false;
			}
			matrix.push_back(matrixValue);
		}
	}

	sample = { startTime, endTime, matrix, epoch };
	return true;
}

bool CCSVLib::readStimulationChunk(std::vector<SStimulationChunk>& stimulations, unsigned long long line)
{
	std::istringstream ssStimeId(m_LineColumns[m_LineColumns.size() - s_ColumnEndMatrixIndex]);
	std::vector<unsigned long long> stimIdentifiers;

	std::string column;

	// pick all time identifiers for the actual time
	while (std::getline(ssStimeId, column, m_InternalDataSeparator))
	{
		unsigned long long id;
		try
		{
			id = std::stoull(column);
		}
		catch (const std::exception& e)
		{
			m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size() - s_StimulationIdentifierColumnNbr) + " : " + e.what();
			m_LogError = LogErrorCodes_InvalidStimulationArgument;
			return false;
		}

		stimIdentifiers.push_back(id);
	}

	// pick all time dates for the actual time

	std::istringstream ssStimeDate(m_LineColumns[m_LineColumns.size() - s_StimulationDateColumn]);
	std::vector<double> stimDates;
	while (std::getline(ssStimeDate, column, m_InternalDataSeparator))
	{
		double date;
		try
		{
			date = std::stof(column);
		}
		catch (const std::exception& e)
		{
			m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size() - s_StimulationDateColumnNbr) + " : " + e.what();
			m_LogError = LogErrorCodes_InvalidStimulationArgument;
			return false;
		}

		if (std::signbit(date))
		{
			m_LogError = LogErrorCodes_NegativeStimulation;
			return false;
		}
		stimDates.push_back(date);
	}

	// pick all time durations for the actual time

	std::istringstream ssStimeDuration(m_LineColumns.back());
	std::vector<double> stimDurations;

	while (std::getline(ssStimeDuration, column, m_InternalDataSeparator))
	{
		double duration;
		try
		{
			duration = std::stof(column);
		}
		catch (const std::exception& e)
		{
			m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size()) + " : " + e.what();
			m_LogError = LogErrorCodes_InvalidStimulationArgument;
			return false;
		}

		if (std::signbit(duration))
		{
			m_LogError = LogErrorCodes_NegativeStimulation;
			return false;
		}
		stimDurations.push_back(duration);
	}

	if (stimIdentifiers.size() != stimDates.size()
		|| stimIdentifiers.size() != stimDurations.size())
	{
		m_LogError = LogErrorCodes_StimulationSize;
		return false;
	}

	for (size_t index = 0; index < stimIdentifiers.size(); index++)
	{
		stimulations.push_back({ stimIdentifiers[index], stimDates[index], stimDurations[index] });
	}
	return true;
}

bool CCSVLib::updateIteratedPosition(std::vector<unsigned int>& position)
{
	for (long index = position.size() - 1; index >= 0; index--)
	{
		if ((position[index] + 1) > m_DimensionSizes[index])
		{
			if (index != 0)
			{
				position[index] = 0;
				position[index - 1]++;
			}
			else
			{
				return false;
			}
		}
	}
	return true;
}

CSV_API ICSVLib* OpenViBE::CSV::createCSVLib()
{
	return new CCSVLib();
}

CSV_API void OpenViBE::CSV::releaseCSVLib(ICSVLib* object)
{
	delete dynamic_cast<CCSVLib*>(object);
}
