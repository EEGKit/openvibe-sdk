#include "ovpCBoxAlgorithmOVCSVFileWriter.h"

#include <openvibe/ovTimeArithmetics.h>
#include <fs/Files.h>

#include <string>
#include <iostream>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace FileIO;

CBoxAlgorithmOVCSVFileWriter::CBoxAlgorithmOVCSVFileWriter()
	: m_writerLib(CSV::createCSVHandler(), CSV::releaseCSVHandler) {}

bool CBoxAlgorithmOVCSVFileWriter::initialize()
{
	m_isFileOpen = false;
	m_epoch      = 0;
	OV_ERROR_UNLESS_KRF(this->getStaticBoxContext().getInputType(0, m_typeID), "Error while getting input type", ErrorType::Internal);

	if (m_typeID == OV_TypeId_Signal)
	{
		m_writerLib->setFormatType(CSV::EStreamType::Signal);
		m_streamDecoder = new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
	{
		m_writerLib->setFormatType(CSV::EStreamType::StreamedMatrix);
		m_streamDecoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else if (m_typeID == OV_TypeId_FeatureVector)
	{
		m_writerLib->setFormatType(CSV::EStreamType::FeatureVector);
		m_streamDecoder = new OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else if (m_typeID == OV_TypeId_Spectrum)
	{
		m_writerLib->setFormatType(CSV::EStreamType::Spectrum);
		m_streamDecoder = new OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else { OV_ERROR_KRF("Input is a type derived from matrix that the box doesn't recognize", ErrorType::BadInput); }

	OV_ERROR_UNLESS_KRF(m_stimulationDecoder.initialize(*this, 1),
						"Error while stimulation decoder initialization",
						ErrorType::Internal);


	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_writerLib->setOutputFloatPrecision(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));
	m_appendData     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_lastMatrixOnly = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	m_writerLib->setLastMatrixOnlyMode(m_lastMatrixOnly);

	if (!m_appendData)
	{
		OV_ERROR_UNLESS_KRF(m_writerLib->openFile(filename.toASCIIString(), OpenViBE::CSV::EFileAccessMode::Write),
							(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
								"Details: " + m_writerLib->getLastErrorString())).c_str(),
							ErrorType::Internal);
		m_writeHeader = true;
	}
	else
	{
		FILE* file = FS::Files::open(filename, "r");

		if (file)
		{
			fseek(file, 0, SEEK_END);
			m_writeHeader = ftell(file) == 0;
			fclose(file);
		}

		OV_ERROR_UNLESS_KRF(m_writerLib->openFile(filename.toASCIIString(), OpenViBE::CSV::EFileAccessMode::Append),
							(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
								"Details: " + m_writerLib->getLastErrorString())).c_str(),
							ErrorType::Internal);
	}

	m_isHeaderReceived = false;

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::uninitialize()
{
	m_streamDecoder.uninitialize();
	m_stimulationDecoder.uninitialize();

	OV_ERROR_UNLESS_KRF(m_writerLib->noEventsUntilDate(std::numeric_limits<double>::max()),
						(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" : "Details: "
							+ m_writerLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(m_writerLib->writeAllDataToFile(),
						(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" : "Details: "
							+ m_writerLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(m_writerLib->closeFile(),
						(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" : "Details: "
							+ m_writerLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::processInput(const size_t /*index*/)
{
	OV_ERROR_UNLESS_KRF(getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(), "Error while marking algorithm as ready to process", ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::process()
{
	OV_ERROR_UNLESS_KRF(this->processStreamedMatrix(),
						"Error have been thrown during streamed matrix process",
						ErrorType::Internal);
	OV_ERROR_UNLESS_KRF(this->processStimulation(),
						"Error have been thrown during stimulation process",
						ErrorType::Internal);

	// write into the library
	if (!m_lastMatrixOnly)
	{
		OV_ERROR_UNLESS_KRF(m_writerLib->writeDataToFile(),
							(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
								"Details: " + m_writerLib->getLastErrorString())).c_str(),
							ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::processStreamedMatrix()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (size_t index = 0; index < dynamicBoxContext.getInputChunkCount(0); ++index)
	{
		OV_ERROR_UNLESS_KRF(m_streamDecoder.decode(index),
							"Failed to decode chunk",
							ErrorType::Internal);

		// represents the properties of the input, no data
		const IMatrix* matrix = m_streamDecoder.getOutputMatrix();

		if (m_streamDecoder.isHeaderReceived())
		{
			OV_ERROR_UNLESS_KRF(!m_isHeaderReceived, "Multiple streamed matrix headers received", ErrorType::BadInput);
			m_isHeaderReceived = true;

			if (m_typeID == OV_TypeId_Signal)
			{
				OV_ERROR_UNLESS_KRF(m_streamDecoder.getOutputSamplingRate() != 0, "Sampling rate can not be 0", ErrorType::BadInput);
				std::vector<std::string> dimensionLabels;

				for (size_t j = 0; j < matrix->getDimensionSize(0); ++j) { dimensionLabels.push_back(matrix->getDimensionLabel(0, j)); }

				OV_ERROR_UNLESS_KRF(
					m_writerLib->setSignalInformation(dimensionLabels, m_streamDecoder.getOutputSamplingRate(), matrix->getDimensionSize(1)),
					(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" : "Details: " +
						m_writerLib->getLastErrorString())).c_str(),
					ErrorType::Internal);

				if (m_writeHeader)
				{
					OV_ERROR_UNLESS_KRF(m_writerLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ?
											"" : "Details: " + m_writerLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
			{
				std::vector<std::string> dimensionLabels;
				std::vector<size_t> dimensionSizes;

				for (size_t dimensionIndex = 0; dimensionIndex < matrix->getDimensionCount(); ++dimensionIndex)
				{
					dimensionSizes.push_back(matrix->getDimensionSize(dimensionIndex));

					for (size_t j = 0; j < matrix->getDimensionSize(dimensionIndex); ++j)
					{
						dimensionLabels.push_back(matrix->getDimensionLabel(dimensionIndex, j));
					}
				}

				OV_ERROR_UNLESS_KRF(m_writerLib->setStreamedMatrixInformation(dimensionSizes, dimensionLabels),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
										"Details: " + m_writerLib->getLastErrorString())).c_str(),
									ErrorType::Internal);

				if (m_writeHeader)
				{
					OV_ERROR_UNLESS_KRF(m_writerLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ?
											"" : "Details: " + m_writerLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_typeID == OV_TypeId_FeatureVector)
			{
				std::vector<std::string> channelsLabels;

				for (size_t j = 0; j < matrix->getDimensionSize(0); ++j) { channelsLabels.push_back(matrix->getDimensionLabel(0, j)); }

				OV_ERROR_UNLESS_KRF(m_writerLib->setFeatureVectorInformation(channelsLabels),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
										"Details: " + m_writerLib->getLastErrorString())).c_str(),
									ErrorType::Internal);

				if (m_writeHeader)
				{
					OV_ERROR_UNLESS_KRF(m_writerLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ?
											"" : "Details: " + m_writerLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_typeID == OV_TypeId_Spectrum)
			{
				const IMatrix* frequencyAbscissaMatrix = m_streamDecoder.getOutputFrequencyAbcissa();
				std::vector<std::string> channelsLabels;
				std::vector<double> frequencyAbscissa;

				for (size_t j = 0; j < matrix->getDimensionSize(0); ++j) { channelsLabels.push_back(matrix->getDimensionLabel(0, j)); }

				for (size_t j = 0; j < frequencyAbscissaMatrix->getDimensionSize(0); ++j)
				{
					frequencyAbscissa.push_back(frequencyAbscissaMatrix->getBuffer()[j]);
				}

				OV_ERROR_UNLESS_KRF(m_writerLib->setSpectrumInformation(channelsLabels, frequencyAbscissa, m_streamDecoder.getOutputSamplingRate()),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
										"Details: " + m_writerLib->getLastErrorString())).c_str(),
									ErrorType::Internal);

				if (m_writeHeader)
				{
					OV_ERROR_UNLESS_KRF(m_writerLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ?
											"" : "Details: " + m_writerLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
		}

		if (m_streamDecoder.isBufferReceived())
		{
			const IMatrix* imatrix = m_streamDecoder.getOutputMatrix();

			if (m_typeID == OV_TypeId_Signal)
			{
				const uint64_t samplingFrequency = m_streamDecoder.getOutputSamplingRate();
				const uint64_t chunkStartTime    = dynamicBoxContext.getInputChunkStartTime(0, index);
				const size_t nChannel      = matrix->getDimensionSize(0);
				const size_t nSample       = matrix->getDimensionSize(1);

				for (size_t sampleIndex = 0; sampleIndex < nSample; ++sampleIndex)
				{
					std::vector<double> matrixValues;
					// get starting and ending time

					const uint64_t timeOfNthSample       = TimeArithmetics::sampleCountToTime(samplingFrequency, sampleIndex); // assuming chunk start is 0
					const uint64_t sampleTime            = chunkStartTime + timeOfNthSample;
					const double startTime               = double(TimeArithmetics::timeToSeconds(sampleTime));
					const uint64_t timeOfNthAndOneSample = TimeArithmetics::sampleCountToTime(samplingFrequency, sampleIndex + 1);
					const double endTime                 = double(TimeArithmetics::timeToSeconds(chunkStartTime + timeOfNthAndOneSample));

					// get matrix values
					for (size_t channelIndex = 0; channelIndex < nChannel; ++channelIndex)
					{
						matrixValues.push_back(imatrix->getBuffer()[channelIndex * nSample + sampleIndex]);
					}

					// add sample to the library
					OV_ERROR_UNLESS_KRF(m_writerLib->addSample({ startTime, endTime, matrixValues, m_epoch }),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ?
											"" : "Details: " + m_writerLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
			{
				const double startTime = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkStartTime(0, index));
				const double endTime   = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(0, index));
				const std::vector<double> streamedMatrixValues(matrix->getBuffer(), matrix->getBuffer() + matrix->getBufferElementCount());

				OV_ERROR_UNLESS_KRF(m_writerLib->addSample({ startTime, endTime, streamedMatrixValues, m_epoch }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
										"Details: " + m_writerLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}
			else if (m_typeID == OV_TypeId_FeatureVector)
			{
				const double startTime = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkStartTime(0, index));
				const double endTime   = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(0, index));
				const IMatrix* zmatrix = m_streamDecoder.getOutputMatrix();

				const std::vector<double> streamedMatrixValues(zmatrix->getBuffer(), zmatrix->getBuffer() + zmatrix->getBufferElementCount());

				OV_ERROR_UNLESS_KRF(m_writerLib->addSample({ startTime, endTime, streamedMatrixValues, m_epoch }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
										"Details: " + m_writerLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}
			else if (m_typeID == OV_TypeId_Spectrum)
			{
				const double startTime = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkStartTime(0, index));
				const double endTime   = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(0, index));
				const std::vector<double> streamedMatrixValues(matrix->getBuffer(), matrix->getBuffer() + matrix->getBufferElementCount());

				OV_ERROR_UNLESS_KRF(m_writerLib->addSample({ startTime, endTime, streamedMatrixValues, m_epoch }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
										"Details: " + m_writerLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}

			m_epoch++;
		}

		OV_ERROR_UNLESS_KRF(dynamicBoxContext.markInputAsDeprecated(0, index),
							"Fail to mark input as deprecated",
							ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::processStimulation()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	// add every stimulation received
	for (size_t index = 0; index < dynamicBoxContext.getInputChunkCount(1); ++index)
	{
		OV_ERROR_UNLESS_KRF(m_stimulationDecoder.decode(index),
							"Failed to decode stimulation chunk",
							ErrorType::Internal);

		if (m_stimulationDecoder.isBufferReceived())
		{
			const IStimulationSet* stimulationSet = m_stimulationDecoder.getOutputStimulationSet();
			// for each stimulation, get its informations

			for (size_t stimulationIndex = 0; stimulationIndex < stimulationSet->getStimulationCount(); ++stimulationIndex)
			{
				OV_ERROR_UNLESS_KRF(m_writerLib->addEvent({ stimulationSet->getStimulationIdentifier(stimulationIndex),
										TimeArithmetics::timeToSeconds(stimulationSet->getStimulationDate(stimulationIndex)),
										TimeArithmetics::timeToSeconds(stimulationSet->getStimulationDuration(stimulationIndex)) }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" :
										"Details: " + m_writerLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}
			
			// set NoEventUntilDate to prevent time that will be empty of stimulations until the end of the last chunk
			OV_ERROR_UNLESS_KRF(
				m_writerLib->noEventsUntilDate(TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(1, (dynamicBoxContext.getInputChunkCount(1
				) - 1)))),
				(OpenViBE::CSV::ICSVHandler::getLogError(m_writerLib->getLastLogError()) + (m_writerLib->getLastErrorString().empty() ? "" : "Details: " +
					m_writerLib->getLastErrorString())).c_str(),
				ErrorType::Internal);
		}

		OV_ERROR_UNLESS_KRF(dynamicBoxContext.markInputAsDeprecated(1, index),
							"Failed to mark stimulations input as deprecated",
							ErrorType::Internal);
	}

	return true;
}
