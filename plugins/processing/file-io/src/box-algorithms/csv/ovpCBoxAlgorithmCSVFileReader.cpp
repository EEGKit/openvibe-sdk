#include "ovpCBoxAlgorithmCSVFileReader.h"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>  // std::ceil() on Linux

#include <openvibe/ovTimeArithmetics.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace FileIO;
using namespace std;

namespace
{
	vector<string> split(const string& sString, const string& c)
	{
		vector<string> result;
		size_t i = 0;
		size_t j;
		while ((j = sString.find(c, i)) != string::npos)
		{
			result.push_back(string(sString, i, j - i));
			i = j + c.size();
		}
		//the last element without the \n character
		result.push_back(string(sString, i, sString.size() - 1 - i));

		return result;
	}

	void clearMatrix(vector<vector<string>>& vMatrix)
	{
		for (size_t i = 0; i < vMatrix.size(); ++i) { vMatrix[i].clear(); }
		vMatrix.clear();
	}
} // namespace

bool CBoxAlgorithmCSVFileReader::initialize()
{
	m_sampling = 0;
	m_encoder  = nullptr;

	this->getStaticBoxContext().getOutputType(0, m_typeID);

	m_sFilename         = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const CString token = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_sSeparator        = token.toASCIIString();
	m_bDoNotUseFileTime = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_samplesPerBuffer  = 1;
	if (m_typeID == OV_TypeId_ChannelLocalisation) { m_channelsPerBuffer = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3); }
	else if (m_typeID != OV_TypeId_Stimulations && m_typeID != OV_TypeId_Spectrum)
	{
		m_samplesPerBuffer = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	}

	m_nextTime = 0.;

	m_chunkStartTime = 0;
	m_chunkEndTime   = 0;

	return true;
}

bool CBoxAlgorithmCSVFileReader::uninitialize()
{
	if (m_file)
	{
		fclose(m_file);
		m_file = nullptr;
	}
	if (m_encoder)
	{
		m_encoder->uninitialize();
		delete m_encoder;
		m_encoder = nullptr;
	}
	return true;
}

bool CBoxAlgorithmCSVFileReader::initializeFile()
{
	//open file,  we don't open as binary as that gives us \r\n on Windows as line-endings and leaves a dangling char after split. CSV files should be text.
	m_file = fopen(m_sFilename.toASCIIString(), "r");

	OV_ERROR_UNLESS_KRF(m_file, "Error opening file [" << m_sFilename << "] for reading", OpenViBE::Kernel::ErrorType::BadFileRead);

	// simulate RAII through closure
	const auto releaseResources = [&]()
	{
		fclose(m_file);
		m_file = nullptr;
	};

	//read the header
	char line[BUFFER_LEN];
	char* result = fgets(line, BUFFER_LEN, m_file);
	if (nullptr == result)
	{
		releaseResources();
		OV_ERROR_KRF("Error reading data from file", ErrorType::BadParsing);
	}

	m_vHeaderFile = split(string(line), m_sSeparator);
	m_nCol        = m_vHeaderFile.size();

	if (m_typeID == OV_TypeId_ChannelLocalisation)
	{
		m_encoder = new OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		//number of column without the column contains the dynamic parameter
		//m_nCol-=1;
		m_fpRealProcess = &CBoxAlgorithmCSVFileReader::processChannelLocalisation;
	}
	else if (m_typeID == OV_TypeId_FeatureVector)
	{
		m_encoder          = new OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess    = &CBoxAlgorithmCSVFileReader::processFeatureVector;
		m_samplesPerBuffer = 1;
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		m_encoder       = new OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess = &CBoxAlgorithmCSVFileReader::processSpectrum;

		//number of column without columns contains min max frequency bands parameters
		m_nCol -= 2;
	}
	else if (m_typeID == OV_TypeId_Signal)
	{
		m_encoder       = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess = &CBoxAlgorithmCSVFileReader::processSignal;

		//find the sampling rate
		result = fgets(line, BUFFER_LEN, m_file);

		if (nullptr == result)
		{
			releaseResources();
			OV_ERROR_KRF("Error reading sampling rate from file", ErrorType::BadParsing);
		}

		vector<string> parsed = split(string(line), m_sSeparator);

		if ((m_nCol - 1) >= parsed.size())
		{
			releaseResources();
			OV_ERROR_KRF("Error reading columns (not enough columns found) from file", ErrorType::BadParsing);
		}

		const double sampling = double(atof(parsed[m_nCol - 1].c_str()));
		if (ceil(sampling) != sampling)
		{
			releaseResources();
			OV_ERROR_KRF("Invalid fractional sampling rate (" << sampling << ") in file", ErrorType::BadValue);
		}

		m_sampling = uint64_t(sampling);

		if (m_sampling == 0)
		{
			releaseResources();
			OV_ERROR_KRF("Invalid NULL sampling rate in file", ErrorType::BadValue);
		}

		// Skip the header
		rewind(m_file);
		result = fgets(line, BUFFER_LEN, m_file);
		if (nullptr == result)
		{
			releaseResources();
			OV_ERROR_KRF("Error reading data from file", ErrorType::BadParsing);
		}

		//number of column without the column contains the sampling rate parameters
		m_nCol -= 1;
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix)
	{
		m_encoder       = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess = &CBoxAlgorithmCSVFileReader::processStreamedMatrix;
	}
	else if (m_typeID == OV_TypeId_Stimulations)
	{
		m_encoder       = new OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess = &CBoxAlgorithmCSVFileReader::processStimulation;
	}
	else
	{
		releaseResources();
		OV_ERROR_KRF("Invalid input type identifier " << this->getTypeManager().getTypeName(m_typeID) << " in file ", ErrorType::BadValue);
	}

	return true;
}

bool CBoxAlgorithmCSVFileReader::processClock(IMessageClock& /*messageClock*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmCSVFileReader::process()
{
	if (m_file == nullptr) { OV_ERROR_UNLESS_KRF(initializeFile(), "Error reading data from csv file " << m_sFilename, ErrorType::Internal); }
	//line buffer
	char line[BUFFER_LEN];
	const double currentTime = TimeArithmetics::timeToSeconds(getPlayerContext().getCurrentTime());
	//IBoxIO& boxContext=this->getDynamicBoxContext();

	//if no line was read, read the first data line.
	if (m_vLastLineSplit.empty())
	{
		//next line
		size_t nSamples = 0;
		while (!feof(m_file) && nSamples < m_samplesPerBuffer && fgets(line, BUFFER_LEN, m_file) != nullptr)
		{
			m_vLastLineSplit = split(string(line), m_sSeparator);

			nSamples++;

			if (m_typeID != OV_TypeId_Stimulations
				&& m_typeID != OV_TypeId_Spectrum
				&& m_typeID != OV_TypeId_ChannelLocalisation) { m_vDataMatrix.push_back(m_vLastLineSplit); }
		}
		if ((m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_Signal)
			&& feof(m_file) && nSamples < m_samplesPerBuffer)
		{
			// Last chunk will be partial, zero the whole output matrix...
			IMatrix* iMatrix = static_cast<OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputMatrix();
			OpenViBEToolkit::Tools::Matrix::clearContent(*iMatrix);
		}
	}

	bool somethingToSend = (!m_vLastLineSplit.empty()) && atof(m_vLastLineSplit[0].c_str()) < currentTime;
	somethingToSend |= (m_typeID == OV_TypeId_Stimulations); // we always send a stim chunk, even if empty

	if (m_typeID == OV_TypeId_Stimulations || m_typeID == OV_TypeId_ChannelLocalisation || m_typeID == OV_TypeId_Spectrum)
	{
		while (!m_vLastLineSplit.empty() && atof(m_vLastLineSplit[0].c_str()) < currentTime)
		{
			m_vDataMatrix.push_back(m_vLastLineSplit);

			somethingToSend = true;

			if (!feof(m_file) && fgets(line, BUFFER_LEN, m_file) != nullptr) { m_vLastLineSplit = split(string(line), m_sSeparator); }
			else { m_vLastLineSplit.clear(); }
		}
	}

	//convert data to the good output type

	if (somethingToSend)
	{
		// Encode the data
		OV_ERROR_UNLESS_KRF((this->*m_fpRealProcess)(), "Error encoding data from csv file " << m_sFilename << " into the right output format",
							ErrorType::Internal);

		//for the stimulation, the line contents in m_vLastLineSplit isn't processed.
		if (m_typeID != OV_TypeId_Stimulations
			&& m_typeID != OV_TypeId_Spectrum
			&& m_typeID != OV_TypeId_ChannelLocalisation) { m_vLastLineSplit.clear(); }

		//clear the Data Matrix.
		clearMatrix(m_vDataMatrix);
	}
	return true;
}

bool CBoxAlgorithmCSVFileReader::processStreamedMatrix()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	IMatrix* iMatrix   = static_cast<OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputMatrix();

	//Header
	if (!m_headerSent)
	{
		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nCol - 1);
		iMatrix->setDimensionSize(1, m_samplesPerBuffer);

		for (size_t i = 1; i < m_nCol; ++i) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }
		m_encoder->encodeHeader();
		m_headerSent = true;

		boxContext.markOutputAsReadyToSend(0, 0, 0);
	}

	OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to streamed matrix", ErrorType::Internal);

	m_encoder->encodeBuffer();

	if (m_bDoNotUseFileTime)
	{
		m_chunkStartTime = m_chunkEndTime;
		m_chunkEndTime   = this->getPlayerContext().getCurrentTime();
	}
	else
	{
		m_chunkStartTime = TimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str()));
		m_chunkEndTime   = TimeArithmetics::secondsToTime(atof(m_vDataMatrix.back()[0].c_str()));
	}

	boxContext.markOutputAsReadyToSend(0, m_chunkStartTime, m_chunkEndTime);

	return true;
}

bool CBoxAlgorithmCSVFileReader::processStimulation()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	//Header
	if (!m_headerSent)
	{
		m_encoder->encodeHeader();
		m_headerSent = true;

		boxContext.markOutputAsReadyToSend(0, 0, 0);
	}

	IStimulationSet* ip_pStimulationSet = static_cast<OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->
			getInputStimulationSet();
	ip_pStimulationSet->clear();

	for (size_t i = 0; i < m_vDataMatrix.size(); ++i)
	{
		OV_ERROR_UNLESS_KRF(m_vDataMatrix[i].size() == 3, "Invalid data row length: must be 3 for stimulation date, index and duration", ErrorType::BadParsing);

		//stimulation date
		const uint64_t date = TimeArithmetics::secondsToTime(atof(m_vDataMatrix[i][0].c_str()));

		//stimulation indices
		const uint64_t id = uint64_t(atof(m_vDataMatrix[i][1].c_str()));

		//stimulation duration
		const uint64_t duration = TimeArithmetics::secondsToTime(atof(m_vDataMatrix[i][2].c_str()));

		ip_pStimulationSet->appendStimulation(id, date, duration);
	}

	m_encoder->encodeBuffer();

	// Never use file time
	m_chunkStartTime = m_chunkEndTime;
	m_chunkEndTime   = this->getPlayerContext().getCurrentTime();

	boxContext.markOutputAsReadyToSend(0, m_chunkStartTime, m_chunkEndTime);

	return true;
}

bool CBoxAlgorithmCSVFileReader::processSignal()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	IMatrix* iMatrix   = static_cast<OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputMatrix();

	//Header
	if (!m_headerSent)
	{
		// This is the first chunk, find out the start time from the file
		// (to keep time chunks continuous, start time is previous end time, hence set end time)
		if (!m_bDoNotUseFileTime) { m_chunkEndTime = TimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str())); }

		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nCol - 1);
		iMatrix->setDimensionSize(1, m_samplesPerBuffer);

		for (size_t i = 1; i < m_nCol; ++i) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }

		static_cast<OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputSamplingRate() = m_sampling;

		m_encoder->encodeHeader();
		m_headerSent = true;

		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
	}

	OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to signal", ErrorType::Internal);

	// this->getLogManager() << LogLevel_Info << "Cols from header " << m_nCol << "\n";
	// this->getLogManager() << LogLevel_Info << "InMatrix " << (m_vDataMatrix.size() > 0 ? m_vDataMatrix[0].size() : 0) << " outMatrix " << iMatrix->getDimensionSize(0) << "\n";

	m_encoder->encodeBuffer();

	if (m_bDoNotUseFileTime)
	{
		// We use time dictated by the sampling rate
		m_chunkStartTime = m_chunkEndTime; // previous time end is current time start
		m_chunkEndTime   = m_chunkStartTime + TimeArithmetics::sampleCountToTime(m_sampling, m_samplesPerBuffer);
	}
	else
	{
		// We use time suggested by the last sample of the chunk
		m_chunkStartTime = TimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str()));
		m_chunkEndTime   = TimeArithmetics::secondsToTime(atof(m_vDataMatrix.back()[0].c_str()));
	}

	boxContext.markOutputAsReadyToSend(0, m_chunkStartTime, m_chunkEndTime);

	return true;
}

bool CBoxAlgorithmCSVFileReader::processChannelLocalisation()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	IMatrix* iMatrix   = static_cast<OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputMatrix();

	if (!m_headerSent)
	{
		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nCol - 1);
		iMatrix->setDimensionSize(1, m_samplesPerBuffer);

		for (size_t i = 1; i < m_nCol; ++i) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }

		static_cast<OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputDynamic() =
				false;	//atoi(m_vDataMatrix[0][m_nCol].c_str());

		m_encoder->encodeHeader();

		boxContext.markOutputAsReadyToSend(0, 0, 0);

		m_headerSent = true;
	}

	vector<vector<string>> channelBloc;
	for (size_t i = 0; i < m_vDataMatrix.size(); ++i) { channelBloc.push_back(m_vDataMatrix[i]); }

	//clear matrix
	clearMatrix(m_vDataMatrix);

	for (size_t i = 0; i < channelBloc.size(); ++i)
	{
		m_vDataMatrix.push_back(channelBloc[i]);

		//send the current bloc if the next data hasn't the same date
		if (i >= channelBloc.size() - 1 || channelBloc[(i + 1)][0] != m_vDataMatrix[0][0])
		{
			OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to channel localisation", ErrorType::Internal);

			m_encoder->encodeBuffer();
			const uint64_t date = TimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str()));
			boxContext.markOutputAsReadyToSend(0, date, date);

			//clear matrix
			clearMatrix(m_vDataMatrix);
		}
	}

	//clear matrix
	clearMatrix(channelBloc);

	return true;
}

bool CBoxAlgorithmCSVFileReader::processFeatureVector()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	IMatrix* matrix    = static_cast<OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputMatrix();

	//Header
	if (!m_headerSent)
	{
		// in this case we need to transpose it
		IMatrix* iMatrix = static_cast<OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputMatrix();

		iMatrix->setDimensionCount(1);
		iMatrix->setDimensionSize(0, m_nCol - 1);

		for (size_t i = 1; i < m_nCol; ++i) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }

		m_encoder->encodeHeader();

		boxContext.markOutputAsReadyToSend(0, 0, 0);

		m_headerSent = true;
	}

	// Each vector has to be sent separately
	for (size_t i = 0; i < m_vDataMatrix.size(); ++i)
	{
		OV_ERROR_UNLESS_KRF(m_vDataMatrix[i].size() == m_nCol,
							"Unexpected number of elements" << "(got " << uint64_t(m_vDataMatrix[i].size()) << ", expected " << m_nCol << ")",
							ErrorType::BadParsing);

		for (size_t j = 0; j < m_nCol - 1; ++j) { matrix->getBuffer()[j] = atof(m_vDataMatrix[i][j + 1].c_str()); }

		m_encoder->encodeBuffer();

		const uint64_t date = TimeArithmetics::secondsToTime(atof(m_vDataMatrix[i][0].c_str()));
		boxContext.markOutputAsReadyToSend(0, date, date);
	}

	clearMatrix(m_vDataMatrix);

	return true;
}

bool CBoxAlgorithmCSVFileReader::processSpectrum()
{
	IBoxIO& boxContext          = this->getDynamicBoxContext();
	IMatrix* iMatrix            = static_cast<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputMatrix();
	IMatrix* iFrequencyAbscissa = static_cast<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputFrequencyAbscissa();

	//Header
	if (!m_headerSent)
	{
		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nCol - 1);
		iMatrix->setDimensionSize(1, m_vDataMatrix.size());

		for (size_t i = 1; i < m_nCol; ++i) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }
		iFrequencyAbscissa->setDimensionCount(1);
		iFrequencyAbscissa->setDimensionSize(0, m_vDataMatrix.size());
		if (m_vDataMatrix.size() > 1)
		{
			for (size_t i = 0; i < m_vDataMatrix.size(); ++i)
			{
				const double curFrequencyAbscissa = std::stod(m_vDataMatrix[i][m_nCol]) + double(i) / (m_vDataMatrix.size() - 1)
													* (std::stod(m_vDataMatrix[i][m_nCol + 1]) - std::stod(m_vDataMatrix[i][m_nCol]));
				iFrequencyAbscissa->getBuffer()[i] = curFrequencyAbscissa;

				stringstream label;
				label << curFrequencyAbscissa;
				iFrequencyAbscissa->setDimensionLabel(0, i, label.str().c_str());
			}
		}
		else { iFrequencyAbscissa->getBuffer()[0] = 0; }

		static_cast<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>*>(m_encoder)->getInputSamplingRate() = uint64_t(
			m_vDataMatrix.size() / (stod(m_vDataMatrix[m_vDataMatrix.size() - 1][m_nCol]) - stod(m_vDataMatrix[0][m_nCol])
			));
		m_headerSent = true;
		m_encoder->encodeHeader();

		boxContext.markOutputAsReadyToSend(0, 0, 0);
	}

	vector<vector<string>> spectrumBloc;
	for (size_t i = 0; i < m_vDataMatrix.size(); ++i) { spectrumBloc.push_back(m_vDataMatrix[i]); }

	//clear matrix
	clearMatrix(m_vDataMatrix);

	for (size_t i = 0; i < spectrumBloc.size(); ++i)
	{
		m_vDataMatrix.push_back(spectrumBloc[i]);
		//send the current bloc if the next data hasn't the same date
		if (i >= spectrumBloc.size() - 1 || spectrumBloc[i + 1][0] != m_vDataMatrix[0][0])
		{
			OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to spectrum", ErrorType::Internal);

			m_encoder->encodeBuffer();
			const uint64_t date = TimeArithmetics::secondsToTime(std::stod(m_vDataMatrix[0][0]));
			boxContext.markOutputAsReadyToSend(0, date - 1, date);

			//clear matrix
			clearMatrix(m_vDataMatrix);
		}
	}

	//clear matrix
	clearMatrix(spectrumBloc);
	return true;
}

bool CBoxAlgorithmCSVFileReader::convertVectorDataToMatrix(IMatrix* matrix)
{
	// note: Chunk size shouldn't change after encoding header, do not mess with it here, even if the input has different size

	// We accept partial data, but not buffer overruns ...
	OV_ERROR_UNLESS_KRF(matrix->getDimensionSize(1) >= m_vDataMatrix.size() && matrix->getDimensionSize(0) >= (m_nCol-1),
						"Matrix size incompatibility, data suggests " << m_nCol-1 << "x" << m_vDataMatrix.size()
						<< ", expected at most " << matrix->getDimensionSize(0) << "x" << matrix->getDimensionSize(0), ErrorType::Overflow);

	stringstream ss;
	for (size_t i = 0; i < m_vDataMatrix.size(); ++i)
	{
		ss << "at time (" << m_vDataMatrix[i][0].c_str() << "):";
		for (size_t j = 0; j < m_nCol - 1; ++j)
		{
			matrix->getBuffer()[j * matrix->getDimensionSize(1) + i] = std::stod(m_vDataMatrix[i][j + 1]);
			ss << matrix->getBuffer()[j * matrix->getDimensionSize(1) + i] << ";";
		}
		ss << "\n";
	}
	getLogManager() << LogLevel_Debug << "Matrix:\n" << ss.str();
	getLogManager() << LogLevel_Debug << "Matrix:\n" << ss.str();

	return true;
}
