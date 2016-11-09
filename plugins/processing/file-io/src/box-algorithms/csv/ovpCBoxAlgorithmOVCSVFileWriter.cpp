#include <string>
#include <iostream>

#include <openvibe/ovITimeArithmetics.h>

#include "ovpCBoxAlgorithmOVCSVFileWriter.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

CBoxAlgorithmOVCSVFileWriter::CBoxAlgorithmOVCSVFileWriter(void)
	:
	m_StreamDecoder(nullptr)
{
}

bool CBoxAlgorithmOVCSVFileWriter::initialize(void)
{
	m_IsFileOpen = false;
	m_Epoch = 0;
	this->getStaticBoxContext().getInputType(0, m_TypeIdentifier);

	m_Separator = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if (m_TypeIdentifier == OV_TypeId_Signal)
	{
		m_WriterLib = OpenViBE::CSV::createCSVLib();
		m_StreamDecoder = new OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmOVCSVFileWriter >();
		OV_ERROR_UNLESS_KRF(m_StreamDecoder->initialize(*this, 0),
			"Error while stream decoder initialization",
			ErrorType::Internal);
		m_WriterLib->setFormatType(OpenViBE::CSV::EStreamType::Signal);
	}
	else
	{
		OV_ERROR_KRF("Input is a type derived from matrix that the box doesn't recognize", ErrorType::BadInput);
	}

	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	OV_ERROR_UNLESS_KRF(m_WriterLib->openFile(filename.toASCIIString(), OpenViBE::CSV::EFileAccessMode::Write),
		OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str(),
		ErrorType::Internal);

	m_IsHeaderReceived = false;

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::uninitialize(void)
{
	if (!m_WriterLib->closeFile())
	{
		OV_FATAL_K(OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << ": " << m_WriterLib->getLastErrorString().c_str(), ErrorType::Internal);
	}
	OpenViBE::CSV::releaseCSVLib(m_WriterLib);
	OV_ERROR_UNLESS_KRF(m_StreamDecoder->uninitialize(),
		"Error have been thrown error while stream decoder unitialize",
		ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::processInput(unsigned int inputIndex)
{
	OV_ERROR_UNLESS_KRF(getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(),
		"Error while marking algorithm as ready to process",
		ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::process(void)
{
	return processStreamedMatrix();
}

bool CBoxAlgorithmOVCSVFileWriter::processStreamedMatrix(void)
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (unsigned int index = 0; index < dynamicBoxContext.getInputChunkCount(0); index++)
	{
		m_StreamDecoder->decode(index);
		// represents the properties of the input, no data
		const IMatrix* matrix = static_cast<OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmOVCSVFileWriter >*>(m_StreamDecoder)->getOutputMatrix();

		if (m_StreamDecoder->isHeaderReceived())
		{
			OV_ERROR_UNLESS_KRF(!m_IsHeaderReceived,
				"Multiple streamed matrix headers received",
				ErrorType::BadInput);
			m_IsHeaderReceived = true;

			if (m_TypeIdentifier == OV_TypeId_Signal)
			{
				std::vector<std::string> dimensionLabels;
				for (size_t index = 0; index < matrix->getDimensionSize(0); index++)
				{
					dimensionLabels.push_back(matrix->getDimensionLabel(0, index));
				}

				OV_ERROR_UNLESS_KRF(m_WriterLib->setSignalInformation(dimensionLabels, static_cast<unsigned int>((static_cast<OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmOVCSVFileWriter>*>(m_StreamDecoder))->getOutputSamplingRate()), matrix->getDimensionSize(1)),
					OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str(),
					ErrorType::Internal);

				OV_ERROR_UNLESS_KRF(m_WriterLib->writeHeaderToFile(),
					OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str(),
					ErrorType::Internal);
			}
		}

		if (m_StreamDecoder->isBufferReceived())
		{
			const IMatrix* imatrix = static_cast<OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmOVCSVFileWriter >*>(m_StreamDecoder)->getOutputMatrix();
			std::vector<double> matrixValues;

			const unsigned long long chunkStartTime = dynamicBoxContext.getInputChunkStartTime(0, index);
			const unsigned long long chunkEndTime = dynamicBoxContext.getInputChunkEndTime(0, index);
			const unsigned int numChannels = matrix->getDimensionSize(0);
			const unsigned int numSamples = matrix->getDimensionSize(1);

			double lastTime = 0.0;
			for (unsigned int sampleIndex = 0; sampleIndex < numSamples; sampleIndex++)
			{
				double startTime = 0.0;
				double endTime = 0.0;

				// get starting and ending time
				if (m_TypeIdentifier == OV_TypeId_Signal)
				{
					const unsigned long long samplingFrequency = static_cast<OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmOVCSVFileWriter >*>(m_StreamDecoder)->getOutputSamplingRate();
					unsigned long long timeOfNthSample = ITimeArithmetics::sampleCountToTime(samplingFrequency, sampleIndex); // assuming chunk start is 0
					unsigned long long sampleTime = chunkStartTime + timeOfNthSample;
					startTime = ITimeArithmetics::timeToSeconds(sampleTime);
					sampleTime = chunkEndTime + timeOfNthSample;
					endTime = ITimeArithmetics::timeToSeconds(sampleTime);
				}

				// get matrix values
				for (unsigned int channelIndex = 0; channelIndex < numChannels; channelIndex++)
				{
					matrixValues.push_back(imatrix->getBuffer()[channelIndex*numSamples + sampleIndex]);
				}

				// add sample to the library
				OV_ERROR_UNLESS_KRF(m_WriterLib->addSample({ startTime, endTime, matrixValues, m_Epoch }),
					OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str(),
					ErrorType::Internal);

				matrixValues.clear();
				lastTime = endTime;
			}

			// write into the library
			OV_ERROR_UNLESS_KRF(m_WriterLib->noEventsUntilDate(lastTime + 0.5),
				OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str(),
				ErrorType::Internal);
			OV_ERROR_UNLESS_KRF(m_WriterLib->writeDataToFile(),
				OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str(),
				ErrorType::Internal);

			m_Epoch++;
		}

		OV_ERROR_UNLESS_KRF(dynamicBoxContext.markInputAsDeprecated(0, index),
			"Fail to mark input as deprecated",
			ErrorType::Internal);
	}

	return true;
}
