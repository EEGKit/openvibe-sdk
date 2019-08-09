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
			CCSVHandler();

			/**
			 * \brief Close the file if it is open.
			 */
			~CCSVHandler() override;

			/**
			 * \brief Get the floating point precision used to write float values.
			 *
			 * \return the Floating point precision.
			 */
			uint32_t getOutputFloatPrecision() override { return m_OutputFloatPrecision; }

			/**
			 * \brief Set the floating point precision used to write float values.
			 *
			 * \param precision the floating point precision.
			 */
			void setOutputFloatPrecision(uint32_t precision) override { m_OutputFloatPrecision = precision; }

			void setFormatType(EStreamType typeIdentifier) override;
			EStreamType getFormatType() override;

			void setLastMatrixOnlyMode(bool isActivated) override { m_LastMatrixOnly = isActivated; }
			bool getLastMatrixOnlyMode() override { return m_LastMatrixOnly; }

			bool setSignalInformation(const std::vector<std::string>& channelNames, uint32_t samplingFrequency, uint32_t sampleCountPerBuffer) override;
			bool getSignalInformation(std::vector<std::string>& channelNames, uint32_t& samplingFrequency, uint32_t& sampleCountPerBuffer) override;

			bool setSpectrumInformation(const std::vector<std::string>& channelNames, const std::vector<double>& frequencyAbscissa, uint32_t samplingRate) override;
			bool getSpectrumInformation(std::vector<std::string>& channelNames, std::vector<double>& frequencyAbscissa, uint32_t& samplingRate) override;

			bool setFeatureVectorInformation(const std::vector<std::string>& channelNames) override;
			bool getFeatureVectorInformation(std::vector<std::string>& channelNames) override;

			bool setStreamedMatrixInformation(const std::vector<uint32_t>& dimensionSizes, const std::vector<std::string>& labels) override;
			bool getStreamedMatrixInformation(std::vector<uint32_t>& dimensionSizes, std::vector<std::string>& labels) override;

			/**
			 * \brief Write the header to the file
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool writeHeaderToFile() override;

			/**
			 * \brief Write current available data to the file until the last stimulation or if you set that it will not have new event before a date.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 *
			 * \sa noEventsUntilDate
			 */
			bool writeDataToFile() override;

			/**
			 * \brief Write current available data to the file.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool writeAllDataToFile() override;

			/**
			 * \brief Read samples and stimulations.
			 *
			 * \param linesToRead Maximum number of lines to read. If there is no more data in the file, the number of lines read can be lower.
			 * \param chunks[out] Valid chunks read.
			 * \param stimulations[out] Valid stimulations read.
			 *
			 * \retval True in case of success, even if the number of lines is different than the linesToRead parameter.
			 * \retval False in case of error.
			 */
			bool readSamplesAndEventsFromFile(size_t linesToRead, std::vector<SMatrixChunk>& chunks, std::vector<SStimulationChunk>& stimulations) override;

			/**
			 * \brief Open a OV CSV file.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool openFile(const std::string& fileName, EFileAccessMode mode) override;

			/**
			 * \brief Close the opened file.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool closeFile() override;

			/**
			 * \brief Add a single sample.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool addSample(const SMatrixChunk& sample) override;

			/**
			 * \brief Add several samples.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool addBuffer(const std::vector<SMatrixChunk>& samples) override;

			/**
			 * \brief Add a single stimulation.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool addEvent(uint64_t code, double date, double duration) override;

			/**
			 * \brief Add several stimulations.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 */
			bool addEvent(const SStimulationChunk& event) override;

			/**
			 * \brief Guarantee that will not have new event before a date.
			 * This information is used to allow the library to write all the chunks available, before this date, to a file.
			 *
			 * \retval True in case of success.
			 * \retval False in case of error.
			 *
			 * \sa writeDataToFile
			 */
			bool noEventsUntilDate(double date) override;

			ELogErrorCodes getLastLogError() override;

			std::string getLastErrorString() override;

			/**
			 * \brief Check if there is still data to read in the file.
			 *
			 * \retval True if there is still data to read in the file.
			 * \retval False if there is no more data to read in the file.
			 */
			bool hasDataToRead() const override;

		private:
			/**
			 * \brief Split a string into a vector of strings.
			 *
			 * \param string String to split.
			 * \param delimitor Delimitor.
			 * \param elements[out] Vector of string.
			 */
			void split(const std::string& string, char delimitor, std::vector<std::string>& elements) const;

			/**
			 * \brief Create a string with stimulations to add in the buffer.
			 *
			 * \param stimulationsToPrint stimulations to put into the buffer
			 *
			 * \return string with stimulations to write
			 */
			std::string stimulationsToString(const std::vector<SStimulationChunk>& stimulationsToPrint) const;

			/**
			 * \brief Create a string representation of the header data.
			 *
			 * \retval true Header data as it should be written in the file
			 * \retval "" in case of error
			 */
			std::string createHeaderString();

			/**
			 * \brief Set the buffer in function of data saved.
			 *
			 * \param canWriteAll true if it must write all lines, false if it write only the next buffer
			 * \param csv The CSV string.
			 *
			 * \retval true in case of success
			 * \retval false in case of wrong data sent
			 */
			bool createCSVStringFromData(bool canWriteAll, std::string& csv);

			/**
			 * \brief Parsing header to read data.
			 *
			 * \retval true in case of correct header
			 * \retval false in case of incorrect header
			 */
			bool parseHeader();

			/**
			 * \brief Parsing header to read signal data.
			 *
			 * \param header[out]
			 *
			 * \retval true in case of correct header
			 * \retval false in case of incorrect header
			 */
			bool parseSignalHeader(const std::vector<std::string>& header);

			/**
			* \brief Parsing header to read Spectrum data.
			*
			* \param header[out]
			*
			* \retval true in case of correct header
			* \retval false in case of incorrect header
			*/
			bool parseSpectrumHeader(const std::vector<std::string>& header);

			/**
			* \brief Parsing header to read matrix data (Streamed Matrix, Covariance matrix and Feature Vector).
			*
			* \param header[out]
			*
			* \retval true in case of correct header
			* \retval false in case of incorrect header
			*/
			bool parseMatrixHeader(const std::vector<std::string>& header);

			/**
			 * \brief Read line data concerning time, epoch and matrix.
			 *
			 * \param line line to read
			 * \param sample[out] reference to stock data in
			 * \param lineNb index of the read line
			 *
			 * \retval true in case of success
			 * \retval false in case of error (as letters instead of numbers)
			 */
			bool readSampleChunk(const std::string& line, SMatrixChunk& sample, uint64_t lineNb);

			/**
			 * \brief Read line data conerning stimulations.
			 *
			 * \param line the line to read
			 * \param stimulations[out] vector to stock stimulations in (identifier, date and duration)
			 * \param lineNb the line actually reading
			 *
			 * \retval true in case of success
			 * \retval false in case of error (as letters instead of numbers)
			 */
			bool readStimulationChunk(const std::string& line, std::vector<SStimulationChunk>& stimulations, uint64_t lineNb);

			/**
			 * \brief Update position into the matrix while reading or writing.
			 *
			 * \param position[out] is the position into the matrix
			 *
			 * \retval true in case of success
			 * \retval false in case of browse matrix
			 */
			bool increasePositionIndexes(std::vector<uint32_t>& position);

			/**
			 * \brief Read lines of the first epoch to found sample count per buffer.
			 *
			 * \retval true in case of success
			 * \retval false in case of error
			 */
			bool calculateSampleCountPerBuffer();

			/**
			 * \brief Read a stream until a delimiter and provide the string before the delimiter.
			 *
			 * \param inputStream The stream to read.
			 * \param outputString The string before the next delimitor.
			 * \param delimiter The delimiter .
			 */
			bool streamReader(std::istream& inputStream, std::string& outputString, char delimiter, std::string& bufferHistory) const;

			std::fstream m_Fs;
			std::string m_Filename;
			std::deque<SMatrixChunk> m_Chunks;
			std::deque<SStimulationChunk> m_Stimulations;
			ELogErrorCodes m_LogError;
			std::string m_LastStringError;

			EStreamType m_InputTypeIdentifier;

			typedef std::istream& GetLine(std::istream& inputStream, std::string& outputString, char delimiter);
			uint32_t m_DimensionCount;
			std::vector<uint32_t> m_DimensionSizes;
			std::vector<std::string> m_DimensionLabels;
			uint32_t m_SampleCountPerBuffer;
			double m_NoEventSince;

			std::vector<double> m_FrequencyAbscissa;

			uint32_t m_SamplingRate;
			size_t m_ColumnCount;

			bool m_HasInputType;
			bool m_IsFirstLineWritten;
			bool m_IsHeaderRead;
			bool m_IsSetInfoCalled;
			bool m_HasEpoch;

			uint32_t m_OriginalSampleNumber;
			uint32_t m_OutputFloatPrecision;

			bool m_LastMatrixOnly;

			std::string m_BufferReadFileLine; // Buffer used to store unused read chars.

			bool m_HasDataToRead;

			bool m_IsCRLFEOL; // Is a CRLF end of line
		};
	}
}
