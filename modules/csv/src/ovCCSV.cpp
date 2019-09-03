/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * OpenViBE SDK
 * Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
 * Copyright (C) Inria, 2015-2017,V1.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#include <cmath>
#include <limits>
#endif

#if defined(_MSC_VER) && _MSC_VER < 1900
#define snprintf _snprintf
#else
#include <stdio.h> //sprintf
#endif

#include <cstdio>
#include <sstream>
#include <numeric>
#include <algorithm>

#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/algorithm/string/join.hpp>

#include "ovCCSV.hpp"

#include <fs/Files.h>

using namespace OpenViBE;
using namespace CSV;

namespace
{
	//const uint32_t s_StimulationDateColumnNbr       = 1;
	//const uint32_t s_StimulationIdentifierColumnNbr = 2;
	const uint32_t s_SignalEpochColumnIndex = 1;
	const uint32_t s_TimeColumnIndex        = 0;
	const uint32_t s_EndTimeColumnIndex     = 1;
	// Number of columns before data (Time/Epoch)
	const uint32_t s_PreDataColumnCount = 2;
	// Number of columns after data (Events)
	const uint32_t s_PostDataColumnCount = 3;

	//Separators
	const char s_Separator(',');
	const char s_InternalDataSeparator(':');
	const char s_DimensionSeparator('x');

	// Columns Names
	const std::string s_EventIdentifierColumn = "Event Id";
	const std::string s_EventDateColumn       = "Event Date";
	const std::string s_EventDurationColumn   = "Event Duration";

	const uint32_t s_CharToRead          = 1000;
	const uint32_t s_MaximumFloatDecimal = 32;

	const char s_EndOfLineChar('\n');
}

bool CCSVHandler::streamReader(std::istream& inputStream, std::string& outputString, const char delimiter, std::string& bufferHistory) const
{
	// To improve the performance of the reading, we read a bunch a chars rather one char by one char.
	// We keep the part of the readed stream, that is not used, in a buffer history.
	std::vector<std::string> buffer;
	size_t lineBreakPos = std::string::npos;

	// We check that the delimiter is present in the buffer history
	if (!bufferHistory.empty())
	{
		buffer.push_back(std::move(bufferHistory));
		lineBreakPos = buffer.back().find_first_of(delimiter);
	}

	// If it's not the case, we have to read the stream.
	while (lineBreakPos == std::string::npos)
	{
		buffer.emplace_back(s_CharToRead, '\0');		   // Construct a string that will store the characters.
		inputStream.read(&buffer.back()[0], s_CharToRead); // Read X chars by X chars
		lineBreakPos = buffer.back().find_first_of(delimiter);

		// If it's the end of the file and no delimiter has been found...
		if (inputStream.gcount() != s_CharToRead && lineBreakPos == std::string::npos) { break; }
	}

	// There is no delimitor in the stream ...
	if (lineBreakPos == std::string::npos)
	{
		// Save the rest of the data in the history
		for_each(buffer.begin(), buffer.end(), [&bufferHistory](const std::string& s) { bufferHistory += s; });
		return false;
	}

	// Keep the string part that following the delimiter in the buffer history for the next call.
	bufferHistory = buffer.back().substr(lineBreakPos + 1);

	// And we remove this part from the last string because we will join them.
	buffer.back().erase(lineBreakPos, buffer.back().size());

	outputString.clear();
	outputString.reserve(std::accumulate(buffer.cbegin(), buffer.cend(), size_t(0),
										 [](size_t sumSize, const std::string& str) { return sumSize + str.size(); }));

	// Let's join the strings !
	for_each(buffer.begin(), buffer.end(), [&outputString](const std::string& s) { outputString += s; });

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	// Check if we are looking for an end of line char and handle the CR/LF Windows/Linux cases.
	if (delimiter == s_EndOfLineChar && m_IsCRLFEOL && !outputString.empty())
	{
		// Remove the carriage return char.
		outputString.pop_back();
	}
#endif

	return true;
}

CCSVHandler::CCSVHandler() : m_InputTypeIdentifier(EStreamType::StreamedMatrix), m_DimensionSizes({}) {}

CCSVHandler::~CCSVHandler() { this->closeFile(); }

void CCSVHandler::split(const std::string& input, char delimiter, std::vector<std::string>& output) const
{
	std::stringstream stringStream(input);
	std::string item;
	output.reserve(m_ColumnCount);

	std::string buffer;

	// Loop until the last delimiter
	while (this->streamReader(stringStream, item, delimiter, buffer)) { output.push_back(item); }

	// Get the part after the last delimiter
	if (this->streamReader(stringStream, item, '\0', buffer)) { output.push_back(item); }
}

void CCSVHandler::setFormatType(EStreamType typeID)
{
	m_InputTypeIdentifier = typeID;
	m_HasInputType        = true;
}

EStreamType CCSVHandler::getFormatType() { return m_InputTypeIdentifier; }

bool CCSVHandler::setSignalInformation(const std::vector<std::string>& channelNames, uint32_t samplingFrequency, uint32_t sampleCountPerBuffer)
{
	if (m_InputTypeIdentifier != EStreamType::Signal)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	if (m_IsSetInfoCalled)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}

	m_IsSetInfoCalled = true;

	if (channelNames.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_NoChannelsName;
		return false;
	}

	m_SamplingRate         = samplingFrequency;
	m_DimensionLabels      = channelNames;
	m_SampleCountPerBuffer = sampleCountPerBuffer;
	return true;
}

bool CCSVHandler::getSignalInformation(std::vector<std::string>& channelNames, uint32_t& samplingFrequency, uint32_t& sampleCountPerBuffer)
{
	if (m_InputTypeIdentifier != EStreamType::Signal)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (!this->parseHeader()) { return false; }

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_SamplingRate == 0)
	{
		m_LastStringError = "Sampling rate invalid";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_SampleCountPerBuffer == 0)
	{
		m_LastStringError = "Sample count per buffer is invalid";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames         = m_DimensionLabels;
	samplingFrequency    = m_SamplingRate;
	sampleCountPerBuffer = m_SampleCountPerBuffer;
	return true;
}

bool CCSVHandler::setSpectrumInformation(const std::vector<std::string>& channelNames, const std::vector<double>& frequencyAbscissa,
										 const uint32_t samplingRate)
{
	if (m_InputTypeIdentifier != EStreamType::Spectrum)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (m_IsSetInfoCalled)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}

	m_IsSetInfoCalled = true;

	if (channelNames.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_NoChannelsName;
		return false;
	}

	m_DimensionLabels      = channelNames;
	m_DimensionSizes       = { uint32_t(channelNames.size()), uint32_t(frequencyAbscissa.size()) };
	m_FrequencyAbscissa    = frequencyAbscissa;
	m_OriginalSampleNumber = samplingRate;
	return true;
}

bool CCSVHandler::getSpectrumInformation(std::vector<std::string>& channelNames, std::vector<double>& frequencyAbscissa, uint32_t& samplingRate)
{
	if (m_InputTypeIdentifier != EStreamType::Spectrum)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (!this->parseHeader()) { return false; }

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_FrequencyAbscissa.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_SampleCountPerBuffer == 0)
	{
		m_LastStringError = "Sample count per buffer is invalid";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames      = m_DimensionLabels;
	frequencyAbscissa = m_FrequencyAbscissa;
	samplingRate      = m_OriginalSampleNumber;
	return true;
}

bool CCSVHandler::setFeatureVectorInformation(const std::vector<std::string>& channelNames)
{
	if (m_InputTypeIdentifier != EStreamType::FeatureVector)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	if (m_IsSetInfoCalled)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}

	m_IsSetInfoCalled = true;

	if (channelNames.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_NoChannelsName;
		return false;
	}

	m_DimensionLabels = channelNames;
	m_DimensionSizes  = { uint32_t(channelNames.size()) };
	m_DimensionCount  = 1;
	return true;
}

bool CCSVHandler::getFeatureVectorInformation(std::vector<std::string>& channelNames)
{
	if (m_InputTypeIdentifier != EStreamType::FeatureVector)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (!this->parseHeader()) { return false; }

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	return true;
}

bool CCSVHandler::setStreamedMatrixInformation(const std::vector<uint32_t>& dimensionSizes, const std::vector<std::string>& labels)
{
	if (m_IsSetInfoCalled)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_SetInfoOnce;
		return false;
	}

	if (m_InputTypeIdentifier != EStreamType::StreamedMatrix && m_InputTypeIdentifier != EStreamType::CovarianceMatrix)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (dimensionSizes.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_DimensionSizeEmpty;
		return false;
	}

	const size_t size = std::accumulate(dimensionSizes.begin(), dimensionSizes.end(), 0);

	if (size != labels.size())
	{
		m_LastStringError = "dimension count is " + std::to_string(size) + " and labels count is " + std::to_string(labels.size());
		m_LogError        = LogErrorCodes_WrongDimensionSize;
		return false;
	}

	m_IsSetInfoCalled = true;
	m_DimensionSizes  = dimensionSizes;
	m_DimensionCount  = uint32_t(m_DimensionSizes.size());
	m_DimensionLabels = labels;
	return true;
}

bool CCSVHandler::getStreamedMatrixInformation(std::vector<uint32_t>& dimensionSizes, std::vector<std::string>& labels)
{
	if (m_InputTypeIdentifier != EStreamType::StreamedMatrix && m_InputTypeIdentifier != EStreamType::CovarianceMatrix)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (!this->parseHeader()) { return false; }

	if (m_DimensionSizes.empty())
	{
		m_LastStringError = "No dimension sizes, header reading may have failed";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	dimensionSizes = m_DimensionSizes;
	labels         = m_DimensionLabels;
	return true;
}

bool CCSVHandler::openFile(const std::string& fileName, EFileAccessMode mode)
{
	if (m_Fs.is_open())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_CantOpenFile;
		return false;
	}

	m_Filename = fileName;
	// This check that file can be written and create it if it does not exist
	if (mode == EFileAccessMode::Write || mode == EFileAccessMode::Append)
	{
		FILE* file = FS::Files::open(m_Filename.c_str(), "a");

		if (!file)
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_CantOpenFile;
			return false;
		}

		if (fclose(file) != 0)
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_ErrorWhileClosing;
			return false;
		}
	}

	try
	{
		switch (mode)
		{
			case EFileAccessMode::Write:
				FS::Files::openFStream(m_Fs, m_Filename.c_str(), std::ios::out | std::ios::trunc);
				break;

			case EFileAccessMode::Append:
				FS::Files::openFStream(m_Fs, m_Filename.c_str(), std::ios::out | std::ios::app);
				break;

			case EFileAccessMode::Read:
				FS::Files::openFStream(m_Fs, m_Filename.c_str(), std::ios::in);
				break;
		}
	}
	catch (const std::ios_base::failure& fail)
	{
		m_LastStringError = "Error while opening file " + m_Filename + ": " + fail.what();
		m_LogError        = LogErrorCodes_CantOpenFile;
		return false;
	}

	if (!m_Fs.is_open())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_CantOpenFile;
		return false;
	}

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	if (mode == EFileAccessMode::Read)
	{
		// Check if the end of line is CRLF
		std::string stringBuffer;
		std::string stringHistory;

		m_IsCRLFEOL = false;
		if(m_Fs.peek() == std::ifstream::traits_type::eof())
		{
			m_IsCRLFEOL = true;
		}
		else 
		{
			if (!this->streamReader(m_Fs, stringBuffer, '\n', stringHistory))
			{
				this->closeFile();

				m_LastStringError = "Error while opening file " + m_Filename + ": " + "Fail to determine the file line ending.";
				m_LogError = LogErrorCodes_CantOpenFile;
				return false;
			}

			m_IsCRLFEOL = *--stringBuffer.end() == '\r';
		}
	}
#endif

	// Reset the read position
	m_Fs.clear();
	m_Fs.seekg(0);

	m_IsHeaderRead       = false;
	m_IsFirstLineWritten = false;
	m_IsSetInfoCalled    = false;
	m_HasEpoch           = false;
	return true;
}

bool CCSVHandler::readSamplesAndEventsFromFile(size_t linesToRead, std::vector<SMatrixChunk>& chunks, std::vector<SStimulationChunk>& stimulations)
{
	if (!m_HasDataToRead) { return false; }

	if (!m_IsHeaderRead)
	{
		m_LastStringError = "Trying to read data without having read a header";
		m_LogError        = LogErrorCodes_HeaderNotRead;
		return false;
	}

	chunks.clear();

	// Calculate the size of the matrix depending of the stream type
	size_t matrixSize = size_t(m_SampleCountPerBuffer);

	if (m_InputTypeIdentifier == EStreamType::Signal)
	{
		const size_t signalSize = size_t(m_ColumnCount - (s_PreDataColumnCount + s_PostDataColumnCount));
		matrixSize *= signalSize;
	}
	else if (m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		const size_t spectrumSize = m_DimensionSizes[0] * m_DimensionSizes[1];
		matrixSize *= spectrumSize;
	}
	else { matrixSize = 0; }

	SMatrixChunk chunk(0, 0, std::vector<double>(matrixSize), 0);

	while (chunks.size() < linesToRead && m_HasDataToRead)
	{
		for (uint32_t lineIndex = 0; lineIndex < m_SampleCountPerBuffer; lineIndex++)
		{
			std::string lineValue;

			if (!this->streamReader(m_Fs, lineValue, s_EndOfLineChar, m_BufferReadFileLine))
			{
				if (lineIndex != 0)
				{
					m_LastStringError = "Chunk is not complete";
					m_LogError        = LogErrorCodes_MissingData;
					return false;
				}
				// There is no more data to read
				m_HasDataToRead = false;
				return true;
			}

			const int64_t columnCount = std::count(lineValue.cbegin(), lineValue.cend(), ',') + 1;

			if (columnCount != m_ColumnCount)
			{
				m_LastStringError = "There is " + std::to_string(columnCount)
									+ " columns in the Header instead of " + std::to_string(m_ColumnCount)
									+ " on line " + std::to_string(lineIndex + 1);

				m_LogError = LogErrorCodes_WrongLineSize;
				return false;
			}

			// get Matrix chunk, LogError set in the function
			if (!this->readSampleChunk(lineValue, chunk, lineIndex)) { return false; }

			// get stimulations chunk, LogError set in the function
			if (!this->readStimulationChunk(lineValue, stimulations, lineIndex + 1)) { return false; }
		}

		chunks.push_back(chunk);
	}

	return true;
}

bool CCSVHandler::writeHeaderToFile()
{
	if (!m_Fs.is_open())
	{
		m_LastStringError.clear();
		m_LastStringError = "File is not opened.";
		m_LogError        = LogErrorCodes_NoFileDefined;
		return false;
	}

	if (m_IsFirstLineWritten)
	{
		m_LastStringError = "Header already written.";
		m_LogError        = LogErrorCodes_CantWriteHeader;
		return false;
	}

	// set header (in case of error, logError set in function)
	const std::string header = this->createHeaderString();
	if (header.empty()) { return false; }

	m_IsFirstLineWritten = true;
	try { m_Fs << header; }
	catch (std::ios_base::failure& fail)
	{
		m_LastStringError = "Error occured while writing: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileWriting;
		return false;
	}

	return true;
}

bool CCSVHandler::writeDataToFile()
{
	if (!m_Fs.is_open())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_NoFileDefined;
		return false;
	}

	std::string csv;

	// set matrix (in case of error, logError set in the function)
	if (!this->createCSVStringFromData(false, csv)) { return false; }

	try { m_Fs << csv; }
	catch (std::ios_base::failure& fail)
	{
		m_LastStringError = "Error occured while writing: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileWriting;
		return false;
	}

	return true;
}

bool CCSVHandler::writeAllDataToFile()
{
	std::string csv;

	// in case of error, logError set in the function
	if (!createCSVStringFromData(true, csv)) { return false; }

	try { m_Fs << csv; }
	catch (std::ios_base::failure& fail)
	{
		m_LastStringError = "Error occured while writing: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileWriting;
		return false;
	}

	return true;
}

bool CCSVHandler::closeFile()
{
	m_Stimulations.clear();
	m_Chunks.clear();
	m_DimensionSizes.clear();
	m_DimensionLabels.clear();
	m_FrequencyAbscissa.clear();
	m_DimensionCount     = 0;
	m_ColumnCount        = 0;
	m_SamplingRate       = 0;
	m_HasInputType       = false;
	m_IsFirstLineWritten = false;
	m_IsSetInfoCalled    = false;

	try { m_Fs.close(); }
	catch (const std::ios_base::failure& fail)
	{
		m_LastStringError = "Error while closing file: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileClosing;
		return false;
	}

	return true;
}

bool CCSVHandler::addSample(const SMatrixChunk& sample)
{
	if (sample.matrix.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_MatrixEmpty;
		return false;
	}

	switch (m_InputTypeIdentifier)
	{
		case EStreamType::Signal:
			if (sample.matrix.size() != m_DimensionLabels.size())
			{
				m_LastStringError.clear();
				m_LogError = LogErrorCodes_WrongMatrixSize;
				return false;
			}
			break;

		case EStreamType::Spectrum:
			if (sample.matrix.size() != (m_DimensionLabels.size() * m_FrequencyAbscissa.size()))
			{
				m_LastStringError.clear();
				m_LogError = LogErrorCodes_WrongMatrixSize;
				return false;
			}
			break;

		default:
			const uint32_t columnsToHave = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1U, std::multiplies<uint32_t>());

			if (sample.matrix.size() != columnsToHave)
			{
				m_LastStringError = "Matrix size is " + std::to_string(sample.matrix.size()) + " and size to have is " + std::to_string(columnsToHave);
				m_LogError        = LogErrorCodes_WrongMatrixSize;
				return false;
			}
			break;
	}

	// Check timestamps
	if (std::signbit(sample.startTime) || std::signbit(sample.endTime) || sample.endTime < sample.startTime)
	{
		m_LastStringError = "Sample start time [" + std::to_string(sample.startTime) + "] | end time [" + std::to_string(sample.endTime) + "]";
		m_LogError        = LogErrorCodes_WrongSampleDate;
		return false;
	}

	if (m_LastMatrixOnly &&
		(m_InputTypeIdentifier != EStreamType::Signal
		 || (!m_Chunks.empty() && sample.epoch != m_Chunks.back().epoch)))
	{
		m_Chunks.clear();
		m_Stimulations.erase(std::remove_if(m_Stimulations.begin(), m_Stimulations.end(), [&sample](const SStimulationChunk& chunk)
							 {
								 return chunk.stimulationDate < sample.startTime;
							 }),
							 m_Stimulations.end());
	}

	m_Chunks.push_back(sample);
	return true;
}

bool CCSVHandler::addBuffer(const std::vector<SMatrixChunk>& samples)
{
	if (samples.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_NoSample;
		return false;
	}

	ELogErrorCodes error;
	const size_t dimensionCount = m_DimensionLabels.size();

	if (!std::all_of(samples.cbegin(), samples.cend(), [&error, dimensionCount](const SMatrixChunk& sample)
	{
		if (sample.matrix.empty())
		{
			error = LogErrorCodes_MatrixEmpty;
			return false;
		}

		if (sample.matrix.size() != dimensionCount)
		{
			error = LogErrorCodes_WrongMatrixSize;
			return false;
		}

		// Check that the timestamps is good
		if (std::signbit(sample.startTime) || std::signbit(sample.endTime) || sample.endTime < sample.startTime)
		{
			error = LogErrorCodes_WrongSampleDate;
			return false;
		}

		return true;
	}))
	{
		m_LogError = error;
		return false;
	}

	if (m_LastMatrixOnly)
	{
		if (m_InputTypeIdentifier == EStreamType::Signal)
		{
			const uint64_t lastEpoch = samples.back().epoch;
			const auto rangeStart    = std::find_if(samples.begin(), samples.end(),
													[&lastEpoch](const SMatrixChunk& s) -> bool { return s.epoch == lastEpoch; });

			if (!m_Chunks.empty() && m_Chunks.front().epoch != lastEpoch) { m_Chunks.clear(); }

			m_Chunks.insert(m_Chunks.end(), rangeStart, samples.end());
			const double curTime = m_Chunks.front().startTime;
			m_Stimulations.erase(std::remove_if(m_Stimulations.begin(), m_Stimulations.end(),
												[curTime](const SStimulationChunk& chunk) { return chunk.stimulationDate < curTime; }), m_Stimulations.end());
		}
		else
		{
			m_Chunks.clear();
			m_Chunks.push_back(samples.back());
			const double curTime = m_Chunks.front().startTime;
			m_Stimulations.erase(std::remove_if(m_Stimulations.begin(), m_Stimulations.end(),
												[curTime](const SStimulationChunk& chunk) { return chunk.stimulationDate < curTime; }), m_Stimulations.end());
		}
	}
	else { m_Chunks.insert(m_Chunks.end(), samples.begin(), samples.end()); }

	return true;
}

bool CCSVHandler::addEvent(uint64_t code, double date, double duration)
{
	if (std::signbit(date))
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_DateError;
		return false;
	}

	if (std::signbit(duration))
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_DurationError;
		return false;
	}

	m_Stimulations.emplace_back(code, date, duration);
	return true;
}

bool CCSVHandler::addEvent(const SStimulationChunk& event)
{
	if (std::signbit(event.stimulationDate))
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_DateError;
		return false;
	}

	if (std::signbit(event.stimulationDuration))
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_DurationError;
		return false;
	}

	m_Stimulations.push_back(event);
	return true;
}

bool CCSVHandler::noEventsUntilDate(double date)
{
	if (std::signbit(date))
	{
		m_LastStringError = "Date is negative: ";
		m_LastStringError += std::to_string(date);
		m_LogError = LogErrorCodes_WrongSampleDate;
		return false;
	}

	m_NoEventSince = date;
	return true;
}

ELogErrorCodes CCSVHandler::getLastLogError() { return m_LogError; }

std::string CCSVHandler::getLastErrorString() { return m_LastStringError; }

std::string CCSVHandler::stimulationsToString(const std::vector<SStimulationChunk>& stimulationsToPrint) const
{
	if (stimulationsToPrint.empty()) { return std::string(2, s_Separator); } // Empty columns

	std::array<std::string, 3> stimulations;

	std::vector<SStimulationChunk>::const_iterator itBegin = stimulationsToPrint.cbegin();
	std::vector<SStimulationChunk>::const_iterator itEnd   = stimulationsToPrint.cend();

	char buffer[s_MaximumFloatDecimal];

	if (itBegin != itEnd)
	{
		stimulations.at(0) = std::to_string(itBegin->stimulationIdentifier);

		buffer[0] = '\0';
		snprintf(buffer, s_MaximumFloatDecimal, "%.*f", m_OutputFloatPrecision, itBegin->stimulationDate);
		stimulations.at(1) = buffer;

		buffer[0] = '\0';
		snprintf(buffer, s_MaximumFloatDecimal, "%.*f", m_OutputFloatPrecision, itBegin->stimulationDuration);
		stimulations.at(2) = buffer;
	}

	for (std::vector<SStimulationChunk>::const_iterator it = itBegin + 1; it != itEnd; ++it)
	{
		stimulations.at(0) += std::string(1, s_InternalDataSeparator) + std::to_string(it->stimulationIdentifier);

		buffer[0] = '\0';
		snprintf(buffer, s_MaximumFloatDecimal, "%.*f", m_OutputFloatPrecision, it->stimulationDate);
		stimulations.at(1) += std::string(1, s_InternalDataSeparator) + buffer;

		buffer[0] = '\0';
		snprintf(buffer, s_MaximumFloatDecimal, "%.*f", m_OutputFloatPrecision, it->stimulationDuration);
		stimulations.at(2) += std::string(1, s_InternalDataSeparator) + buffer;
	}

	return boost::algorithm::join(stimulations, std::string(1, s_Separator));
}

std::string CCSVHandler::createHeaderString()
{
	const std::string invalidHeader = "";
	std::string header;

	auto addColumn = [&](const std::string& columnLabel)
	{
		if (m_ColumnCount != 0) { header += s_Separator; }

		header += columnLabel;
		m_ColumnCount++;
	};

	// add Time Header
	switch (m_InputTypeIdentifier)
	{
		case EStreamType::Signal:
			addColumn("Time" + std::string(1, s_InternalDataSeparator) + std::to_string(m_SamplingRate) + "Hz");
			break;

		case EStreamType::Spectrum:
			if (m_DimensionSizes.size() != 2)
			{
				m_LastStringError = "Channel names and number of frequency are needed to write time column";
				return invalidHeader;
			}

			addColumn("Time"
					  + std::string(1, s_InternalDataSeparator)
					  + std::to_string(m_DimensionSizes[0])
					  + std::string(1, s_DimensionSeparator)
					  + std::to_string(m_DimensionSizes[1])
					  + std::string(1, s_InternalDataSeparator)
					  + std::to_string(m_OriginalSampleNumber != 0 //original number of samples or calculated one	
										   ? m_OriginalSampleNumber
										   : m_DimensionSizes[0] * m_DimensionSizes[1])
			);
			break;

		case EStreamType::CovarianceMatrix:
		case EStreamType::StreamedMatrix:
		case EStreamType::FeatureVector:
			if (m_DimensionCount == 0)
			{
				m_LastStringError.clear();
				m_LogError = LogErrorCodes_DimensionCountZero;
				return invalidHeader;
			}
			{
				std::string timeColumn = "Time" + std::string(1, s_InternalDataSeparator);

				for (uint32_t index = 0; index < m_DimensionCount; index++)
				{
					timeColumn += std::to_string(m_DimensionSizes[index]);

					if ((index + 1) < m_DimensionCount) { timeColumn += std::string(1, s_DimensionSeparator); }
				}

				addColumn(timeColumn);
			}
			break;
	}

	// add Epoch Header to signal
	switch (m_InputTypeIdentifier)
	{
		case EStreamType::Signal:
			addColumn("Epoch");
			break;

		case EStreamType::Spectrum:
		case EStreamType::StreamedMatrix:
		case EStreamType::CovarianceMatrix:
		case EStreamType::FeatureVector:
			addColumn("End Time");
			break;
	}

	// add matrix columns names
	if (m_DimensionLabels.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_NoMatrixLabels;
		return invalidHeader;
	}

	switch (m_InputTypeIdentifier)
	{
		case EStreamType::Signal:
		case EStreamType::FeatureVector:
			for (const std::string& label : m_DimensionLabels) { addColumn(label); }
			break;

		case EStreamType::CovarianceMatrix:
		case EStreamType::StreamedMatrix:
		{
			const uint32_t matrixColumns = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1U, std::multiplies<uint32_t>());

			if (matrixColumns == 0)
			{
				m_LastStringError.clear();
				m_LogError = LogErrorCodes_DimensionSizeZero;
				return invalidHeader;
			}

			std::vector<uint32_t> position(m_DimensionCount, 0);
			m_ColumnCount += matrixColumns;

			do
			{
				header += s_Separator;
				size_t previousDimensionsSize = 0;

				for (size_t index = 0; index < position.size(); index++)
				{
					header += m_DimensionLabels[previousDimensionsSize + position[index]];
					previousDimensionsSize += m_DimensionSizes[index];

					if ((index + 1) < position.size()) { header += std::string(1, s_InternalDataSeparator); }
				}
			} while (this->increasePositionIndexes(position));
		}
			break;

		case EStreamType::Spectrum:
			for (const std::string& label : m_DimensionLabels)
			{
				for (double frequencyAbscissa : m_FrequencyAbscissa)
				{
					addColumn(label + std::string(1, s_InternalDataSeparator) + std::to_string(frequencyAbscissa));
				}
			}
			break;
	}

	addColumn("Event Id");
	addColumn("Event Date");
	addColumn("Event Duration\n");
	return header;
}

bool CCSVHandler::createCSVStringFromData(bool canWriteAll, std::string& csv)
{
	// if chunks is empty, their is no more lines to write
	if (m_Chunks.empty()) { return true; }

	// if header isn't written, size of first line to write will be the reference
	if (m_ColumnCount == 0)
	{
		m_ColumnCount = m_Chunks.front().matrix.size();
		m_ColumnCount += s_PreDataColumnCount + s_PostDataColumnCount; // Will be set correctly with call to setFormatType
	}

	// loop will add a line to the buffer while the last stimulation date registered is greater than the end of the current chunk or until their is an event
	uint64_t linesWritten = 0;

	while (!m_Chunks.empty() && (canWriteAll
								 || (m_Stimulations.empty() && m_Chunks.front().endTime <= m_NoEventSince)
								 || (!m_Stimulations.empty() && m_Chunks.front().startTime <= m_Stimulations.back().stimulationDate))
	)
	{
		// Signal data must be written as sampleCounterPerBuffer th lines;
		if (m_InputTypeIdentifier == EStreamType::Signal
			&& canWriteAll == false
			&& linesWritten != 0
			&& linesWritten % m_SampleCountPerBuffer == 0) { break; }
		// check line size

		if ((m_InputTypeIdentifier == EStreamType::Signal || m_InputTypeIdentifier == EStreamType::Spectrum)
			&& (m_Chunks.front().matrix.size() + s_PreDataColumnCount + s_PostDataColumnCount) != m_ColumnCount)
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_WrongLineSize;
			return false;
		}

		if (m_InputTypeIdentifier == EStreamType::FeatureVector
			|| m_InputTypeIdentifier == EStreamType::CovarianceMatrix
			|| m_InputTypeIdentifier == EStreamType::StreamedMatrix)
		{
			uint32_t columnstoHave = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1U, std::multiplies<uint32_t>());
			columnstoHave += s_PreDataColumnCount + s_PostDataColumnCount;

			if (columnstoHave != m_ColumnCount)
			{
				m_LastStringError = "Line size is " + std::to_string(columnstoHave) + " but must be " + std::to_string(m_ColumnCount);
				m_LogError        = LogErrorCodes_WrongLineSize;
				return false;
			}
		}

		// Time and Epoch
		const std::pair<double, double> currentTime = { m_Chunks.front().startTime, m_Chunks.front().endTime };
		char buffer[s_MaximumFloatDecimal];

		snprintf(buffer, s_MaximumFloatDecimal, "%.*f", m_OutputFloatPrecision, currentTime.first);
		csv += buffer;

		switch (m_InputTypeIdentifier)
		{
			case EStreamType::Spectrum:
			case EStreamType::StreamedMatrix:
			case EStreamType::CovarianceMatrix:
			case EStreamType::FeatureVector:
				snprintf(buffer, s_MaximumFloatDecimal, "%.*f", m_OutputFloatPrecision, currentTime.second);
				csv += std::string(1, s_Separator) + std::string(buffer);
				break;

			case EStreamType::Signal:
				csv += std::string(1, s_Separator) + std::to_string(m_Chunks.front().epoch);
				break;
		}

		// Matrix
		for (const double& value : m_Chunks.front().matrix)
		{
			csv += s_Separator;
			snprintf(buffer, s_MaximumFloatDecimal, "%.*f", m_OutputFloatPrecision, value);
			csv += buffer;
		}

		m_Chunks.pop_front();

		csv += s_Separator;

		// Stimulations
		if (!m_Stimulations.empty())
		{
			std::vector<SStimulationChunk> stimulationsToWrite;

			double stimulationTime = m_Stimulations.front().stimulationDate;

			while ((stimulationTime - currentTime.first) < (currentTime.second - currentTime.first))
			{
				stimulationsToWrite.push_back(m_Stimulations.front());
				m_Stimulations.pop_front();

				if (m_Stimulations.empty()) { break; }

				stimulationTime = m_Stimulations.front().stimulationDate;
			}

			csv += stimulationsToString(stimulationsToWrite);
		}
		else { csv += std::string(2, s_Separator); }

		csv += "\n";
		linesWritten++;
	}

	return true;
}

bool CCSVHandler::parseHeader()
{
	if (m_IsHeaderRead) { return true; }

	std::string header;
	m_Fs.clear(); // Useful if the end of file is reached before.
	m_Fs.seekg(0);

	if (!this->streamReader(m_Fs, header, s_EndOfLineChar, m_BufferReadFileLine))
	{
		m_LastStringError = "No header in the file or file empty";
		m_LogError        = LogErrorCodes_EmptyColumn;
		return false;
	}

	m_IsHeaderRead = true;

	std::vector<std::string> columns;

	this->split(header, s_Separator, columns);

	m_ColumnCount = columns.size();

	if (columns.empty())
	{
		m_LastStringError = "No header in the file or file empty";
		m_LogError        = LogErrorCodes_EmptyColumn;
		return false;
	}

	for (const std::string& columnPart : columns)
	{
		if (columnPart.empty())
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_EmptyColumn;
			return false;
		}
	}

	if (!m_HasInputType)
	{
		m_LastStringError = "Cannot read header without input type";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	m_Fs.clear();				   // Useful if the end of file is reached before.
	m_Fs.seekg(header.size() + 2); // Go to the begin of the next line
	m_BufferReadFileLine.clear();

	switch (m_InputTypeIdentifier)
	{
		case EStreamType::Signal:
			if (!this->parseSignalHeader(columns))
			{
				m_LogError = LogErrorCodes_WrongHeader;
				return false;
			}
			break;

		case EStreamType::Spectrum:
			if (!this->parseSpectrumHeader(columns))
			{
				m_LogError = LogErrorCodes_WrongHeader;
				return false;
			}
			break;

		case EStreamType::StreamedMatrix:
		case EStreamType::CovarianceMatrix:
		case EStreamType::FeatureVector:
			if (!this->parseMatrixHeader(columns))
			{
				m_LogError = LogErrorCodes_WrongHeader;
				return false;
			}
			break;
	}

	m_Fs.clear();				   // Useful if the end of file is reached before.
	m_Fs.seekg(header.size() + 2); // Go to the begin of the next line
	m_BufferReadFileLine.clear();

	// check stimulations
	if (columns.cend()[-int(s_PostDataColumnCount)] != s_EventIdentifierColumn
		|| columns.cend()[-int(s_PreDataColumnCount)] != s_EventDateColumn
		|| columns.back() != s_EventDurationColumn)
	{
		m_LastStringError = "Three last column must be \"Even Id\", \"Event Date\" and \"Event Duration\"";
		m_LogError        = LogErrorCodes_WrongHeader;
		return false;
	}

	m_ColumnCount = columns.size();

	return true;
}

bool CCSVHandler::parseSignalHeader(const std::vector<std::string>& header)
{
	//check time
	if (header[s_TimeColumnIndex].substr(0, 5) != "Time:"
		|| header[s_TimeColumnIndex].substr(header[s_TimeColumnIndex].size() - 2) != "Hz"
		|| header[s_TimeColumnIndex].size() <= 7)
	{
		m_LastStringError = "First column (" + header[s_TimeColumnIndex] + ") is not well formed";
		return false;
	}

	// get sampling rate
	try { m_SamplingRate = std::stoul(header[s_TimeColumnIndex].substr(5, header[s_TimeColumnIndex].size() - 3)); }
	catch (const std::exception& ia)
	{
		m_LastStringError = "On entry \"" + header[0].substr(5, header[0].size() - 3) + "\", exception have been thrown: ";
		m_LastStringError += ia.what();
		return false;
	}

	// check epoch
	if (header[s_SignalEpochColumnIndex] != "Epoch")
	{
		m_LastStringError = "Second column (" + header[s_SignalEpochColumnIndex] + ") must be Epoch column";
		return false;
	}

	// get dimension labels
	for (size_t index = s_PreDataColumnCount; index < header.size() - s_PostDataColumnCount; index++) { m_DimensionLabels.push_back(header[index]); }

	if (!this->calculateSampleCountPerBuffer()) { return false; } // m_LastErrorString set in the function, m_LogError set outside the function

	return true;
}

bool CCSVHandler::parseSpectrumHeader(const std::vector<std::string>& header)
{
	m_DimensionSizes.clear();
	// check time column
	std::string buffer;
	std::istringstream iss(header[s_TimeColumnIndex]);

	std::string bufferTemp;

	if (!this->streamReader(iss, buffer, s_InternalDataSeparator, bufferTemp))
	{
		m_LastStringError = "First column (" + header[s_TimeColumnIndex] + ") is empty";
		return false;
	}

	if (buffer != "Time")
	{
		m_LastStringError = "First column (" + header[s_TimeColumnIndex] +
							") is not well formed, must have \"Time\" written before the first inqternal data separator (\':\' as default)";
		return false;
	}

	auto getNextElem = [&](uint32_t& resultvar, const char separator, const char* missingString)
	{
		this->streamReader(iss, buffer, separator, bufferTemp);

		if (buffer.empty()) { m_LastStringError = "First column (" + header[s_TimeColumnIndex] + ") is not well formed, missing " + missingString; }

		try { resultvar = std::stoul(buffer); }
		catch (std::exception& e)
		{
			m_LastStringError = "On entry \"" + buffer + "\", exception have been thrown: ";
			m_LastStringError += e.what();
			return false;
		}

		return true;
	};

	uint32_t dimensionSize = 0;

	if (!getNextElem(dimensionSize, s_DimensionSeparator, "channels number")) { return false; }

	m_DimensionSizes.push_back(dimensionSize);

	if (!getNextElem(dimensionSize, s_InternalDataSeparator, "number frequencies per channel")) { return false; }

	m_DimensionSizes.push_back(dimensionSize);

	if (!getNextElem(m_OriginalSampleNumber, '\0', "original number of samples")) { return false; }

	if (m_DimensionSizes[0] == 0 || m_DimensionSizes[1] == 0)
	{
		m_LastStringError = "Dimension size must be only positive";
		return false;
	}

	//check Time End column
	if (header[s_EndTimeColumnIndex] != "End Time")
	{
		m_LastStringError = "Second column (" + header[s_EndTimeColumnIndex] + ") must be End Time Column";
		return false;
	}

	// add every new label

	std::string channelLabel;
	m_DimensionLabels.clear();

	for (uint32_t labelCounter = 0; labelCounter < m_DimensionSizes[0]; labelCounter++)
	{
		double lastFrequency = 0.0;

		for (size_t labelSizeCounter = 0; labelSizeCounter < m_DimensionSizes[1]; labelSizeCounter++)
		{
			std::string dimensionData;
			// get channel name and check that it's the same one for all the frequency dimension size
			std::vector<std::string> spectrumChannel;
			this->split(header.at((labelCounter * m_DimensionSizes[1]) + labelSizeCounter + s_PreDataColumnCount), s_InternalDataSeparator, spectrumChannel);

			if (spectrumChannel.size() != 2)
			{
				m_LastStringError = "Spectrum channel is invalid: " + header.at((labelCounter * m_DimensionSizes[1]) + labelSizeCounter + s_PreDataColumnCount);
				return false;
			}

			dimensionData = spectrumChannel[0];

			if (labelSizeCounter == 0)
			{
				channelLabel = dimensionData;
				m_DimensionLabels.push_back(channelLabel);
			}
			else if (channelLabel != dimensionData)
			{
				m_LastStringError = "Channel name must be the same during " + std::to_string(m_DimensionSizes[1]) +
									" columns (number of frequencies per channel)";
				return false;
			}

			// get all frequency and check that they're the same for all labels
			dimensionData = spectrumChannel[1];

			if (labelCounter == 0)
			{
				double frequency;

				try { frequency = std::stod(dimensionData); }
				catch (const std::exception& e)
				{
					m_LastStringError = "On entry \"" + dimensionData + "\", exception have been thrown: ";
					m_LastStringError += e.what();
					return false;
				}

				m_FrequencyAbscissa.push_back(frequency);
			}
			else
			{
				double frequency;

				try { frequency = std::stod(dimensionData); }
				catch (const std::exception& e)
				{
					m_LastStringError = "On entry \"" + dimensionData + "\", exception have been thrown: ";
					m_LastStringError += e.what();
					return false;
				}

				if (labelSizeCounter == 0) { lastFrequency = frequency; }
				else if (frequency < lastFrequency)
				{
					m_LastStringError = "Frequencies must be in ascending order";
					return false;
				}

				if (!(std::fabs(frequency - m_FrequencyAbscissa[labelSizeCounter]) < std::numeric_limits<double>::epsilon()))
				{
					m_LastStringError = "Channels must have the same frequency bands";
					return false;
				}
			}
		}
	}

	m_SampleCountPerBuffer = 1;
	return true;
}

bool CCSVHandler::parseMatrixHeader(const std::vector<std::string>& header)
{
	// check time column
	std::istringstream iss(header[s_TimeColumnIndex]);
	std::string linePart;
	// check Time is written

	std::string bufferTemp;

	if (!this->streamReader(iss, linePart, s_InternalDataSeparator, bufferTemp))
	{
		m_LastStringError = "First column is empty";
		return false;
	}
	if (linePart != "Time")
	{
		m_LastStringError = "First column " + header[s_TimeColumnIndex] + " is not well formed";
		return false;
	}

	linePart = header[s_TimeColumnIndex].substr(5);

	m_DimensionSizes.clear();
	std::vector<std::string> dimensionParts;
	this->split(linePart, s_DimensionSeparator, dimensionParts);
	m_DimensionCount = 0;

	for (const std::string& dimensionSize : dimensionParts)
	{
		uint32_t size = 0;

		try { size = std::stoul(dimensionSize); }
		catch (std::exception& e)
		{
			m_LastStringError = "Error on a dimension size, exception have been thrown: ";
			m_LastStringError += e.what();
			return false;
		}

		if (size == 0)
		{
			m_LastStringError = "A dimension size must be strictly positive";
			return false;
		}

		m_DimensionSizes.push_back(size);
		m_DimensionCount++;
	}

	if (m_DimensionCount == 0)
	{
		m_LastStringError = "First column must indicate at least one dimension size";
		return false;
	}

	// check columnLabels number according to dimension sizes
	uint32_t matrixColumnCount = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1, std::multiplies<uint32_t>());

	if ((matrixColumnCount + s_PreDataColumnCount + s_PostDataColumnCount) != header.size())
	{
		m_LastStringError = "Every line must have " + std::to_string(matrixColumnCount + s_PreDataColumnCount + s_PostDataColumnCount) + " columnLabels";
		return false;
	}

	// it saves labels for each dimensions
	std::vector<std::vector<std::string>> labelsInDimensions(m_DimensionCount);

	// check if labels are already filled or if it's a reset (labels are empty before being set, but empty labels are accepted)
	std::vector<std::vector<bool>> filledLabel(m_DimensionCount);

	for (size_t index = 0; index < m_DimensionSizes.size(); index++)
	{
		labelsInDimensions[index].resize(m_DimensionSizes[index]);
		filledLabel[index].resize(m_DimensionSizes[index], false);
	}

	// corresponding to the position in the multi multidimensional matrix (as exemple the third label of the second dimension will be positionsInDimensions[1] = 2)
	std::vector<uint32_t> positionsInDimensions(m_DimensionCount, 0);
	size_t columnIndex = 0;

	// we will visit each column containing matrix labels
	do
	{
		std::vector<std::string> columnLabels;

		// get all column part
		this->split(header[columnIndex + s_PreDataColumnCount], s_InternalDataSeparator, columnLabels);

		if (columnLabels.size() != m_DimensionCount)
		{
			m_LastStringError = "On column " + std::to_string(columnIndex + s_PreDataColumnCount) + "(" + header[columnIndex + s_PreDataColumnCount] +
								"), there is " + std::to_string(columnLabels.size()) + " label instead of " + std::to_string(m_DimensionCount);
			m_LogError = LogErrorCodes_WrongHeader;
			return false;
		}

		// check column labels one per one
		for (size_t dimensionIndex = 0; dimensionIndex < columnLabels.size(); dimensionIndex++)
		{
			size_t positionInCurrentDimension = positionsInDimensions[dimensionIndex];

			if (columnLabels[dimensionIndex].empty())
			{
				// if saved label is empty, mark it as saved (even if it is already)
				if (labelsInDimensions[dimensionIndex][positionInCurrentDimension].empty()) { filledLabel[dimensionIndex][positionInCurrentDimension] = true; }
					// else,there is an error, it means that label is already set
				else
				{
					m_LastStringError = "Error at column " + std::to_string(columnIndex + 1)
										+ " for the label \"" + columnLabels[dimensionIndex]
										+ "\" in dimension " + std::to_string(dimensionIndex + 1)
										+ " is trying to reset label to \"" + columnLabels[dimensionIndex]
										+ "\" that have been already set to \"" + labelsInDimensions[dimensionIndex][positionInCurrentDimension]
										+ "\"";
					m_LogError = LogErrorCodes_WrongHeader;
					return false;
				}
			}
			else
			{
				// if label is already set, check that it'st the same, if it's not, there is an error
				if (labelsInDimensions[dimensionIndex][positionInCurrentDimension] != columnLabels[dimensionIndex]
					&& filledLabel[dimensionIndex][positionInCurrentDimension])
				{
					m_LastStringError = "Error at column " + std::to_string(columnIndex + 1) + " for the label \"" + columnLabels[dimensionIndex]
										+ "\" in dimension " + std::to_string(dimensionIndex + 1) + " is trying to reset label to \"" + columnLabels[
											dimensionIndex]
										+ "\" that have been already set to \"" + labelsInDimensions[dimensionIndex][positionInCurrentDimension] + "\"";
					m_LogError = LogErrorCodes_WrongHeader;
					return false;
				}
				// if label isn't set, set it
				if (!(filledLabel[dimensionIndex][positionInCurrentDimension]))
				{
					labelsInDimensions[dimensionIndex][positionInCurrentDimension] = columnLabels[dimensionIndex];
					filledLabel[dimensionIndex][positionInCurrentDimension]        = true;
				}
			}
		}

		columnIndex++;
	} while (increasePositionIndexes(positionsInDimensions));

	for (const std::vector<std::string>& dimensionIndex : labelsInDimensions)
	{
		m_DimensionLabels.insert(m_DimensionLabels.end(), dimensionIndex.begin(), dimensionIndex.end());
	}

	m_SampleCountPerBuffer = 1;

	return true;
}

bool CCSVHandler::readSampleChunk(const std::string& line, SMatrixChunk& sample, const uint64_t lineNb)
{
	std::string::const_iterator firstColumn  = std::find(line.cbegin(), line.cend(), s_Separator);
	std::string::const_iterator secondColumn = std::find(firstColumn + 1, line.cend(), s_Separator);

	if (lineNb % m_SampleCountPerBuffer == 0)
	{
		if (!boost::spirit::qi::parse(line.cbegin(), firstColumn,
									  boost::spirit::qi::double_,
									  sample.startTime))
		{
			m_LastStringError = "Invalid value for the start time. Error on line " + std::to_string(lineNb);
			m_LogError        = LogErrorCodes_InvalidArgumentException;
			return false;
		}
	}

	if (m_InputTypeIdentifier == EStreamType::Signal)
	{
		if (!boost::spirit::qi::parse(firstColumn + 1, secondColumn,
									  boost::spirit::qi::ulong_long,
									  sample.epoch))
		{
			m_LastStringError = "Invalid value for the epoch. Error on line " + std::to_string(lineNb);
			m_LogError        = LogErrorCodes_InvalidArgumentException;
			return false;
		}

		sample.endTime = sample.startTime + (double(m_SampleCountPerBuffer) / double(m_SamplingRate));
	}
	else
	{
		sample.epoch = std::numeric_limits<uint64_t>::max();

		if (!boost::spirit::qi::parse(firstColumn + 1, secondColumn,
									  boost::spirit::qi::double_,
									  sample.endTime))
		{
			m_LastStringError = "Invalid value for the end time. Error on line " + std::to_string(lineNb);
			m_LogError        = LogErrorCodes_InvalidArgumentException;
			return false;
		}
	}

	const size_t eventDurationColumn = line.find_last_of(s_Separator);
	const size_t eventDateColumn     = line.find_last_of(s_Separator, eventDurationColumn - 1);
	const size_t eventIdColumn       = line.find_last_of(s_Separator, eventDateColumn - 1);

	std::vector<double> columnsMatrix;

	boost::spirit::qi::phrase_parse(secondColumn + 1, line.cbegin() + eventIdColumn,
									boost::spirit::qi::double_ % s_Separator,
									boost::spirit::ascii::space,
									columnsMatrix);

	if (columnsMatrix.size() != m_ColumnCount - s_PostDataColumnCount - s_PreDataColumnCount)
	{
		m_LastStringError = "Invalid number of channel. Error on line " + std::to_string(lineNb);
		m_LogError        = LogErrorCodes_InvalidArgumentException;
		return false;
	}

	if (m_InputTypeIdentifier == EStreamType::Signal)
	{
		for (size_t index = 0; index < m_DimensionLabels.size(); ++index)
		{
			sample.matrix[(index * m_SampleCountPerBuffer) + uint32_t(lineNb)] = columnsMatrix[index];
		}
	}
	else if (m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		for (size_t index = 0; index < columnsMatrix.size(); ++index)
		{
			sample.matrix[(index * m_SampleCountPerBuffer) + uint32_t(lineNb)] = columnsMatrix[index];
		}
	}
	else
	{
		sample.matrix.clear();
		std::move(columnsMatrix.begin(), columnsMatrix.end(), std::back_inserter(sample.matrix));
	}

	return true;
}

bool CCSVHandler::readStimulationChunk(const std::string& line, std::vector<SStimulationChunk>& stimulations, const uint64_t lineNb)
{
	size_t eventDurationColumn = line.find_last_of(s_Separator);
	size_t eventDateColumn     = line.find_last_of(s_Separator, eventDurationColumn - 1);
	size_t eventIdColumn       = line.find_last_of(s_Separator, eventDateColumn - 1);

	std::vector<uint64_t> stimIdentifiers;
	// pick all time identifiers for the current time
	boost::spirit::qi::phrase_parse(line.cbegin() + eventIdColumn + 1, line.cbegin() + eventDateColumn,
									boost::spirit::qi::ulong_long % s_InternalDataSeparator,
									boost::spirit::ascii::space,
									stimIdentifiers);

	std::vector<double> stimDates;
	// pick all time identifiers for the current time
	boost::spirit::qi::phrase_parse(line.cbegin() + eventDateColumn + 1, line.cbegin() + eventDurationColumn,
									boost::spirit::qi::double_ % s_InternalDataSeparator,
									boost::spirit::ascii::space,
									stimDates);

	std::vector<double> stimDurations;
	// pick all time identifiers for the current time
	boost::spirit::qi::phrase_parse(line.cbegin() + eventDurationColumn + 1, line.cend(),
									boost::spirit::qi::double_ % s_InternalDataSeparator,
									boost::spirit::ascii::space,
									stimDurations);

	if (stimIdentifiers.size() != stimDates.size() || stimIdentifiers.size() != stimDurations.size())
	{
		m_LastStringError = "There is " + std::to_string(stimIdentifiers.size()) + " identifiers, " + std::to_string(stimDates.size()) + " dates, and " + std::
							to_string(stimDurations.size()) + " durations";
		m_LogError = LogErrorCodes_StimulationSize;
		return false;
	}

	for (size_t index = 0; index < stimIdentifiers.size(); index++)
	{
		stimulations.emplace_back(stimIdentifiers[index], stimDates[index], stimDurations[index]);
	}

	return true;
}

bool CCSVHandler::increasePositionIndexes(std::vector<uint32_t>& position)
{
	position.back()++;

	for (size_t counter = 1; counter <= position.size(); counter++)
	{
		const size_t index = position.size() - counter;

		if ((position[index] + 1) > m_DimensionSizes[index])
		{
			if (index != 0)
			{
				position[index] = 0;
				position[index - 1]++;
			}
			else if ((position[index] + 1) > m_DimensionSizes[index]) { return false; }
		}
	}
	return true;
}

bool CCSVHandler::calculateSampleCountPerBuffer()
{
	// get samples per buffer
	std::vector<std::string> lineParts({ "", "0" });
	uint32_t sampleCount = 0;

	std::string bufferTemp;

	while (lineParts[s_SignalEpochColumnIndex] == "0")
	{
		std::string line;

		if (!this->streamReader(m_Fs, line, s_EndOfLineChar, bufferTemp))
		{
			// protect against sampleCount--, no need here
			sampleCount++;
			break;
		}

		lineParts.clear();
		this->split(line, s_Separator, lineParts);

		if (lineParts.size() != size_t(m_ColumnCount))
		{
			m_LastStringError = "File may be corrupt, can't found sample count per buffer";
			return false;
		}

		sampleCount++;
	}

	// assume that we might read too far
	sampleCount--;

	if (sampleCount == 0)
	{
		m_LastStringError = "File contain no data to get sample count per buffer, or is corrupted";
		return false;
	}

	m_SampleCountPerBuffer = sampleCount;
	return true;
}

bool CCSVHandler::hasDataToRead() const { return m_HasDataToRead; }

CSV_API ICSVHandler* CSV::createCSVHandler() { return new CCSVHandler(); }

CSV_API void CSV::releaseCSVHandler(ICSVHandler* object) { delete dynamic_cast<CCSVHandler *>(object); }
