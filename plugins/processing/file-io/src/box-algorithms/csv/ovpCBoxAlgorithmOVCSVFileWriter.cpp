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
	: m_WriterLib(CSV::createCSVHandler(), CSV::releaseCSVHandler) {}

bool CBoxAlgorithmOVCSVFileWriter::initialize()
{
	m_IsFileOpen = false;
	m_Epoch      = 0;
	OV_ERROR_UNLESS_KRF(this->getStaticBoxContext().getInputType(0, m_TypeIdentifier),
						"Error while getting input type",
						ErrorType::Internal);

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		m_WriterLib->setFormatType(CSV::EStreamType::Signal);
		m_StreamDecoder = new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
	{
		m_WriterLib->setFormatType(CSV::EStreamType::StreamedMatrix);
		m_StreamDecoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
	{
		m_WriterLib->setFormatType(CSV::EStreamType::FeatureVector);
		m_StreamDecoder = new OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else if (m_TypeIdentifier == OV_TypeId_Spectrum)
	{
		m_WriterLib->setFormatType(CSV::EStreamType::Spectrum);
		m_StreamDecoder = new OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmOVCSVFileWriter>(*this, 0);
	}
	else { OV_ERROR_KRF("Input is a type derived from matrix that the box doesn't recognize", ErrorType::BadInput); }

	OV_ERROR_UNLESS_KRF(m_StimulationDecoder.initialize(*this, 1),
						"Error while stimulation decoder initialization",
						ErrorType::Internal);


	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_WriterLib->setOutputFloatPrecision(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));
	m_AppendData     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_LastMatrixOnly = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	m_WriterLib->setLastMatrixOnlyMode(m_LastMatrixOnly);

	if (!m_AppendData)
	{
		OV_ERROR_UNLESS_KRF(m_WriterLib->openFile(filename.toASCIIString(), OpenViBE::CSV::EFileAccessMode::Write),
							(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
								"Details: " + m_WriterLib->getLastErrorString())).c_str(),
							ErrorType::Internal);
		m_WriteHeader = true;
	}
	else
	{
		FILE* file = FS::Files::open(filename, "r");

		if (file)
		{
			fseek(file, 0, SEEK_END);
			m_WriteHeader = ftell(file) == 0;
			fclose(file);
		}

		OV_ERROR_UNLESS_KRF(m_WriterLib->openFile(filename.toASCIIString(), OpenViBE::CSV::EFileAccessMode::Append),
							(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
								"Details: " + m_WriterLib->getLastErrorString())).c_str(),
							ErrorType::Internal);
	}

	m_IsHeaderReceived = false;

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::uninitialize()
{
	m_StreamDecoder.uninitialize();
	m_StimulationDecoder.uninitialize();

	OV_ERROR_UNLESS_KRF(m_WriterLib->noEventsUntilDate(std::numeric_limits<double>::max()),
						(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" : "Details: "
							+ m_WriterLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(m_WriterLib->writeAllDataToFile(),
						(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" : "Details: "
							+ m_WriterLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(m_WriterLib->closeFile(),
						(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" : "Details: "
							+ m_WriterLib->getLastErrorString())).c_str(),
						ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::processInput(const uint32_t /*index*/)
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
	if (!m_LastMatrixOnly)
	{
		OV_ERROR_UNLESS_KRF(m_WriterLib->writeDataToFile(),
							(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
								"Details: " + m_WriterLib->getLastErrorString())).c_str(),
							ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::processStreamedMatrix()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (uint32_t index = 0; index < dynamicBoxContext.getInputChunkCount(0); index++)
	{
		OV_ERROR_UNLESS_KRF(m_StreamDecoder.decode(index),
							"Failed to decode chunk",
							ErrorType::Internal);

		// represents the properties of the input, no data
		const IMatrix* matrix = m_StreamDecoder.getOutputMatrix();

		if (m_StreamDecoder.isHeaderReceived())
		{
			OV_ERROR_UNLESS_KRF(!m_IsHeaderReceived,
								"Multiple streamed matrix headers received",
								ErrorType::BadInput);
			m_IsHeaderReceived = true;

			if (m_TypeIdentifier == OV_TypeId_Signal)
			{
				OV_ERROR_UNLESS_KRF(m_StreamDecoder.getOutputSamplingRate() != 0,
									"Sampling rate can not be 0", ErrorType::BadInput);
				std::vector<std::string> dimensionLabels;

				for (uint32_t j = 0; j < matrix->getDimensionSize(0); j++) { dimensionLabels.push_back(matrix->getDimensionLabel(0, j)); }

				OV_ERROR_UNLESS_KRF(
					m_WriterLib->setSignalInformation(dimensionLabels, uint32_t(m_StreamDecoder.getOutputSamplingRate()), matrix->getDimensionSize(1)),
					(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" : "Details: " +
						m_WriterLib->getLastErrorString())).c_str(),
					ErrorType::Internal);

				if (m_WriteHeader)
				{
					OV_ERROR_UNLESS_KRF(m_WriterLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ?
											"" : "Details: " + m_WriterLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
			{
				std::vector<std::string> dimensionLabels;
				std::vector<uint32_t> dimensionSizes;

				for (uint32_t dimensionIndex = 0; dimensionIndex < matrix->getDimensionCount(); dimensionIndex++)
				{
					dimensionSizes.push_back(matrix->getDimensionSize(dimensionIndex));

					for (uint32_t j = 0; j < matrix->getDimensionSize(dimensionIndex); j++)
					{
						dimensionLabels.push_back(matrix->getDimensionLabel(dimensionIndex, j));
					}
				}

				OV_ERROR_UNLESS_KRF(m_WriterLib->setStreamedMatrixInformation(dimensionSizes, dimensionLabels),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
										"Details: " + m_WriterLib->getLastErrorString())).c_str(),
									ErrorType::Internal);

				if (m_WriteHeader)
				{
					OV_ERROR_UNLESS_KRF(m_WriterLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ?
											"" : "Details: " + m_WriterLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
			{
				std::vector<std::string> channelsLabels;

				for (uint32_t j = 0; j < matrix->getDimensionSize(0); j++) { channelsLabels.push_back(matrix->getDimensionLabel(0, j)); }

				OV_ERROR_UNLESS_KRF(m_WriterLib->setFeatureVectorInformation(channelsLabels),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
										"Details: " + m_WriterLib->getLastErrorString())).c_str(),
									ErrorType::Internal);

				if (m_WriteHeader)
				{
					OV_ERROR_UNLESS_KRF(m_WriterLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ?
											"" : "Details: " + m_WriterLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_TypeIdentifier == OV_TypeId_Spectrum)
			{
				const IMatrix* frequencyAbscissaMatrix = m_StreamDecoder.getOutputFrequencyAbcissa();
				std::vector<std::string> channelsLabels;
				std::vector<double> frequencyAbscissa;

				for (uint32_t j = 0; j < matrix->getDimensionSize(0); j++) { channelsLabels.push_back(matrix->getDimensionLabel(0, j)); }

				for (uint32_t j = 0; j < frequencyAbscissaMatrix->getDimensionSize(0); j++)
				{
					frequencyAbscissa.push_back(frequencyAbscissaMatrix->getBuffer()[j]);
				}

				OV_ERROR_UNLESS_KRF(m_WriterLib->setSpectrumInformation(channelsLabels, frequencyAbscissa, uint32_t(m_StreamDecoder.getOutputSamplingRate())),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
										"Details: " + m_WriterLib->getLastErrorString())).c_str(),
									ErrorType::Internal);

				if (m_WriteHeader)
				{
					OV_ERROR_UNLESS_KRF(m_WriterLib->writeHeaderToFile(),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ?
											"" : "Details: " + m_WriterLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
		}

		if (m_StreamDecoder.isBufferReceived())
		{
			const IMatrix* imatrix = m_StreamDecoder.getOutputMatrix();

			if (m_TypeIdentifier == OV_TypeId_Signal)
			{
				const uint64_t samplingFrequency = m_StreamDecoder.getOutputSamplingRate();
				const uint64_t chunkStartTime    = dynamicBoxContext.getInputChunkStartTime(0, index);
				const uint32_t nChannel      = matrix->getDimensionSize(0);
				const uint32_t nSample       = matrix->getDimensionSize(1);

				for (uint32_t sampleIndex = 0; sampleIndex < nSample; sampleIndex++)
				{
					std::vector<double> matrixValues;
					// get starting and ending time

					const uint64_t timeOfNthSample       = TimeArithmetics::sampleCountToTime(samplingFrequency, sampleIndex); // assuming chunk start is 0
					const uint64_t sampleTime            = chunkStartTime + timeOfNthSample;
					const double startTime               = double(TimeArithmetics::timeToSeconds(sampleTime));
					const uint64_t timeOfNthAndOneSample = TimeArithmetics::sampleCountToTime(samplingFrequency, sampleIndex + 1);
					const double endTime                 = double(TimeArithmetics::timeToSeconds(chunkStartTime + timeOfNthAndOneSample));

					// get matrix values
					for (uint32_t channelIndex = 0; channelIndex < nChannel; channelIndex++)
					{
						matrixValues.push_back(imatrix->getBuffer()[channelIndex * nSample + sampleIndex]);
					}

					// add sample to the library
					OV_ERROR_UNLESS_KRF(m_WriterLib->addSample({ startTime, endTime, matrixValues, m_Epoch }),
										(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ?
											"" : "Details: " + m_WriterLib->getLastErrorString())).c_str(),
										ErrorType::Internal);
				}
			}
			else if (m_TypeIdentifier == OV_TypeId_StreamedMatrix || m_TypeIdentifier == OV_TypeId_CovarianceMatrix)
			{
				const double startTime = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkStartTime(0, index));
				const double endTime   = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(0, index));
				const std::vector<double> streamedMatrixValues(matrix->getBuffer(), matrix->getBuffer() + matrix->getBufferElementCount());

				OV_ERROR_UNLESS_KRF(m_WriterLib->addSample({ startTime, endTime, streamedMatrixValues, m_Epoch }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
										"Details: " + m_WriterLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}
			else if (m_TypeIdentifier == OV_TypeId_FeatureVector)
			{
				const double startTime = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkStartTime(0, index));
				const double endTime   = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(0, index));
				const IMatrix* zmatrix = m_StreamDecoder.getOutputMatrix();

				const std::vector<double> streamedMatrixValues(zmatrix->getBuffer(), zmatrix->getBuffer() + zmatrix->getBufferElementCount());

				OV_ERROR_UNLESS_KRF(m_WriterLib->addSample({ startTime, endTime, streamedMatrixValues, m_Epoch }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
										"Details: " + m_WriterLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}
			else if (m_TypeIdentifier == OV_TypeId_Spectrum)
			{
				const double startTime = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkStartTime(0, index));
				const double endTime   = TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(0, index));
				const std::vector<double> streamedMatrixValues(matrix->getBuffer(), matrix->getBuffer() + matrix->getBufferElementCount());

				OV_ERROR_UNLESS_KRF(m_WriterLib->addSample({ startTime, endTime, streamedMatrixValues, m_Epoch }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
										"Details: " + m_WriterLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}

			m_Epoch++;
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
	for (uint32_t index = 0; index < dynamicBoxContext.getInputChunkCount(1); index++)
	{
		OV_ERROR_UNLESS_KRF(m_StimulationDecoder.decode(index),
							"Failed to decode stimulation chunk",
							ErrorType::Internal);

		if (m_StimulationDecoder.isBufferReceived())
		{
			const IStimulationSet* stimulationSet = m_StimulationDecoder.getOutputStimulationSet();
			// for each stimulation, get its informations

			for (uint64_t stimulationIndex = 0; stimulationIndex < stimulationSet->getStimulationCount(); stimulationIndex++)
			{
				OV_ERROR_UNLESS_KRF(m_WriterLib->addEvent({ stimulationSet->getStimulationIdentifier(stimulationIndex),
										TimeArithmetics::timeToSeconds(stimulationSet->getStimulationDate(stimulationIndex)),
										TimeArithmetics::timeToSeconds(stimulationSet->getStimulationDuration(stimulationIndex)) }),
									(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" :
										"Details: " + m_WriterLib->getLastErrorString())).c_str(),
									ErrorType::Internal);
			}
			
			// set NoEventUntilDate to prevent time that will be empty of stimulations until the end of the last chunk
			OV_ERROR_UNLESS_KRF(
				m_WriterLib->noEventsUntilDate(TimeArithmetics::timeToSeconds(dynamicBoxContext.getInputChunkEndTime(1, (dynamicBoxContext.getInputChunkCount(1
				) - 1)))),
				(OpenViBE::CSV::ICSVHandler::getLogError(m_WriterLib->getLastLogError()) + (m_WriterLib->getLastErrorString().empty() ? "" : "Details: " +
					m_WriterLib->getLastErrorString())).c_str(),
				ErrorType::Internal);
		}

		OV_ERROR_UNLESS_KRF(dynamicBoxContext.markInputAsDeprecated(1, index),
							"Failed to mark stimulations input as deprecated",
							ErrorType::Internal);
	}

	return true;
}
