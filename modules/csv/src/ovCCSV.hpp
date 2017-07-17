/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * CertiViBE
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

#pragma once

#include "defines.h"
#include <ovICSV.h>

#include <fstream>
#include <string>
#include <vector>

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

			uint32_t getOutputFloatPrecision() { return m_OutputFloatPrecision; }
			void setOutputFloatPrecision(uint32_t precision) { m_OutputFloatPrecision = precision; }

			void setFormatType(EStreamType typeIdentifier);
			EStreamType getFormatType(void);

			void setLastMatrixOnlyMode(bool isActivated) { m_LastMatrixOnly = isActivated; }
			bool getLastMatrixOnlyMode(void) { return m_LastMatrixOnly; }

			bool setSignalInformation(const std::vector<std::string>& channelNames, uint32_t samplingFrequency, uint32_t sampleCountPerBuffer);
			bool getSignalInformation(std::vector<std::string>& channelNames, uint32_t& samplingFrequency, uint32_t& sampleCountPerBuffer);

			bool setSpectrumInformation(const std::vector<std::string>& channelNames, const std::vector<double>& frequencyAbscissa, const uint32_t samplingRate);
			bool getSpectrumInformation(std::vector<std::string> &channelNames, std::vector<double> &frequencyAbscissa, uint32_t& samplingRate);

			bool setFeatureVectorInformation(const std::vector<std::string>& channelNames);
			bool getFeatureVectorInformation(std::vector<std::string>& channelNames);

			bool setStreamedMatrixInformation(const std::vector<uint32_t>& dimensionSizes, const std::vector<std::string>& labels);
			bool getStreamedMatrixInformation(std::vector<uint32_t>& dimensionSizes, std::vector<std::string>& labels);

			bool writeHeaderToFile(void);

			bool writeDataToFile(void);

			bool writeAllDataToFile(void);

			bool readSamplesAndEventsFromFile(uint64_t linesToRead, std::vector<SMatrixChunk>& samples, std::vector<SStimulationChunk>& events);

			bool openFile(const std::string& fileName, EFileAccessMode mode);

			bool closeFile(void);

			bool addSample(const SMatrixChunk& sample);

			bool addBuffer(const std::vector<SMatrixChunk>& samples);

			bool addEvent(uint64_t code, double date, double duration);

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
			bool readSampleChunk(SMatrixChunk& sample, uint64_t line);

			/**
			 * \brief Read line data conerning stimulations
			 *
			 * \param stimulations vector to stock stimulations in (identifier, date and duration)
			 * \param line the line actually reading
			 *
			 * \retval true in case of success
			 * \retval false in case of error (as letters instead of numbers)
			 */
			bool readStimulationChunk(std::vector<SStimulationChunk>& stimulations, uint64_t line);

			/**
			 * \brief Update position into the matrix while reading or writing
			 *
			 * \param position is the position into the matrix
			 *
			 * \retval true in case of success
			 * \retval false in case of browse matrix
			 */
			bool increasePositionIndexes(std::vector<uint32_t>& position);

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

			uint32_t m_DimensionCount;
			std::vector<uint32_t> m_DimensionSizes;
			std::vector<std::string> m_DimensionLabels;
			uint32_t m_SampleCountPerBuffer;
			double m_NoEventSince;

			std::vector<double> m_FrequencyAbscissa;

			// columns between each separator (as : {Time, Epoch, O1, O2, O3, Event Id, Event date, Event Duration})
			std::vector<std::string> m_LineColumns;
			uint32_t m_SamplingRate;
			uint32_t m_ColumnCount;
			uint32_t m_PreDataColumnCount;
			uint32_t m_PostDataColumnCount;

			bool m_HasInputType;
			bool m_IsFirstLineWritten;
			bool m_IsHeaderRead;
			bool m_IsSetInfoCalled;
			bool m_HasEpoch;

			uint32_t m_OriginalSampleNumber;
			uint32_t m_OutputFloatPrecision;

			bool m_LastMatrixOnly;
		};
	}
}
