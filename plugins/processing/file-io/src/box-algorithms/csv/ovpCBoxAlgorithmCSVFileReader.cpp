#include "ovpCBoxAlgorithmCSVFileReader.h"
#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <cmath>  // std::ceil() on Linux

#include <openvibe/ovITimeArithmetics.h>

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
		for (uint32_t i = 0; i < vMatrix.size(); i++) { vMatrix[i].clear(); }
		vMatrix.clear();
	}
}

CBoxAlgorithmCSVFileReader::CBoxAlgorithmCSVFileReader() : m_fpRealProcess(nullptr) {}

uint64_t CBoxAlgorithmCSVFileReader::getClockFrequency()
{
	return 128LL << 32; // the box clock frequency
}

bool CBoxAlgorithmCSVFileReader::initialize()
{
	m_ui64SamplingRate  = 0;
	m_pAlgorithmEncoder = nullptr;

	this->getStaticBoxContext().getOutputType(0, m_oTypeIdentifier);

	m_sFilename            = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const CString token    = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_sSeparator           = token.toASCIIString();
	m_bDoNotUseFileTime    = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_ui32SamplesPerBuffer = 1;
	if (m_oTypeIdentifier == OV_TypeId_ChannelLocalisation) { m_ui32ChannelNumberPerBuffer = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3); }
	else if (m_oTypeIdentifier != OV_TypeId_Stimulations && m_oTypeIdentifier != OV_TypeId_Spectrum)
	{
		m_ui32SamplesPerBuffer = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	}

	m_f64NextTime = 0.;

	m_ui64ChunkStartTime = 0;
	m_ui64ChunkEndTime   = 0;

	return true;
}

bool CBoxAlgorithmCSVFileReader::uninitialize()
{
	if (m_pFile)
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	}
	if (m_pAlgorithmEncoder)
	{
		m_pAlgorithmEncoder->uninitialize();
		delete m_pAlgorithmEncoder;
		m_pAlgorithmEncoder = nullptr;
	}
	return true;
}

bool CBoxAlgorithmCSVFileReader::initializeFile()
{
	//open file
	m_pFile =
			fopen(m_sFilename.toASCIIString(),
				  "r"); // we don't open as binary as that gives us \r\n on Windows as line-endings and leaves a dangling char after split. CSV files should be text.

	OV_ERROR_UNLESS_KRF(m_pFile, "Error opening file [" << m_sFilename << "] for reading", OpenViBE::Kernel::ErrorType::BadFileRead);

	// simulate RAII through closure
	const auto releaseResources = [&]()
	{
		fclose(m_pFile);
		m_pFile = nullptr;
	};

	//read the header
	char line[m_ui32bufferLen];
	char* result = fgets(line, m_ui32bufferLen, m_pFile);
	if (nullptr == result)
	{
		releaseResources();
		OV_ERROR_KRF("Error reading data from file", ErrorType::BadParsing);
	}

	m_vHeaderFile     = split(string(line), m_sSeparator);
	m_nColumn = m_vHeaderFile.size();

	if (m_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
	{
		m_pAlgorithmEncoder = new OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		//number of column without the column contains the dynamic parameter
		//m_ui32NbColumn-=1;
		m_fpRealProcess = &CBoxAlgorithmCSVFileReader::process_channelLocalisation;
	}
	else if (m_oTypeIdentifier == OV_TypeId_FeatureVector)
	{
		m_pAlgorithmEncoder    = new OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess        = &CBoxAlgorithmCSVFileReader::process_featureVector;
		m_ui32SamplesPerBuffer = 1;
	}
	else if (m_oTypeIdentifier == OV_TypeId_Spectrum)
	{
		m_pAlgorithmEncoder = new OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess     = &CBoxAlgorithmCSVFileReader::process_spectrum;

		//number of column without columns contains min max frequency bands parameters
		m_nColumn -= 2;
	}
	else if (m_oTypeIdentifier == OV_TypeId_Signal)
	{
		m_pAlgorithmEncoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess     = &CBoxAlgorithmCSVFileReader::process_signal;

		//find the sampling rate
		result = fgets(line, m_ui32bufferLen, m_pFile);

		if (nullptr == result)
		{
			releaseResources();
			OV_ERROR_KRF("Error reading sampling rate from file", ErrorType::BadParsing);
		}

		vector<string> l_vParsed = split(string(line), m_sSeparator);

		if ((m_nColumn - 1) >= l_vParsed.size())
		{
			releaseResources();
			OV_ERROR_KRF("Error reading columns (not enough columns found) from file", ErrorType::BadParsing);
		}

		const double l_f64SamplingRate = double(atof(l_vParsed[m_nColumn - 1].c_str()));
		if (ceil(l_f64SamplingRate) != l_f64SamplingRate)
		{
			releaseResources();
			OV_ERROR_KRF("Invalid fractional sampling rate (" << l_f64SamplingRate << ") in file", ErrorType::BadValue);
		}

		m_ui64SamplingRate = uint64_t(l_f64SamplingRate);

		if (m_ui64SamplingRate == 0)
		{
			releaseResources();
			OV_ERROR_KRF("Invalid NULL sampling rate in file", ErrorType::BadValue);
		}

		// Skip the header
		rewind(m_pFile);
		result = fgets(line, m_ui32bufferLen, m_pFile);
		if (nullptr == result)
		{
			releaseResources();
			OV_ERROR_KRF("Error reading data from file", ErrorType::BadParsing);
		}

		//number of column without the column contains the sampling rate parameters
		m_nColumn -= 1;
	}
	else if (m_oTypeIdentifier == OV_TypeId_StreamedMatrix)
	{
		m_pAlgorithmEncoder = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess     = &CBoxAlgorithmCSVFileReader::process_streamedMatrix;
	}
	else if (m_oTypeIdentifier == OV_TypeId_Stimulations)
	{
		m_pAlgorithmEncoder = new OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmCSVFileReader>(*this, 0);
		m_fpRealProcess     = &CBoxAlgorithmCSVFileReader::process_stimulation;
	}
	else
	{
		releaseResources();
		OV_ERROR_KRF("Invalid input type identifier " << this->getTypeManager().getTypeName(m_oTypeIdentifier) << " in file ", ErrorType::BadValue);
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
	if (m_pFile == nullptr) { OV_ERROR_UNLESS_KRF(initializeFile(), "Error reading data from csv file " << m_sFilename, ErrorType::Internal); }
	//line buffer
	char l_pLine[m_ui32bufferLen];
	const double l_f64currentTime = ITimeArithmetics::timeToSeconds(getPlayerContext().getCurrentTime());
	//IBoxIO& boxContext=this->getDynamicBoxContext();

	//if no line was read, read the first data line.
	if (m_vLastLineSplit.empty())
	{
		//next line
		uint32_t nSamples = 0;
		while (!feof(m_pFile) && nSamples < m_ui32SamplesPerBuffer && fgets(l_pLine, m_ui32bufferLen, m_pFile) != nullptr)
		{
			m_vLastLineSplit = split(string(l_pLine), m_sSeparator);

			nSamples++;

			if (m_oTypeIdentifier != OV_TypeId_Stimulations
				&& m_oTypeIdentifier != OV_TypeId_Spectrum
				&& m_oTypeIdentifier != OV_TypeId_ChannelLocalisation) { m_vDataMatrix.push_back(m_vLastLineSplit); }
		}
		if ((m_oTypeIdentifier == OV_TypeId_StreamedMatrix || m_oTypeIdentifier == OV_TypeId_Signal)
			&& feof(m_pFile) && nSamples < m_ui32SamplesPerBuffer)
		{
			// Last chunk will be partial, zero the whole output matrix...
			IMatrix* iMatrix = static_cast<OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputMatrix();
			OpenViBEToolkit::Tools::Matrix::clearContent(*iMatrix);
		}
	}

	bool somethingToSend = (!m_vLastLineSplit.empty()) && atof(m_vLastLineSplit[0].c_str()) < l_f64currentTime;
	somethingToSend |= (m_oTypeIdentifier == OV_TypeId_Stimulations); // we always send a stim chunk, even if empty

	if (m_oTypeIdentifier == OV_TypeId_Stimulations || m_oTypeIdentifier == OV_TypeId_ChannelLocalisation || m_oTypeIdentifier == OV_TypeId_Spectrum)
	{
		while (!m_vLastLineSplit.empty() && atof(m_vLastLineSplit[0].c_str()) < l_f64currentTime)
		{
			m_vDataMatrix.push_back(m_vLastLineSplit);

			somethingToSend = true;

			if (!feof(m_pFile) && fgets(l_pLine, m_ui32bufferLen, m_pFile) != nullptr) { m_vLastLineSplit = split(string(l_pLine), m_sSeparator); }
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
		if (m_oTypeIdentifier != OV_TypeId_Stimulations
			&& m_oTypeIdentifier != OV_TypeId_Spectrum
			&& m_oTypeIdentifier != OV_TypeId_ChannelLocalisation) { m_vLastLineSplit.clear(); }

		//clear the Data Matrix.
		clearMatrix(m_vDataMatrix);
	}
	return true;
}

bool CBoxAlgorithmCSVFileReader::process_streamedMatrix()
{
	IMatrix* iMatrix = static_cast<OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputMatrix();

	//Header
	if (!m_bHeaderSent)
	{
		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nColumn - 1);
		iMatrix->setDimensionSize(1, m_ui32SamplesPerBuffer);

		for (uint32_t i = 1; i < m_nColumn; i++) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }
		m_pAlgorithmEncoder->encodeHeader();
		m_bHeaderSent = true;

		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
	}

	OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to streamed matrix", ErrorType::Internal);

	m_pAlgorithmEncoder->encodeBuffer();

	if (m_bDoNotUseFileTime)
	{
		m_ui64ChunkStartTime = m_ui64ChunkEndTime;
		m_ui64ChunkEndTime   = this->getPlayerContext().getCurrentTime();
	}
	else
	{
		m_ui64ChunkStartTime = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str()));
		m_ui64ChunkEndTime   = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix.back()[0].c_str()));
	}

	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_ui64ChunkStartTime, m_ui64ChunkEndTime);

	return true;
}

bool CBoxAlgorithmCSVFileReader::process_stimulation()
{
	//Header
	if (!m_bHeaderSent)
	{
		m_pAlgorithmEncoder->encodeHeader();
		m_bHeaderSent = true;

		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
	}

	IStimulationSet* ip_pStimulationSet = static_cast<OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->
			getInputStimulationSet();
	ip_pStimulationSet->clear();

	for (uint32_t i = 0; i < m_vDataMatrix.size(); i++)
	{
		OV_ERROR_UNLESS_KRF(m_vDataMatrix[i].size() == 3, "Invalid data row length: must be 3 for stimulation date, index and duration", ErrorType::BadParsing);

		//stimulation date
		const uint64_t l_ui64StimulationDate = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix[i][0].c_str()));

		//stimulation indices
		const uint64_t l_ui64Stimulation = uint64_t(atof(m_vDataMatrix[i][1].c_str()));

		//stimulation duration
		const uint64_t l_ui64StimulationDuration = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix[i][2].c_str()));

		ip_pStimulationSet->appendStimulation(l_ui64Stimulation, l_ui64StimulationDate, l_ui64StimulationDuration);
	}

	m_pAlgorithmEncoder->encodeBuffer();

	// Never use file time
	m_ui64ChunkStartTime = m_ui64ChunkEndTime;
	m_ui64ChunkEndTime   = this->getPlayerContext().getCurrentTime();

	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_ui64ChunkStartTime, m_ui64ChunkEndTime);

	return true;
}

bool CBoxAlgorithmCSVFileReader::process_signal()
{
	IMatrix* iMatrix = static_cast<OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputMatrix();

	//Header
	if (!m_bHeaderSent)
	{
		// This is the first chunk, find out the start time from the file
		// (to keep time chunks continuous, start time is previous end time, hence set end time)
		if (!m_bDoNotUseFileTime) { m_ui64ChunkEndTime = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str())); }

		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nColumn - 1);
		iMatrix->setDimensionSize(1, m_ui32SamplesPerBuffer);

		for (uint32_t i = 1; i < m_nColumn; i++) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }

		static_cast<OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputSamplingRate() = m_ui64SamplingRate;

		m_pAlgorithmEncoder->encodeHeader();
		m_bHeaderSent = true;

		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
	}

	OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to signal", ErrorType::Internal);

	// this->getLogManager() << LogLevel_Info << "Cols from header " << m_ui32NbColumn << "\n";
	// this->getLogManager() << LogLevel_Info << "InMatrix " << (m_vDataMatrix.size() > 0 ? m_vDataMatrix[0].size() : 0) << " outMatrix " << iMatrix->getDimensionSize(0) << "\n";

	m_pAlgorithmEncoder->encodeBuffer();

	if (m_bDoNotUseFileTime)
	{
		// We use time dictated by the sampling rate
		m_ui64ChunkStartTime = m_ui64ChunkEndTime; // previous time end is current time start
		m_ui64ChunkEndTime   = m_ui64ChunkStartTime + ITimeArithmetics::sampleCountToTime(m_ui64SamplingRate, m_ui32SamplesPerBuffer);
	}
	else
	{
		// We use time suggested by the last sample of the chunk
		m_ui64ChunkStartTime = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str()));
		m_ui64ChunkEndTime   = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix.back()[0].c_str()));
	}

	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_ui64ChunkStartTime, m_ui64ChunkEndTime);

	return true;
}

bool CBoxAlgorithmCSVFileReader::process_channelLocalisation()
{
	IMatrix* iMatrix = static_cast<OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputMatrix();

	if (!m_bHeaderSent)
	{
		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nColumn - 1);
		iMatrix->setDimensionSize(1, m_ui32SamplesPerBuffer);

		for (uint32_t i = 1; i < m_nColumn; i++) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }

		static_cast<OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputDynamic() =
				false;	//atoi(m_vDataMatrix[0][m_ui32NbColumn].c_str());

		m_pAlgorithmEncoder->encodeHeader();

		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);

		m_bHeaderSent = true;
	}

	vector<vector<string>> channelBloc;
	for (uint32_t i = 0; i < m_vDataMatrix.size(); i++) { channelBloc.push_back(m_vDataMatrix[i]); }

	//clear matrix
	clearMatrix(m_vDataMatrix);

	for (size_t i = 0; i < channelBloc.size(); i++)
	{
		m_vDataMatrix.push_back(channelBloc[i]);

		//send the current bloc if the next data hasn't the same date
		if (i >= channelBloc.size() - 1 || channelBloc[(i + 1)][0] != m_vDataMatrix[0][0])
		{
			OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to channel localisation", ErrorType::Internal);

			m_pAlgorithmEncoder->encodeBuffer();
			const uint64_t date = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix[0][0].c_str()));
			this->getDynamicBoxContext().markOutputAsReadyToSend(0, date, date);

			//clear matrix
			clearMatrix(m_vDataMatrix);
		}
	}

	//clear matrix
	clearMatrix(channelBloc);

	return true;
}

bool CBoxAlgorithmCSVFileReader::process_featureVector()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	IMatrix* matrix    = static_cast<OpenViBEToolkit::TFeatureVectorEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputMatrix();

	//Header
	if (!m_bHeaderSent)
	{
		// in this case we need to transpose it
		IMatrix* iMatrix = static_cast<OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputMatrix();

		iMatrix->setDimensionCount(1);
		iMatrix->setDimensionSize(0, m_nColumn - 1);

		for (uint32_t i = 1; i < m_nColumn; i++) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }

		m_pAlgorithmEncoder->encodeHeader();

		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);

		m_bHeaderSent = true;
	}

	// Each vector has to be sent separately
	for (uint32_t i = 0; i < m_vDataMatrix.size(); i++)
	{
		OV_ERROR_UNLESS_KRF(m_vDataMatrix[i].size() == m_nColumn,
							"Unexpected number of elements" << "(got " << uint64_t(m_vDataMatrix[i].size()) << ", expected " << m_nColumn << ")",
							ErrorType::BadParsing);

		for (uint32_t j = 0; j < m_nColumn - 1; j++) { matrix->getBuffer()[j] = atof(m_vDataMatrix[i][j + 1].c_str()); }

		m_pAlgorithmEncoder->encodeBuffer();

		const uint64_t date = ITimeArithmetics::secondsToTime(atof(m_vDataMatrix[i][0].c_str()));
		boxContext.markOutputAsReadyToSend(0, date, date);
	}

	clearMatrix(m_vDataMatrix);

	return true;
}

bool CBoxAlgorithmCSVFileReader::process_spectrum()
{
	IMatrix* iMatrix            = static_cast<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputMatrix();
	IMatrix* iFrequencyAbscissa = static_cast<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputFrequencyAbscissa();

	//Header
	if (!m_bHeaderSent)
	{
		iMatrix->setDimensionCount(2);
		iMatrix->setDimensionSize(0, m_nColumn - 1);
		iMatrix->setDimensionSize(1, m_vDataMatrix.size());

		for (uint32_t i = 1; i < m_nColumn; i++) { iMatrix->setDimensionLabel(0, i - 1, m_vHeaderFile[i].c_str()); }
		iFrequencyAbscissa->setDimensionCount(1);
		iFrequencyAbscissa->setDimensionSize(0, m_vDataMatrix.size());
		if (m_vDataMatrix.size() > 1)
		{
			for (uint32_t frequencyBandIndex = 0; frequencyBandIndex < m_vDataMatrix.size(); frequencyBandIndex++)
			{
				const double curFrequencyAbscissa = std::stod(m_vDataMatrix[frequencyBandIndex][m_nColumn].c_str())
													+ double(frequencyBandIndex) / (m_vDataMatrix.size() - 1) * (
														std::stod(m_vDataMatrix[frequencyBandIndex][m_nColumn + 1].c_str())
														- std::stod(m_vDataMatrix[frequencyBandIndex][m_nColumn].c_str()));
				iFrequencyAbscissa->getBuffer()[frequencyBandIndex] = curFrequencyAbscissa;

				stringstream l_sLabel;
				l_sLabel << curFrequencyAbscissa;
				iFrequencyAbscissa->setDimensionLabel(0, frequencyBandIndex, l_sLabel.str().c_str());
			}
		}
		else { iFrequencyAbscissa->getBuffer()[0] = 0; }

		static_cast<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmCSVFileReader>*>(m_pAlgorithmEncoder)->getInputSamplingRate() = uint64_t(
			m_vDataMatrix.size() / (stod(m_vDataMatrix[m_vDataMatrix.size() - 1][m_nColumn].c_str()) - stod(m_vDataMatrix[0][m_nColumn].c_str())
			));
		m_bHeaderSent = true;
		m_pAlgorithmEncoder->encodeHeader();

		this->getDynamicBoxContext().markOutputAsReadyToSend(0, 0, 0);
	}

	vector<vector<string>> l_vSpectrumBloc;
	for (uint32_t i = 0; i < m_vDataMatrix.size(); i++) { l_vSpectrumBloc.push_back(m_vDataMatrix[i]); }

	//clear matrix
	clearMatrix(m_vDataMatrix);

	for (size_t i = 0; i < l_vSpectrumBloc.size(); i++)
	{
		m_vDataMatrix.push_back(l_vSpectrumBloc[i]);
		//send the current bloc if the next data hasn't the same date
		if (i >= l_vSpectrumBloc.size() - 1 || l_vSpectrumBloc[i + 1][0] != m_vDataMatrix[0][0])
		{
			OV_ERROR_UNLESS_KRF(convertVectorDataToMatrix(iMatrix), "Error converting vector data to spectrum", ErrorType::Internal);

			m_pAlgorithmEncoder->encodeBuffer();
			const uint64_t l_ui64Date = ITimeArithmetics::secondsToTime(std::stod(m_vDataMatrix[0][0].c_str()));
			this->getDynamicBoxContext().markOutputAsReadyToSend(0, l_ui64Date - 1, l_ui64Date);

			//clear matrix
			clearMatrix(m_vDataMatrix);
		}
	}

	//clear matrix
	clearMatrix(l_vSpectrumBloc);
	return true;
}

bool CBoxAlgorithmCSVFileReader::convertVectorDataToMatrix(IMatrix* matrix)
{
	// note: Chunk size shouldn't change after encoding header, do not mess with it here, even if the input has different size

	// We accept partial data, but not buffer overruns ...
	OV_ERROR_UNLESS_KRF(matrix->getDimensionSize(1) >= m_vDataMatrix.size() && matrix->getDimensionSize(0) >= (m_nColumn-1),
						"Matrix size incompatibility, data suggests " << m_nColumn-1 << "x" << uint64_t(m_vDataMatrix.size())
						<< ", expected at most " << matrix->getDimensionSize(0) << "x" << matrix->getDimensionSize(0), ErrorType::Overflow);

	stringstream l_sMatrix;
	for (uint32_t i = 0; i < m_vDataMatrix.size(); i++)
	{
		l_sMatrix << "at time (" << m_vDataMatrix[i][0].c_str() << "):";
		for (uint32_t j = 0; j < m_nColumn - 1; j++)
		{
			matrix->getBuffer()[j * matrix->getDimensionSize(1) + i] = std::stod(m_vDataMatrix[i][j + 1].c_str());
			l_sMatrix << matrix->getBuffer()[j * matrix->getDimensionSize(1) + i] << ";";
		}
		l_sMatrix << "\n";
	}
	getLogManager() << LogLevel_Debug << "Matrix:\n" << l_sMatrix.str().c_str();
	getLogManager() << LogLevel_Debug << "Matrix:\n" << l_sMatrix.str().c_str();

	return true;
}
