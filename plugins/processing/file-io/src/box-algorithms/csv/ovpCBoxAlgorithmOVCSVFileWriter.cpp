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
		if (!m_StreamDecoder->initialize(*this, 0))
		{
			OV_ERROR_KRF("Error while stream decoder initialization\n", ErrorType::Internal);
		}
		m_WriterLib->setFormatType(OpenViBE::CSV::EStreamType::Signal);
	}
	else
	{
		OV_ERROR_KRF("Input is a type derived from matrix that the box doesn't recognize\n", ErrorType::BadInput);
	}

	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	if (!m_WriterLib->openFile(filename.toASCIIString(), OpenViBE::CSV::EFileAccessMode::Write))
	{
		OV_ERROR_KRF(OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n", ErrorType::Internal);
	}
	else
	{
		this->getLogManager() << LogLevel_Info << "File written in: " << filename.toASCIIString() << "\n";
	}

	m_IsHeaderReceived = false;

	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::uninitialize(void)
{
	if (!m_WriterLib->closeFile())
	{
		this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << ": " << m_WriterLib->getLastErrorString().c_str() << "\n";
	}

	OpenViBE::CSV::releaseCSVLib(m_WriterLib);
	if (!m_StreamDecoder->uninitialize())
	{
		OV_ERROR_KRF("Error have been thrown error while stream decoder unitialize\n", ErrorType::Internal);
	}
	return true;
}

bool CBoxAlgorithmOVCSVFileWriter::processInput(unsigned int inputIndex)
{
	if (!getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess())
	{
		OV_ERROR_KRF("Error while marling algorithm as ready to process\n", ErrorType::Internal);
	}
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
			if (!m_IsHeaderReceived)
			{
				m_IsHeaderReceived = true;

				if (m_TypeIdentifier == OV_TypeId_Signal)
				{
					std::vector<std::string> dimensionLabels;
					for (size_t index = 0; index < matrix->getDimensionSize(0); index++)
					{
						dimensionLabels.push_back(matrix->getDimensionLabel(0, index));
					}

					if (!m_WriterLib->setSignalInformation(dimensionLabels, static_cast<unsigned int>(((OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmOVCSVFileWriter>*)m_StreamDecoder)->getOutputSamplingRate()), matrix->getDimensionSize(1)))
					{
						OV_ERROR_KRF(OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n", ErrorType::Internal);
					}

					if (!m_WriterLib->writeHeaderToFile())
					{
						OV_ERROR_KRF(OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n", ErrorType::Internal);
					}
				}
			}
			else
			{
				OV_ERROR_KRF("Multiple streamed matrix headers received\n", ErrorType::BadInput);
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
				if (!m_WriterLib->addSample({ startTime, endTime, matrixValues, m_Epoch }))
				{
					OV_ERROR_KRF(OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n", ErrorType::Internal);
				}

				matrixValues.clear();
				lastTime = endTime;
			}

			// write into the library
			if (!m_WriterLib->noEventsUntilDate(lastTime + 0.5))
			{
				OV_ERROR_KRF(OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n", ErrorType::Internal);
			}

			if (!m_WriterLib->writeDataToFile())
			{
				OV_ERROR_KRF(OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n", ErrorType::Internal);
			}

			m_Epoch++;
		}

		if (!dynamicBoxContext.markInputAsDeprecated(0, index))
		{
			return false;
		}
	}

	return true;
}
