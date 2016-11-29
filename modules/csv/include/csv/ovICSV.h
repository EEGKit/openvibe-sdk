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

#include <queue>
#include <string>
#include <vector>
#include <array>

namespace OpenViBE
{
	namespace CSV
	{
		struct SMatrixChunk
		{
			double startTime;
			double endTime;
			std::vector<double> matrix;
			unsigned long long epoch;

			SMatrixChunk(double startTime, double endTime, const std::vector<double>& matrix, unsigned long long epoch)
				: startTime(startTime),
				endTime(endTime),
				matrix(matrix),
				epoch(epoch) {}
		};

		struct SStimulationChunk
		{
			unsigned long long stimulationIdentifier;
			double stimulationDate;
			double stimulationDuration;

			SStimulationChunk(unsigned long long stimulationIdentifier, double stimulationDate, double stimulationDuration)
				: stimulationIdentifier(stimulationIdentifier),
				stimulationDate(stimulationDate),
				stimulationDuration(stimulationDuration) {}
		};

		enum class EStreamType
		{
			StreamedMatrix = 0,
			FeatureVector = 1,
			Signal = 2,
			Spectrum = 3,
			CovarianceMatrix = 4
		};

		enum ELogErrorCodes
		{
			LogErrorCodes_MissingData = -31,
			LogErrorCodes_WrongParameters = -30,
			LogErrorCodes_ErrorWhileWriting = -29,
			LogErrorCodes_CantWriteHeader = -28,
			LogErrorCodes_InvalidArgumentException = -27,
			LogErrorCodes_OutOfRangeException = -26,
			LogErrorCodes_DimensionCountZero = -25,
			LogErrorCodes_WrongDimensionSize = -24,
			LogErrorCodes_EmptyColumn = -23,
			LogErrorCodes_StimulationSize = -22,
			LogErrorCodes_InvalidStimulationArgument = -21,
			LogErrorCodes_WrongSampleDate = -20,
			LogErrorCodes_NegativeStimulation = -19,
			LogErrorCodes_NotEnoughLines = -18,
			LogErrorCodes_SampleNotEmpty = -17,
			LogErrorCodes_WrongLineSize = -16,
			LogErrorCodes_WrongHeader = -15,
			LogErrorCodes_WrongInputType = -14,
			LogErrorCodes_SetInfoOnce = -13,
			LogErrorCodes_NoChannelsName = -12,
			LogErrorCodes_DimensionSizeEmpty = -11,
			LogErrorCodes_DimensionSizeZero = -10,
			LogErrorCodes_MatrixEmpty = -9,
			LogErrorCodes_WrongMatrixSize = -8,
			LogErrorCodes_NoMatrixLabels = -7,
			LogErrorCodes_NoSample = -6,
			LogErrorCodes_DateError = -5,
			LogErrorCodes_DurationError = -4,
			LogErrorCodes_CantOpenFile = -3,
			LogErrorCodes_NoFileDefined = -2,
			LogErrorCodes_ErrorWhileClosing = -1
		};

		enum class EFileAccessMode
		{
			Write,
			Append,
			Read
		};

		class CSV_API ICSVLib
		{
		public:
			/**
			 * \brief Set the format type that will be written or read.
			 *
			 * \param typeIdentifier Could be Streamed Matrix, Signal, Covariance Matrix, etc ...
			 */
			virtual void setFormatType(EStreamType typeIdentifier) = 0;

			/**
			 * \brief Return the format type
			 *
			 * \return EStreamType format type
			 */
			virtual EStreamType getFormatType(void) = 0;

			/**
			 * \brief Set informations to read or write signal data
			 *
			 * \param channelNames all channels names for the matrix
			 * \param samplingFrequency sampling frequency
			 * \param sampleCountPerBuffer number of sample per buffer
			 *
			 * \retval true in case of success
			 * \retval false in case of incorrect or incomplete parameters
			 */
			virtual bool setSignalInformation(const std::vector<std::string>& channelNames, unsigned int samplingFrequency, unsigned int sampleCountPerBuffer) = 0;

			/**
			 * \brief Get signal information in file
			 *
			 * \param channelNames reference to fill with file channel names
			 * \param samplingFrequency sampling frequency
			 * \param sampleCountPeBuffer number of sample per buffer
			 *
			 * \retval true in case of success
			 * \retval false in case of failure
			 */
			virtual bool getSignalInformation(std::vector<std::string>& channelNames, unsigned int& samplingFrequency, unsigned int& sampleCountPerBuffer) = 0;

			/**
			 * \brief Set informations to read or write spectrum data
			 *
			 * \param channelNames all channels names for the matrix
			 * \param frequencyBands two frequency (start and end) of the frequency range
			 *
			 * \retval true in case of success
			 * \retval false in case of incorrect or incomplete parameters
			 */
			virtual bool setSpectrumInformation(const std::vector<std::string>& channelNames, std::vector<std::array<double, 2>> frequencyBands) = 0;

			/**
			 * \brief get spectrum information in file
			 *
			 * \param channelNames reference to fill with file channel names
			 * \param frequencyBands frequency bands
			 * \param sampleCountPerBuffer number of sample per buffer
			 *
			 * \retval true in case of success
			 * \retval false in case of failure
			 */
			virtual bool getSpectrumInformation(std::vector<std::string>& channelNames, std::vector<double>& frequencyBands, unsigned int& sampleCountPerBuffer) = 0;

			/**
			 * \brief Set informations to read or write vector data
			 *
			 * \param channelNames all channels names for the matrix
			 *
			 * \retval true in case of success
			 * \retval false in case of incorrect or incomplete parameters
			 */
			virtual bool setFeatureVectorInformation(const std::vector<std::string>& channelNames) = 0;

			/**
			 * \brief Get feature vector information in file
			 *
			 * \param channelNames reference to fill with file channel names
			 *
			 * \retval true in case of success
			 * \retval false in case of failure
			 */
			virtual bool getFeatureVectorInformation(std::vector<std::string>& channelNames) = 0;

			/**
			 * \brief Set informations to read or write covariance matrix data
			 *
			 * \param channelNames all channels names for the matrix
			 *
			 * \retval true in case of success
			 * \retval false in case of incorrect or incomplete parameters
			 */
			virtual bool setCovarianceMatrixInformation(const std::vector<std::string>& channelNames) = 0;

			/**
			 * \brief Get covariance matrix information in file
			 *
			 * \param channelNames reference to fill with file channel names
			 *
			 * \retval true in case of success
			 * \retval false in case of failure
			 */
			virtual bool getCovarianceMatrixInformation(std::vector<std::string>& channelNames) = 0;

			/**
			 * \brief Set informations to read or write streamed matrix data
			 *
			 * \param dimensionSizes size of each dimension
			 * \param labels all channels names for the matrix
			 *
			 * \retval true in case of success
			 * \retval false in case of incorrect or incomplete parameters
			 */
			virtual bool setStreamedMatrixInformation(const std::vector<unsigned int>& dimensionSizes, const std::vector<std::string>& labels) = 0;

			/**
			 * \brief Get streamed matrix information in file
			 *
			 * \param labels reference to fill with file channel names
			 *
			 * \retval true in case of success
			 * \retval false in case of error
			 */
			virtual bool getStreamedMatrixInformation(std::vector<unsigned int>& dimensionSizes, std::vector<std::string>& labels) = 0;

			/**
			 * \brief Write the header if available in the file
			 *
			 * \retval true in case of writing header
			 * \retval false in case of already written header
			 */
			virtual bool writeHeaderToFile(void) = 0;

			/**
			 * \brief Write the matrix data in the opened file
			 *
			 * \retval true in case of success
			 * \return false in case of error while writing
			 */
			virtual bool writeDataToFile(void) = 0;

			/**
			 * \brief Write all the remaining data
			 *
			 * \retval true in case of sucess
			 * \retval false in case of error while writing
			 */
			virtual bool writeAllDataToFile(void) = 0;

			/**
			 * \brief Read samples and events on the specified number of chunks
			 *
			 * \param chunksToRead number of chunks to read
			 * \param samples structures reference to put the data in
			 * \param events reference to a vector of event structure to put the data in
			 * \param chunksRead reference to read chunks during the function call
			 *
			 * \return number of read chunks
			 */
			virtual bool readSamplesAndEventsFromFile(unsigned long long chunksToRead, std::vector<SMatrixChunk>& samples, std::vector<SStimulationChunk>& events) = 0;

			/**
			 * \brief Open file specified on parameter
			 *
			 * \param filename is the filename of the file to open
			 *
			 * \retval true in case of success
			 * \retval false in case of error while opening the file
			 */
			virtual bool openFile(const std::string& fileName, EFileAccessMode mode) = 0;

			/**
			 * \brief close the opened file
			 *
			 * \retval true in case of sucess
			 * \retval false in case of error while closing
			 */
			virtual bool closeFile(void) = 0;

			/**
			 * \brief Add a sample of the data type
			 *
			 * \param sample is the sample to add
			 *
			 * \retval true in case of correct sample
			 * \retval false in case of empty or incorrect sample
			 */
			virtual bool addSample(const SMatrixChunk& sample) = 0;

			/**
			 * \brief Add a buffer (multiple samples) of the data type.
			 *
			 * \param samples to add
			 *
			 * \retval true in case of success
			 * \retval false in case of error in the added sample
			 */
			virtual bool addBuffer(const std::vector<SMatrixChunk>& samples) = 0;

			/**
			 * \brief Add an event
			 *
			 * \param code is the event identifier
			 * \param date in seconds (must be positive)
			 * \param duration in seconds (must be positive)
			 *
			 * \retval true in case of correct event
			 * \retval false in case of incorrect param
			 */
			virtual bool addEvent(unsigned long long code, double date, double duration) = 0;

			/**
			 * \brief Add an event
			 *
			 * \param event is the event to add (with identifier, data and duration)
			 *
			 * \retval true in case of success
			 * \retval false in case of incorrect event
			 */
			virtual bool addEvent(const SStimulationChunk& event) = 0;

			/**
			 * \brief Set the time of the next stimulation
			 *
			 * \param date the date of the next stimulation
			 *
			 * \retval true in case of success
			 * \retval false in case of negative date
			 */
			virtual bool noEventsUntilDate(double date) = 0;

			/**
			 * \brief Return the error last log error.
			 *
			 * \return last log error code
			 */
			virtual ELogErrorCodes getLastLogError() = 0;

			/**
			 * \brief Return the error associated to the code in parameter.
			 *
			 * \param code error code
			 *
			 * \return error string
			 */
			static std::string getLogError(ELogErrorCodes code);

			/**
			 * \brief Return additionnal information on the error in certain cases
			 *
			 * \return string additionnal information
			 */
			virtual std::string getLastErrorString() = 0;
		protected:
			virtual ~ICSVLib() {}
		};

		extern CSV_API ICSVLib* createCSVLib();

		extern CSV_API void releaseCSVLib(ICSVLib* object);
	}
}
