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

#pragma once

#include "defines.h"
#include <ovICSV.h>

#include <fstream>
#include <string>
#include <vector>
#include <array>

namespace OpenViBE
{
	namespace CSV
	{
		class CCSVHandler final : public ICSVHandler
		{
		public:
			/**
			 * \brief Set lib value to default
			 */
			CCSVHandler(void);

			/**
			 * \brief Close the file if it is open.
			 */
			~CCSVHandler(void);

			unsigned int getOutputFloatPrecision() {return m_OutputFloatPrecision; }
			void setOutputFloatPrecision(unsigned int precision) { m_OutputFloatPrecision = precision; }

			void setFormatType(EStreamType typeIdentifier);

			EStreamType getFormatType(void);

			bool setSignalInformation(const std::vector<std::string>& channelNames, unsigned int samplingFrequency, unsigned int sampleCountPerBuffer);
			bool getSignalInformation(std::vector<std::string>& channelNames, unsigned int& samplingFrequency, unsigned int& sampleCountPerBuffer);

			bool setSpectrumInformation(const std::vector<std::string>& channelNames, const std::vector<double>& frequencyAbscissa, const unsigned int samplingRate);
			bool getSpectrumInformation(std::vector<std::string>& channelNames, std::vector<double>& frequencyBands, unsigned int& sampleCountPerBuffer);

			bool setFeatureVectorInformation(const std::vector<std::string>& channelNames);
			bool getFeatureVectorInformation(std::vector<std::string>& channelNames);

			bool setStreamedMatrixInformation(const std::vector<unsigned int>& dimensionSizes, const std::vector<std::string>& labels);
			bool getStreamedMatrixInformation(std::vector<unsigned int>& dimensionSizes, std::vector<std::string>& labels);

			bool writeHeaderToFile(void);

			bool writeDataToFile(void);

			bool writeAllDataToFile(void);

			bool readSamplesAndEventsFromFile(unsigned long long linesToRead, std::vector<SMatrixChunk>& samples, std::vector<SStimulationChunk>& events);

			bool openFile(const std::string& fileName, EFileAccessMode mode);

			bool closeFile(void);

			bool addSample(const SMatrixChunk& sample);

			bool addBuffer(const std::vector<SMatrixChunk>& samples);

			bool addEvent(unsigned long long code, double date, double duration);

			bool addEvent(const SStimulationChunk& event);

			bool noEventsUntilDate(double date);

			ELogErrorCodes getLastLogError();

			std::string getLastErrorString();

		private:
			void split(const std::string& string, char delimitor, std::vector<std::string>& element);

			/**
			 * \brief Create a string with stimulations to add in the buffer
			 *
			 * \param stimulationsToPrint stimulations to put into the buffer
			 *
			 * \return string with stimulations to write
			 */
			std::string writeStimulations(const std::vector<SStimulationChunk>& stimulationsToPrint);

			/**
			 * \brief Create a string representation of the header data
			 *
			 * \retval true Header data as it should be written in the file
			 * \retval "" in case of error
			 */
			std::string createHeaderString(void);

			/**
			 * \brief Set the buffer in function of data saved
			 *
			 * \param canWriteAll true if it must write all lines, false if it write only the next buffer
			 *
			 * \retval true in case of success
			 * \retval false in case of wrong data sent
			 */
			bool createCSVStringFromData(bool canWriteAll);

			/**
			 * \brief Parsing header to read data
			 *
			 * \retval true in case of correct header
			 * \retval false in case of incorrect header
			 */
			bool parseHeader(void);

			/**
			 * \brief Parsing header to read signal data
			 *
			 * \retval true in case of correct header
			 * \retval false in case of incorrect header
			 */
			bool parseSignalHeader(void);

			/**
			* \brief Parsing header to read Spectrum data
			*
			* \retval true in case of correct header
			* \retval false in case of incorrect header
			*/
			bool parseSpectrumHeader(void);

			/**
			* \brief Parsing header to read matrix data (Streamed Matrix, Covariance matrix and Feature Vector)
			*
			* \retval true in case of correct header
			* \retval false in case of incorrect header
			*/
			bool parseMatrixHeader(void);

			/**
			 * \brief Read line data concerning time, epoch and matrix
			 *
			 * \param sample reference to stock data in
			 * \param line index of the read line
			 *
			 * \retval true in case of success
			 * \retval false in case of error (as letters instead of numbers)
			 */
			bool readSampleChunk(SMatrixChunk& sample, unsigned long long line);

			/**
			 * \brief Read line data conerning stimulations
			 *
			 * \param stimulations vector to stock stimulations in (identifier, date and duration)
			 * \param line the line actually reading
			 *
			 * \retval true in case of success
			 * \retval false in case of error (as letters instead of numbers)
			 */
			bool readStimulationChunk(std::vector<SStimulationChunk>& stimulations, unsigned long long line);

			/**
			 * \brief Update position into the matrix while reading or writing
			 *
			 * \param position is the position into the matrix
			 *
			 * \retval true in case of success
			 * \retval false in case of browse matrix
			 */
			bool increasePositionIndexes(std::vector<unsigned int>& position);

			/**
			 * \brief Read lines of the first epoch to found sample count per buffer
			 *
			 * \retval true in case of success
			 * \retval false in case of error
			 */
			bool calculateSampleCountPerBuffer();

			std::fstream m_Fs;
			std::string m_Filename;
			std::string m_Buffer;
			std::vector<SMatrixChunk> m_Chunks;
			std::vector<SStimulationChunk> m_Stimulations;
			ELogErrorCodes m_LogError;
			std::string m_LastStringError;

			EStreamType m_InputTypeIdentifier;

			typedef std::istream& GetLine(std::istream& inputStream, std::string& outputString, const char delimiter);
			GetLine* m_GetLineFunction;

			unsigned int m_DimensionCount;
			std::vector<unsigned int> m_DimensionSizes;
			std::vector<std::string> m_DimensionLabels;
			unsigned int m_SampleCountPerBuffer;
			double m_NoEventSince;

			std::vector<double> m_FrequencyAbscissa;
			std::vector<double> m_FrequencyBandsBuffer;

			// columns between each separator (as : {Time, Epoch, O1, O2, O3, Event Id, Event date, Event Duration})
			std::vector<std::string> m_LineColumns;
			unsigned int m_SamplingRate;
			unsigned int m_ColumnCount;
			unsigned int m_PreDataColumnCount;
			unsigned int m_PostDataColumnCount;

			bool m_HasInputType;
			bool m_IsFirstLineWritten;
			bool m_IsHeaderRead;
			bool m_IsSetInfoCalled;
			bool m_HasEpoch;

			unsigned int m_OriginalSampleNumber;
			unsigned int m_OutputFloatPrecision;
		};
	}
}
