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
#include <numeric>

#include "ovCCSV.hpp"

using namespace OpenViBE;
using namespace OpenViBE::CSV;

namespace {
	static const int s_StimulationDateColumnNbr = 1;
	static const int s_StimulationIdentifierColumnNbr = 2;
	static const unsigned int s_SignalEpochColumnIndex = 1;
	static const unsigned int s_TimeColumnIndex = 0;
	static const unsigned int s_EndTimeColumnIndex = 1;
	// Number of columns before data (Time/Epoch)
	static const unsigned int s_PreDataColumnCount = 2;
	// Number of columns after data (Events)
	static const unsigned int s_PostDataColumnCount = 3;

	//Separators
	static const char s_Separator(',');
	static const char s_InternalDataSeparator(':');
	static const char s_DimensionSeparator('x');
	// Columns Names
	static const std::string s_EventIdentifierColumn = "Event Id";
	static const std::string s_EventDateColumn = "Event Date";
	static const std::string s_EventDurationColumn = "Event Duration";

	// Uses std::getline when the system and file EOF match, otherwise falls back to a
	// custom solution wich supports all common endlines
	std::istream& universalGetLine(std::istream& inputStream, std::string& outputString, const char delimiter = '\n')
	{
		outputString.clear();

		// The characters in the stream are read one-by-one using a std::streambuf.
		// That is faster than reading them one-by-one using the std::istream.
		// Code that uses streambuf this way must be guarded by a sentry object.
		// The sentry object performs various tasks,
		// such as thread synchronization and updating the stream state.

		std::istream::sentry sentry(inputStream, true);
		std::streambuf* buffer = inputStream.rdbuf();

		// We need to read a character from the stream to initailize in_avail count,
		// otherwise the next call would return 0
		buffer->sgetc();

		if (buffer->in_avail() == 0)
		{
			inputStream.setstate(std::ios::failbit);
		}

		while (true)
		{
			int c = buffer->sbumpc();

			if (c == '\n')
			{
				return inputStream;
			}
			else if (c == '\r')
			{
				if(buffer->sgetc() == '\n')
				{
					buffer->sbumpc();
				}
				return inputStream;
			}
			else if (c == std::istream::traits_type::eof())
			{
				// Also handle the case when the last line has no line ending
				if (outputString.empty() || inputStream.peek() == std::istream::traits_type::eof())
				{
					inputStream.setstate(std::ios::eofbit);
				}
				return inputStream;
			}
			else if (c == delimiter)
			{
				return inputStream;
			}
			else
			{
				outputString += (char)c;
			}
		}
	}

}

CCSVHandler::CCSVHandler(void)
	: m_LastStringError(""),
	m_DimensionCount(0),
	m_DimensionSizes({}),
	m_NoEventSince(0),
	m_SamplingRate(0),
	m_ColumnCount(0),
	m_PreDataColumnCount(0), // Time
	m_PostDataColumnCount(0), // Event Id, Event Date and Event Duration
	m_HasInputType(false),
	m_IsFirstLineWritten(false),
	m_IsHeaderRead(false),
	m_IsSetInfoCalled(false),
	m_HasEpoch(false),
	m_OutputFloatPrecision(10)
{
}

CCSVHandler::~CCSVHandler(void)
{
	closeFile();
}

void CCSVHandler::split(const std::string& string, char delimitor, std::vector<std::string>& element)
{
	std::stringstream stringStream(string);
	std::string item;

	while (m_GetLineFunction(stringStream, item, delimitor))
	{
		element.push_back(item);
	}

	// if the list of channels ends with a trailing ; then we have to count the empty channel
	if (string.empty() || *string.rbegin() == delimitor)
	{
		element.push_back("");
	}
}


void CCSVHandler::setFormatType(EStreamType typeIdentifier)
{
	m_InputTypeIdentifier = typeIdentifier;
	switch (m_InputTypeIdentifier) {
	case EStreamType::Signal :
	case EStreamType::Spectrum :
	case EStreamType::StreamedMatrix :
	case EStreamType::CovarianceMatrix :
	case EStreamType::FeatureVector :
		m_PreDataColumnCount = s_PreDataColumnCount;
		m_PostDataColumnCount = s_PostDataColumnCount;
		break;
	}
	m_HasInputType = true;
}

EStreamType CCSVHandler::getFormatType(void)
{
	return m_InputTypeIdentifier;
}

bool CCSVHandler::setSignalInformation(const std::vector<std::string>& channelNames, unsigned int samplingFrequency, unsigned int sampleCountPerBuffer)
{
	if (m_InputTypeIdentifier != EStreamType::Signal)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}
	else if (m_IsSetInfoCalled)
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

	m_SamplingRate = samplingFrequency;
	m_DimensionLabels = channelNames;
	m_SampleCountPerBuffer = sampleCountPerBuffer;
	return true;
}

bool CCSVHandler::getSignalInformation(std::vector<std::string>& channelNames, unsigned int& samplingFrequency, unsigned int& sampleCountPerBuffer)
{
	if (m_InputTypeIdentifier != EStreamType::Signal)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (!parseHeader())
	{
		return false;
	}

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_SamplingRate == 0)
	{
		m_LastStringError = "Sampling rate invalid";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_SampleCountPerBuffer == 0)
	{
		m_LastStringError = "Sample count per buffer is invalid";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	samplingFrequency = m_SamplingRate;
	sampleCountPerBuffer = m_SampleCountPerBuffer;
	return true;
}

bool CCSVHandler::setSpectrumInformation(const std::vector<std::string>& channelNames, const std::vector<double>& frequencyAbscissa, const unsigned int samplingRate)
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

	m_DimensionLabels = channelNames;
	m_DimensionSizes = { static_cast<unsigned int>(channelNames.size()), static_cast<unsigned int>(frequencyAbscissa.size()) };
	m_FrequencyAbscissa = frequencyAbscissa;
	m_OriginalSampleNumber = samplingRate;
	return true;
}

bool CCSVHandler::getSpectrumInformation(std::vector<std::string> &channelNames, std::vector<double> &frequencyAbscissa, unsigned int& samplingRate)
{
	if (m_InputTypeIdentifier != EStreamType::Spectrum)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (!parseHeader())
	{
		return false;
	}

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_FrequencyAbscissa.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_SampleCountPerBuffer == 0)
	{
		m_LastStringError = "Sample count per buffer is invalid";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	frequencyAbscissa = m_FrequencyAbscissa;
	samplingRate = m_OriginalSampleNumber;
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
	else if (m_IsSetInfoCalled)
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
	m_DimensionSizes = { static_cast<unsigned int>(channelNames.size())};
	m_DimensionCount = 1;
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

	if (!parseHeader())
	{
		return false;
	}

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	channelNames = m_DimensionLabels;
	return true;
}

bool CCSVHandler::setStreamedMatrixInformation(const std::vector<unsigned int>& dimensionSizes, const std::vector<std::string>& labels)
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
	unsigned int size = 0;
	for (size_t index = 0; index < dimensionSizes.size(); index++)
	{
		size += dimensionSizes[index];
	}

	if (size != labels.size())
	{
		m_LastStringError = "dimension count is " + std::to_string(size) + " and labels count is " + std::to_string(labels.size());
		m_LogError = LogErrorCodes_WrongDimensionSize;
		return false;
	}
	m_IsSetInfoCalled = true;
	m_DimensionSizes = dimensionSizes;
	m_DimensionCount = m_DimensionSizes.size();
	m_DimensionLabels = labels;
	return true;
}

bool CCSVHandler::getStreamedMatrixInformation(std::vector<unsigned int>& dimensionSizes, std::vector<std::string>& labels)
{
	if (m_InputTypeIdentifier != EStreamType::StreamedMatrix && m_InputTypeIdentifier != EStreamType::CovarianceMatrix)
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongInputType;
		return false;
	}

	if (!parseHeader())
	{
		return false;
	}

	if (m_DimensionSizes.empty())
	{
		m_LastStringError = "no dimension sizes, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	if (m_DimensionLabels.empty())
	{
		m_LastStringError = "No dimension labels, header reading may have failed";
		m_LogError = LogErrorCodes_WrongHeader;
		return false;
	}

	dimensionSizes = m_DimensionSizes;
	labels = m_DimensionLabels;
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
	if (mode == EFileAccessMode::Write)
	{
		FILE *file = fopen(m_Filename.c_str(), "w");
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
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_CantOpenFile;
		return false;
	}

	// check the line endings, if the file is opened for reading
	// TODO: When appending to a file check the endlines first and use the original ones
	m_GetLineFunction = std::getline;
	if (mode == EFileAccessMode::Read)
	{
		char c;
		while (!m_Fs.eof())
		{
			m_Fs.get(c);
			if (c == '\n')
			{
#if defined TARGET_OS_Windows
				m_GetLineFunction = universalGetLine;
#endif
				break;
			}
			else if (c == '\r')
			{
				if (m_Fs.peek() == '\n')
				{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
					m_GetLineFunction = universalGetLine;
#endif
				}
				else
				{
					m_GetLineFunction = universalGetLine;
				}
				break;
			}
			else if (c == '\0' || c == std::char_traits<char>::eof())
			{
				break;
			}
		}
		m_Fs.seekg(0, m_Fs.beg);
	}

	if (mode == EFileAccessMode::Append)
	{
		m_Fs.seekg(0, m_Fs.end);
	}

	m_IsHeaderRead = false;
	m_IsFirstLineWritten = false;
	m_IsSetInfoCalled = false;
	m_HasInputType = false;
	m_HasEpoch = false;
	return true;
}

bool CCSVHandler::readSamplesAndEventsFromFile(unsigned long long chunksToRead, std::vector<SMatrixChunk>& chunks, std::vector<SStimulationChunk>& stimulations)
{
	if (!m_IsHeaderRead)
	{
		m_LastStringError = "Trying to read data without having read a header";
		// TODO: Set the error code
		return false;
	}

	chunks.clear();
	SMatrixChunk chunk(0, 0, {}, 0);

	while (chunks.size() < chunksToRead)
	{
		if (m_InputTypeIdentifier == EStreamType::Signal && chunks.size() == 0)
		{
			chunk.matrix.resize(m_SampleCountPerBuffer * (m_ColumnCount - (m_PreDataColumnCount + m_PostDataColumnCount)));
		}
		else if (m_InputTypeIdentifier == EStreamType::Spectrum && chunks.size() == 0)
		{
			chunk.matrix.resize(m_SampleCountPerBuffer * (m_DimensionSizes[0] * m_DimensionSizes[1]));
		}

		for (unsigned long long line = 0; line < m_SampleCountPerBuffer; line++)
		{
			if (!m_GetLineFunction(m_Fs, m_Buffer, '\n'))
			{
				if (line != 0)
				{
					m_LastStringError = "Chunk is not complete";
					m_LogError = LogErrorCodes_MissingData;
					return false;
				}
				else
				{
					// There is no more data to read
					return true;
				}
			}

			m_LineColumns.clear();
			this->split(m_Buffer, s_Separator, m_LineColumns);
			if (m_LineColumns[0].empty())
			{
				if (line != 0)
				{
					m_LastStringError = "Can't reach all the chunk, file doesn't get enough";
					m_LogError = LogErrorCodes_WrongParameters;
					return false;
				}
				else
				{
					// There is no unread chunk left
					return true;
				}
			}

			if (m_LineColumns.size() != m_ColumnCount)
			{
				m_LastStringError = "There is " + std::to_string(m_LineColumns.size())
					+ " columns in the Header instead of " + std::to_string(m_ColumnCount)
					+ " on line " + std::to_string(line + 1);
				m_LogError = LogErrorCodes_WrongLineSize;
				return false;
			}
			//get matrix data
			for (size_t index = 0; index < m_LineColumns.size(); index++)
			{
				if (m_LineColumns[index].empty() && index < m_LineColumns.size() - m_PostDataColumnCount)
				{
					m_LastStringError = "Empty at index " + std::to_string(index);
					m_LogError = LogErrorCodes_EmptyColumn;
					return false;
				}
			}

			// get Matrix chunk, LogError set in the function
			if (!readSampleChunk(chunk, line))
			{
				return false;
			}

			// get stimulations chunk, LogError set in the function
			if (!readStimulationChunk(stimulations, line + 1))
			{
				return false;
			}

			m_LineColumns.clear();
		}

		chunks.push_back(chunk);
	}

	return true;
}

bool CCSVHandler::writeHeaderToFile(void)
{
	if (!m_Fs.is_open())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_NoFileDefined;
		return false;
	}

	if (m_IsFirstLineWritten)
	{
		m_LastStringError = "Header already written";
		m_LogError = LogErrorCodes_CantWriteHeader;
		return false;
	}
	// set header (in case of error, logError set in function)
	std::string header = createHeaderString();
	if (header == "")
	{
		return false;
	}
	m_IsFirstLineWritten = true;
	try
	{
		m_Fs << header;
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

bool CCSVHandler::writeDataToFile(void)
{
	if (!m_Fs.is_open())
	{
		m_LastStringError.clear();
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
		m_Fs << m_Buffer;
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

bool CCSVHandler::writeAllDataToFile(void)
{
	// in case of error, logError set in the function
	if (!createCSVStringFromData(true))
	{
		return false;
	}

	try
	{
		m_Fs << m_Buffer;
	}
	catch (std::ios_base::failure& fail)
	{
		m_LastStringError = "Error occured while writing: ";
		m_LastStringError += fail.what();
		m_LogError = LogErrorCodes_ErrorWhileWriting;
		return false;
	}
	m_Buffer.clear();
	return true;
}

bool CCSVHandler::closeFile()
{
	m_Stimulations.clear();
	m_Chunks.clear();
	m_Buffer.clear();
	m_DimensionSizes.clear();
	m_DimensionLabels.clear();
	m_FrequencyAbscissa.clear();
	m_DimensionCount = 0;
	m_ColumnCount = 0;
	m_SamplingRate = 0;
	m_HasInputType = false;
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

bool CCSVHandler::addSample(const SMatrixChunk& sample)
{
	if (sample.matrix.empty())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_MatrixEmpty;
		return false;
	}
	else if (m_InputTypeIdentifier == EStreamType::Signal
		&& sample.matrix.size() != m_DimensionLabels.size())
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongMatrixSize;
		return false;
	}
	else if (m_InputTypeIdentifier == EStreamType::Spectrum
		&& sample.matrix.size() != (m_DimensionLabels.size() * m_FrequencyAbscissa.size()))
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_WrongMatrixSize;
		return false;
	}
	else if (m_InputTypeIdentifier != EStreamType::Signal
		&& m_InputTypeIdentifier != EStreamType::Spectrum)
	{
		unsigned int columnsToHave = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1U, std::multiplies<unsigned int>());
		if (sample.matrix.size() != columnsToHave)
		{
			m_LastStringError = "Matrix size is " + std::to_string(sample.matrix.size()) + " and size to have is " + std::to_string(columnsToHave);
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
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_WrongSampleDate;
			return false;
		}
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

	for (const SMatrixChunk& sample : samples)
	{
		if (sample.matrix.empty())
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_MatrixEmpty;
			return false;
		}
		else if (sample.matrix.size() != m_DimensionLabels.size())
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_WrongMatrixSize;
			return false;
		}

		for (unsigned int matrixIndex = 0; matrixIndex < sample.matrix[matrixIndex]; matrixIndex++)
		{
			if (std::signbit(sample.startTime) || std::signbit(sample.endTime) || sample.endTime < sample.startTime)
			{
				m_LastStringError.clear();
				m_LogError = LogErrorCodes_WrongSampleDate;
				return false;
			}
		}
	}
	m_Chunks.insert(m_Chunks.end(), samples.begin(), samples.end());
	return true;
}

bool CCSVHandler::addEvent(unsigned long long code, double date, double duration)
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

bool CCSVHandler::addEvent(const SStimulationChunk& stimulation)
{
	if (std::signbit(stimulation.stimulationDate))
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_DateError;
		return false;
	}

	if (std::signbit(stimulation.stimulationDuration))
	{
		m_LastStringError.clear();
		m_LogError = LogErrorCodes_DurationError;
		return false;
	}
	m_Stimulations.push_back(stimulation);
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

ELogErrorCodes CCSVHandler::getLastLogError()
{
	return m_LogError;
}

std::string CCSVHandler::getLastErrorString()
{
	return m_LastStringError;
}

std::string CCSVHandler::writeStimulations(const std::vector<SStimulationChunk>& stimulationToPrint)
{
	std::string stimulations;
	for (size_t index = 0; index < stimulationToPrint.size(); index++)
	{
		stimulations += std::to_string(stimulationToPrint[index].stimulationIdentifier);
		if ((index + 1) < stimulationToPrint.size())
		{
			stimulations += s_InternalDataSeparator;
		}
	}
	stimulations += s_Separator;

	char buffer[1024];
	for (size_t index = 0; index < stimulationToPrint.size(); index++)
	{
		sprintf(buffer, "%.*f", m_OutputFloatPrecision, stimulationToPrint[index].stimulationDate);
		stimulations += buffer;
		if ((index + 1) < stimulationToPrint.size())
		{
			stimulations += s_InternalDataSeparator;
		}
	}
	stimulations += s_Separator;

	for (size_t index = 0; index < stimulationToPrint.size(); index++)
	{
		sprintf(buffer, "%.*f", m_OutputFloatPrecision, stimulationToPrint[index].stimulationDuration);
		stimulations += buffer;
		if ((index + 1) < stimulationToPrint.size())
		{
			stimulations += s_InternalDataSeparator;
		}
	}
	return stimulations;
}

std::string CCSVHandler::createHeaderString(void)
{
	const std::string invalidHeader = "";
	std::string header;
	auto addColumn = [&](const std::string columnLabel)
	{
		if (m_ColumnCount != 0)
		{
				header += s_Separator + columnLabel;
		}
		else
		{
				header = columnLabel;
		}
		m_ColumnCount++;
	};

	// add Time Header
	switch (m_InputTypeIdentifier) {
	case EStreamType::Signal :
		addColumn(std::string("Time") + s_InternalDataSeparator + std::to_string(m_SamplingRate) + "Hz");
		break;
	case EStreamType::Spectrum :
		if (m_DimensionSizes.size() != 2)
		{
			m_LastStringError = "Channel names and number of frequency are needed to write time column";
			return invalidHeader;
		}
		addColumn(std::string("Time")
				  + s_InternalDataSeparator
				  + std::to_string(m_DimensionSizes[0])
				  + s_DimensionSeparator
				  + std::to_string(m_DimensionSizes[1])
				  + s_InternalDataSeparator
				  + std::to_string(m_OriginalSampleNumber != 0 //original number of samples or calculated one
						? m_OriginalSampleNumber
						: m_DimensionSizes[0] * m_DimensionSizes[1])
		);
		break;
	case EStreamType::CovarianceMatrix:
	case EStreamType::StreamedMatrix :
	case EStreamType::FeatureVector :
		if (m_DimensionCount == 0)
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_DimensionCountZero;
			return invalidHeader;
		}
		{
			std::string timeColumn(std::string("Time") + s_InternalDataSeparator);
			for (unsigned int index = 0; index < m_DimensionCount; index++)
			{
				timeColumn += std::to_string(m_DimensionSizes[index]);
				if ((index + 1) < m_DimensionCount)
				{
					timeColumn += s_DimensionSeparator;
				}
			}
			addColumn(timeColumn);
		}
		break;
	}

	// add Epoch Header to signal
	switch (m_InputTypeIdentifier) {
	case EStreamType::Signal :
		addColumn("Epoch");
		break;
	case EStreamType::Spectrum :
	case EStreamType::StreamedMatrix :
	case EStreamType::CovarianceMatrix :
	case EStreamType::FeatureVector :
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

	switch (m_InputTypeIdentifier) {
	case EStreamType::Signal :
	case EStreamType::FeatureVector :
		for (size_t labelIndex = 0; labelIndex < m_DimensionLabels.size(); labelIndex++)
		{
			addColumn(m_DimensionLabels[labelIndex]);
		}
		break;
	case EStreamType::CovarianceMatrix :
	case EStreamType::StreamedMatrix :
		{
			unsigned int matrixColumns = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1U, std::multiplies<unsigned int>());
			if (matrixColumns == 0)
			{
				m_LastStringError.clear();
				m_LogError = LogErrorCodes_DimensionSizeZero;
				return invalidHeader;
			}
			std::vector<unsigned int> position(m_DimensionCount, 0);
			m_ColumnCount += matrixColumns;
			do
			{
				header += s_Separator;
				unsigned int previousDimensionsSize = 0;
				for (size_t index = 0; index < position.size(); index++)
				{
					header += m_DimensionLabels[previousDimensionsSize + position[index]];
					previousDimensionsSize += m_DimensionSizes[index];
					if ((index + 1) < position.size())
					{
						header += s_InternalDataSeparator;
					}
				}
			} while (increasePositionIndexes(position));
			break;
		}
	case EStreamType::Spectrum :
		for (size_t channel = 0; channel < m_DimensionLabels.size(); channel++)
		{
			for (unsigned int column = 0; column < m_FrequencyAbscissa.size(); column++)
			{
				addColumn(m_DimensionLabels[channel] + s_InternalDataSeparator + std::to_string(m_FrequencyAbscissa[column]));
			}
		}
		break;
	}

	addColumn("Event Id");
	addColumn("Event Date");
	addColumn("Event Duration\n");
	return header;
}

bool CCSVHandler::createCSVStringFromData(bool canWriteAll)
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
		m_ColumnCount += m_PreDataColumnCount + m_PostDataColumnCount; // Will be set correctly with call to setFormatType
	}
	// loop will add a line to the buffer while the last stimulation date registered is greater than the end of the current chunk or until their is an event
	unsigned long long linesWritten = 0;
	while (!m_Chunks.empty() && (
	           canWriteAll
	           || (m_Stimulations.empty() && m_Chunks.front().endTime <= m_NoEventSince)
	           || (!m_Stimulations.empty() && m_Chunks.front().startTime <= m_Stimulations.back().stimulationDate))
	       )
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
			&& (m_Chunks.front().matrix.size() + m_PreDataColumnCount + m_PostDataColumnCount) != m_ColumnCount)
		{
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_WrongLineSize;
			return false;
		}

		if (m_InputTypeIdentifier == EStreamType::FeatureVector
			|| m_InputTypeIdentifier == EStreamType::CovarianceMatrix
			|| m_InputTypeIdentifier == EStreamType::StreamedMatrix)
		{
			unsigned int columnstoHave = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1U, std::multiplies<unsigned int>());
			columnstoHave += m_PreDataColumnCount + m_PostDataColumnCount;
			if (columnstoHave != m_ColumnCount)
			{
				m_LastStringError = "Line size is " + std::to_string(columnstoHave) + " but must be " + std::to_string(m_ColumnCount);
				m_LogError = LogErrorCodes_WrongLineSize;
				return false;
			}
		}
		// time and epoch

		std::pair<double, double> currentTime = { m_Chunks.front().startTime, m_Chunks.front().endTime };
		char buffer[1024] = {};

		sprintf(buffer, "%.*f", m_OutputFloatPrecision, currentTime.first);
		m_Buffer += buffer;
		switch(m_InputTypeIdentifier)
		{
		case EStreamType::Spectrum:
		case EStreamType::StreamedMatrix:
		case EStreamType::CovarianceMatrix:
		case EStreamType::FeatureVector:
			sprintf(buffer, "%.*f", m_OutputFloatPrecision, currentTime.second);
			m_Buffer += s_Separator;
			m_Buffer += buffer;
			break;
		case EStreamType::Signal :
			m_Buffer += s_Separator;
			m_Buffer += std::to_string(m_Chunks.front().epoch);
			break;
		}

		// matrix
		while (!m_Chunks.front().matrix.empty())
		{
			m_Buffer += s_Separator;
			sprintf(buffer, "%.*f", m_OutputFloatPrecision, m_Chunks.front().matrix.front());
			m_Buffer += buffer;
			m_Chunks.front().matrix.erase(m_Chunks.front().matrix.begin());
		}

		m_Buffer += s_Separator;
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
			m_Buffer += s_Separator;
			m_Buffer += s_Separator;
		}
		m_Buffer += "\n";
		linesWritten++;
		m_Chunks.erase(m_Chunks.begin());
	}
	return true;
}

bool CCSVHandler::parseHeader(void)
{
	if (m_IsHeaderRead)
	{
		return true;
	}

	std::string header;
	m_Fs.seekg(0);
	if (!m_GetLineFunction(m_Fs, header, '\n'))
	{
		m_LastStringError = "No header in the file or file empty";
		m_LogError = LogErrorCodes_EmptyColumn;
		return false;
	}
	m_IsHeaderRead = true;

	this->split(header, s_Separator, m_LineColumns);
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
			m_LastStringError.clear();
			m_LogError = LogErrorCodes_EmptyColumn;
			return false;
		}
	}

	if (!m_HasInputType)
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
	if (m_LineColumns[m_LineColumns.size() - m_PostDataColumnCount] != s_EventIdentifierColumn
		|| m_LineColumns[m_LineColumns.size() - m_PreDataColumnCount] != s_EventDateColumn
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

bool CCSVHandler::parseSignalHeader(void)
{
	//check time
	if (m_LineColumns[s_TimeColumnIndex].substr(0, 5) != "Time:"
		|| m_LineColumns[s_TimeColumnIndex].substr(m_LineColumns[s_TimeColumnIndex].size() - 2) != "Hz"
		|| m_LineColumns[s_TimeColumnIndex].size() <= 7)
	{
		m_LastStringError = "First column (" + m_LineColumns[s_TimeColumnIndex] + ") is not well formed";
		return false;
	}

	// get sampling rate
	try
	{
		m_SamplingRate = stoul(m_LineColumns[s_TimeColumnIndex].substr(5, m_LineColumns[s_TimeColumnIndex].size() - 3));
	}
	catch (const std::exception& ia)
	{
		m_LastStringError = "On entry \"" + m_LineColumns[0].substr(5, m_LineColumns[0].size() - 3) + "\", exception have been thrown: ";
		m_LastStringError += ia.what();
		return false;
	}

	// check epoch
	if (m_LineColumns[s_SignalEpochColumnIndex] != "Epoch")
	{
		m_LastStringError = "Second column (" + m_LineColumns[s_SignalEpochColumnIndex] + ") must be Epoch column";
		return false;
	}

	// get dimension labels
	for (unsigned int index = m_PreDataColumnCount; index < m_LineColumns.size() - m_PostDataColumnCount; index++)
	{
		m_DimensionLabels.push_back(m_LineColumns[index]);
	}

	if (!calculateSampleCountPerBuffer()) // m_LastErrorString set in the function, m_LogError set outside the function
	{
		return false;
	}

	return true;
}

bool CCSVHandler::parseSpectrumHeader(void)
{
	m_DimensionSizes.clear();
	// check time column
	std::string buffer;
	std::istringstream iss(m_LineColumns[s_TimeColumnIndex]);
	if (!m_GetLineFunction(iss, buffer, s_InternalDataSeparator))
	{
		m_LastStringError = "First column (" + m_LineColumns[s_TimeColumnIndex] + ") is empty";
		return false;
	}

	if (buffer != "Time")
	{
		m_LastStringError = "First column (" + m_LineColumns[s_TimeColumnIndex] + ") is not well formed, must have \"Time\" written before the first inqternal data separator (\':\' as default)";
		return false;
	}


	auto getNextElem = [&](unsigned int& resultvar, const char separator, const char* missingString)
	{
		if (!m_GetLineFunction(iss, buffer, separator))
		{
			m_LastStringError = "First column (" + m_LineColumns[s_TimeColumnIndex] + ") is not well formed, missing " + missingString;
			return false;
		}
		try
		{
			resultvar = std::stoul(buffer);
		}
		catch (std::exception& e)
		{
			m_LastStringError = "On entry \"" + buffer + "\", exception have been thrown: ";
			m_LastStringError += e.what();
			return false;
		}
		return true;
	};

	unsigned int dimensionSize = 0;
	if (!getNextElem(dimensionSize, s_DimensionSeparator, "channels number"))
	{
		return false;
	}
	m_DimensionSizes.push_back(dimensionSize);

	if (!getNextElem(dimensionSize, s_InternalDataSeparator, "number frequencies per channel"))
	{
		return false;
	}
	m_DimensionSizes.push_back(dimensionSize);

	if (!getNextElem(m_OriginalSampleNumber, s_InternalDataSeparator, "original number of samples"))
	{
		return false;
	}

	if (m_DimensionSizes[0] == 0 || m_DimensionSizes[1] == 0)
	{
		m_LastStringError = "Dimension size must be only positive";
		return false;
	}
	//check Time End column
	if (m_LineColumns[s_EndTimeColumnIndex] != "End Time")
	{
		m_LastStringError = "Second column (" + m_LineColumns[s_EndTimeColumnIndex] + ") must be End Time Column";
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
			std::vector<std::string> spectrumChannel;
			this->split(m_LineColumns.at((labelCounter * m_DimensionSizes[1]) + labelSizeCounter + m_PreDataColumnCount), s_InternalDataSeparator, spectrumChannel);

			if (spectrumChannel.size() != 2)
			{
				m_LastStringError = "Spectrum channel is invalid: " + m_LineColumns.at((labelCounter * m_DimensionSizes[1]) + labelSizeCounter + m_PreDataColumnCount);
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
				m_LastStringError = "Channel name must be the same during " + std::to_string(m_DimensionSizes[1]) + " columns (number of frequencies per channel)";
				return false;
			}

			// get all frequency and check that they're the same for all labels
			dimensionData = spectrumChannel[1];
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

				m_FrequencyAbscissa.push_back(frequency);
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

bool CCSVHandler::parseMatrixHeader(void)
{
	// check time column
	std::istringstream iss(m_LineColumns[s_TimeColumnIndex]);
	std::string linePart;
	// check Time is written
	if (!m_GetLineFunction(iss, linePart, s_InternalDataSeparator))
	{
		m_LastStringError = "First column is empty";
		return false;
	}
	else if (linePart != "Time")
	{
		m_LastStringError = "First column " + m_LineColumns[s_TimeColumnIndex] + " is not well formed";
		return false;
	}

	// check if at least one dimension
	if (!m_GetLineFunction(iss, linePart, s_InternalDataSeparator) || linePart.empty())
	{
		m_LastStringError = "First column must indicate at least one dimension size";
		return false;
	}

	unsigned int size = 0;
	m_DimensionSizes.clear();
	std::vector<std::string> dimensionParts;
	this->split(linePart, s_DimensionSeparator, dimensionParts);
	m_DimensionCount = 0;
	for (const std::string& dimensionSize : dimensionParts)
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
	unsigned int matrixColumnCount = std::accumulate(m_DimensionSizes.begin(), m_DimensionSizes.end(), 1, std::multiplies<unsigned int>());

	if ((matrixColumnCount + m_PreDataColumnCount + m_PostDataColumnCount) != m_LineColumns.size())
	{
		m_LastStringError = "Every line must have " + std::to_string(matrixColumnCount + m_PreDataColumnCount + m_PostDataColumnCount) + " columnLabels";
		return false;
	}

	m_ColumnCount = m_LineColumns.size();

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
	std::vector<unsigned int> positionsInDimensions(m_DimensionCount, 0);
	size_t columnIndex = 0;
	// we will visit each column containing matrix labels
	do
	{
		std::vector<std::string> columnLabels;
		// get all column part
		this->split(m_LineColumns[columnIndex + m_PreDataColumnCount], s_InternalDataSeparator, columnLabels);
		if (columnLabels.size() != m_DimensionCount)
		{
			m_LastStringError = "On column " + std::to_string(columnIndex + m_PreDataColumnCount) + "(" + m_LineColumns[columnIndex + m_PreDataColumnCount] + "), there is " + std::to_string(columnLabels.size()) + " label instead of " + std::to_string(m_DimensionCount);
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
				if (labelsInDimensions[dimensionIndex][positionInCurrentDimension].empty())
				{
					filledLabel[dimensionIndex][positionInCurrentDimension] = true;
				}
				// else,there is an error, it means that label is already set
				else
				{
					m_LastStringError = "Error at column " + std::to_string(columnIndex + 1)
						+ " for the label \"" + columnLabels[dimensionIndex]
						+ "\" in dimension " + std::to_string(dimensionIndex + 1)
						+ " is trying to reset label to \"" + columnLabels[dimensionIndex]
						+ "\" that have been already set to \"" + labelsInDimensions[dimensionIndex][positionInCurrentDimension] + "\"";
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
					m_LastStringError = "Error at column " + std::to_string(columnIndex + 1)
						+ " for the label \"" + columnLabels[dimensionIndex]
						+ "\" in dimension " + std::to_string(dimensionIndex + 1)
						+ " is trying to reset label to \"" + columnLabels[dimensionIndex]
						+ "\" that have been already set to \"" + labelsInDimensions[dimensionIndex][positionInCurrentDimension] + "\"";
					m_LogError = LogErrorCodes_WrongHeader;
					return false;
				}
				// if label isn't set, set it
				else if (!(filledLabel[dimensionIndex][positionInCurrentDimension]))
				{
					labelsInDimensions[dimensionIndex][positionInCurrentDimension] = columnLabels[dimensionIndex];
					filledLabel[dimensionIndex][positionInCurrentDimension] = true;
				}
			}
		}

		columnIndex++;
	} while (increasePositionIndexes(positionsInDimensions));
	for (const std::vector<std::string>& dimensionIndex : labelsInDimensions)
	{
		for (const std::string& label : dimensionIndex)
		{
			m_DimensionLabels.push_back(label);
		}
	}
	m_SampleCountPerBuffer = 1;
	return true;
}

bool CCSVHandler::readSampleChunk(SMatrixChunk& sample, unsigned long long line)
{
	double startTime;

	auto getVar = [this](double& resultVar, const std::string& varToRead, const std::string& errorLocationStr){
		try
		{
			resultVar = std::stod(varToRead);
		}
		catch (const std::invalid_argument& ia)
		{
			m_LastStringError = "Invalid argument error on " + errorLocationStr + " : ";
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
		return true;
	};
	if (!getVar(startTime, m_LineColumns[s_TimeColumnIndex], "start time"))
	{
		return false;
	}


	if (line % m_SampleCountPerBuffer == 0)
	{
		sample.startTime = startTime;
	}

	if (m_InputTypeIdentifier == EStreamType::Signal)
	{
		sample.endTime = sample.startTime + (static_cast<double>(m_SampleCountPerBuffer) / static_cast<double>(m_SamplingRate));
		if (m_LineColumns[s_SignalEpochColumnIndex].empty())
		{
			m_LastStringError = "Missing data in Epoch column";
			m_LogError = LogErrorCodes_MissingData;
			return false;
		}
		try
		{
			sample.epoch = std::stoull(m_LineColumns[s_SignalEpochColumnIndex].c_str());
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
		sample.epoch = std::numeric_limits<unsigned long long>::max();
		if (m_LineColumns[s_TimeColumnIndex].empty())
		{
			m_LastStringError = "Missing data in End Time column";
			m_LogError = LogErrorCodes_MissingData;
			return false;
		}

		if (!getVar(sample.endTime, m_LineColumns[s_EndTimeColumnIndex], "end time"))
		{
			return false;
		}
	}

	if (m_InputTypeIdentifier == EStreamType::Signal || m_InputTypeIdentifier == EStreamType::Spectrum)
	{
		for (unsigned int index = m_PreDataColumnCount; index < (m_LineColumns.size() - m_PostDataColumnCount); index++)
		{
			if (!m_LineColumns[index].empty())
			{
				double matrixValue;
				if (!getVar(matrixValue, m_LineColumns[index], "matrix value on column " + std::to_string(index)))
				{
					return false;
				}
				sample.matrix[((index - m_PreDataColumnCount) * m_SampleCountPerBuffer) + static_cast<unsigned int>(line)] = matrixValue;
			}
		}
	}
	else
	{
		for (unsigned int index = m_PreDataColumnCount; index < (m_LineColumns.size() - m_PostDataColumnCount); index++)
		{
			double matrixValue;
			if (!getVar(matrixValue, m_LineColumns[index], "matrix value on column " + std::to_string(index)))
			{
				return false;
			}
			sample.matrix.push_back(matrixValue);
		}
	}

	return true;
}

bool CCSVHandler::readStimulationChunk(std::vector<SStimulationChunk>& stimulations, unsigned long long line)
{
	std::vector<unsigned long long> stimIdentifiers;
	// pick all time identifiers for the actual time

	if (!m_LineColumns[m_LineColumns.size() - m_PostDataColumnCount].empty())
	{
		std::vector<std::string> column;
		this->split(m_LineColumns[m_LineColumns.size() - m_PostDataColumnCount], s_InternalDataSeparator, column);
		for (const std::string& idValue : column)
		{
			unsigned long long id;
			try
			{
				id = std::stoull(idValue);
			}
			catch (const std::exception& e)
			{
				m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size() - s_StimulationIdentifierColumnNbr) + " : " + e.what();
				m_LogError = LogErrorCodes_InvalidStimulationArgument;
				return false;
			}

			stimIdentifiers.push_back(id);
		}
	}

	// pick all time dates for the actual time
	std::vector<double> stimDates;
	if (!m_LineColumns[m_LineColumns.size() - s_StimulationIdentifierColumnNbr].empty())
	{
		std::vector<std::string> column;
		this->split(m_LineColumns[m_LineColumns.size() - s_StimulationIdentifierColumnNbr], s_InternalDataSeparator, column);
		if (column.size() != stimIdentifiers.size())
		{
			m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size() - s_StimulationDateColumnNbr) + " : all stimulations columns must have the same number of stimulations";
			m_LogError = LogErrorCodes_InvalidStimulationArgument;
			return false;
		}

		for (const std::string& dateValue : column)
		{
			double date;
			try
			{
				date = std::stod(dateValue);
			}
			catch (const std::exception& e)
			{
				m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size() - s_StimulationDateColumnNbr) + " : " + e.what();
				m_LogError = LogErrorCodes_InvalidStimulationArgument;
				return false;
			}

			if (std::signbit(date))
			{
				m_LastStringError = "Stimulation date is negative: ";
				m_LastStringError += std::to_string(date);
				m_LogError = LogErrorCodes_NegativeStimulation;
				return false;
			}
			stimDates.push_back(date);
		}
	}

	// pick all time durations for the actual time

	std::vector<double> stimDurations;
	if (!m_LineColumns.back().empty())
	{
		std::vector<std::string> column;
		this->split(m_LineColumns.back(), s_InternalDataSeparator, column);
		if (column.size() != stimIdentifiers.size())
		{
			m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size()) + " : all stimulations columns must have the same number of stimulations";
			m_LogError = LogErrorCodes_InvalidStimulationArgument;
			return false;
		}
		for (const std::string& durationValue : column)
		{
			double duration;
			try
			{
				duration = std::stod(durationValue);
			}
			catch (const std::exception& e)
			{
				m_LastStringError = "At line " + std::to_string(line) + " column " + std::to_string(m_LineColumns.size()) + " : " + e.what();
				m_LogError = LogErrorCodes_InvalidStimulationArgument;
				return false;
			}

			if (std::signbit(duration))
			{
				m_LastStringError = "Stimulation duration is negative: ";
				m_LastStringError += std::to_string(duration);
				m_LogError = LogErrorCodes_NegativeStimulation;
				return false;
			}
			stimDurations.push_back(duration);
		}
	}

	if (stimIdentifiers.size() != stimDates.size() || stimIdentifiers.size() != stimDurations.size())
	{
		m_LastStringError = "There is " + std::to_string(stimIdentifiers.size()) + " identifiers, " + std::to_string(stimDates.size()) + " dates, and " + std::to_string(stimDurations.size()) + " durations";
		m_LogError = LogErrorCodes_StimulationSize;
		return false;
	}

	for (size_t index = 0; index < stimIdentifiers.size(); index++)
	{
		stimulations.push_back({ stimIdentifiers[index], stimDates[index], stimDurations[index] });
	}
	return true;
}

bool CCSVHandler::increasePositionIndexes(std::vector<unsigned int>& position)
{
	position.back()++;
	for (size_t counter = 1; counter <= position.size(); counter++)
	{
		size_t index = position.size() - counter;
		if ((position[index] + 1) > m_DimensionSizes[index])
		{
			if (index != 0)
			{
				position[index] = 0;
				position[index - 1]++;
			}
			else if ((position[index] + 1) > m_DimensionSizes[index])
			{
				return false;
			}
		}
	}
	return true;
}

bool CCSVHandler::calculateSampleCountPerBuffer()
{
	// get samples per buffer

	std::fstream::pos_type posEndHeader = m_Fs.tellg();
	std::vector<std::string> lineParts({ "", "0" });
	unsigned int sampleCount = 0;
	while (lineParts[s_SignalEpochColumnIndex] == "0")
	{
		std::string line;
		if (!m_GetLineFunction(m_Fs, line, '\n'))
		{
			// protect against sampleCount--, no need here
			sampleCount++;
			break;
		}

		lineParts.clear();
		this->split(line, s_Separator, lineParts);
		if (lineParts.size() != m_LineColumns.size())
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
	m_Fs.seekg(posEndHeader);
	return true;
}

CSV_API ICSVHandler* OpenViBE::CSV::createCSVHandler()
{
	return new CCSVHandler();
}

CSV_API void OpenViBE::CSV::releaseCSVHandler(ICSVHandler* object)
{
	delete dynamic_cast<CCSVHandler*>(object);
}
