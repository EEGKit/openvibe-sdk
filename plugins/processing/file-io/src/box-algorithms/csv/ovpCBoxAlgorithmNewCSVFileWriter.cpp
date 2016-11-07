#include "ovpCBoxAlgorithmNewCSVFileWriter.h"

#include <string>
#include <iostream>

#include "openvibe/ovITimeArithmetics.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

CBoxAlgorithmNewCSVFileWriter::CBoxAlgorithmNewCSVFileWriter(void)
	:
	m_RealProcess(nullptr)
	, m_StreamDecoder(nullptr)
{
}

bool CBoxAlgorithmNewCSVFileWriter::initialize(void)
{
	m_IsFileOpen = false;
	m_Epoch = 0;
	m_WriterLib = OpenViBE::CSV::createCSVLib();
	this->getStaticBoxContext().getInputType(0, m_TypeIdentifier);

	m_Separator = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if (this->getTypeManager().isDerivedFromStream(m_TypeIdentifier, OV_TypeId_StreamedMatrix))
	{
		if (m_TypeIdentifier == OV_TypeId_Signal)
		{
			m_StreamDecoder = new OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmNewCSVFileWriter >();
			m_StreamDecoder->initialize(*this, 0);
			m_WriterLib->setFormatType(OpenViBE::CSV::EStreamType::Signal);
		}
		else
		{
			this->getLogManager() << LogLevel_Info << "Input is a type derived from matrix that the box doesn't recognize\n";
			return false;
		}
		m_RealProcess = &CBoxAlgorithmNewCSVFileWriter::processStreamedMatrix;
	}
	else
	{
		this->getLogManager() << LogLevel_Error << "this input type identifier (" << this->getTypeManager().getTypeName(m_TypeIdentifier) << ") is an invalid input type identifier\n";
		return false;
	}

	m_IsHeaderReceived = false;

	return true;
}

bool CBoxAlgorithmNewCSVFileWriter::uninitialize(void)
{
	if (!m_WriterLib->closeFile())
	{
		this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << ": " << m_WriterLib->getLastErrorString().c_str() << "\n";
		return false;
	}

	OpenViBE::CSV::releaseCSVLib(m_WriterLib);
	return true;
}

bool CBoxAlgorithmNewCSVFileWriter::initializeFile()
{
	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	if (!m_WriterLib->openFile(filename.toASCIIString(), OpenViBE::CSV::EFileAccessMode::Write))
	{
		this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n";
		return false;
	}
	else
	{
		this->getLogManager() << LogLevel_Info << "File written in: " << filename.toASCIIString() << "\n";
	}

	return true;
}

bool CBoxAlgorithmNewCSVFileWriter::processInput(unsigned int inputIndex)
{
	if (!getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess())
	{
		return false;
	}
	return true;
}

bool CBoxAlgorithmNewCSVFileWriter::process(void)
{
	if (!m_IsFileOpen)
	{
		if (!initializeFile())
		{
			return false;
		}
		m_IsFileOpen = true;
	}
	return (this->*m_RealProcess)();
}

bool CBoxAlgorithmNewCSVFileWriter::processStreamedMatrix(void)
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	for (unsigned int index = 0; index < dynamicBoxContext.getInputChunkCount(0); index++)
	{
		m_StreamDecoder->decode(index);

		if (m_StreamDecoder->isHeaderReceived())
		{
			if (!m_IsHeaderReceived)
			{
				m_IsHeaderReceived = true;

				const IMatrix* matrix = ((OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmNewCSVFileWriter >*)m_StreamDecoder)->getOutputMatrix();

				if (m_TypeIdentifier != OV_TypeId_FeatureVector)
				{
					OpenViBEToolkit::Tools::Matrix::copyDescription(m_Matrix, *matrix);
				}

				if (m_TypeIdentifier == OV_TypeId_Signal)
				{
					std::vector<std::string> dimensionLabels;
					for (size_t index = 0; index < matrix->getDimensionSize(0); index++)
					{
						dimensionLabels.push_back(matrix->getDimensionLabel(0, index));
					}

					if (!m_WriterLib->setSignalInformation(dimensionLabels, static_cast<unsigned int>(((OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmNewCSVFileWriter>*)m_StreamDecoder)->getOutputSamplingRate()), matrix->getDimensionSize(1)))
					{
						this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n";
						return false;
					}

					if (!m_WriterLib->writeHeaderToFile())
					{
						this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n";
						return false;
					}
				}
			}
			else
			{
				OV_ERROR_KRF("Multiple streamed matrix headers received", ErrorType::BadInput);
				return false;
			}
		}

		if (m_StreamDecoder->isBufferReceived())
		{
			const IMatrix* imatrix = ((OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmNewCSVFileWriter >*)m_StreamDecoder)->getOutputMatrix();
			std::vector<double> matrix;

			const unsigned long long startTime = dynamicBoxContext.getInputChunkStartTime(0, index);
			const unsigned long long endTime = dynamicBoxContext.getInputChunkEndTime(0, index);
			const unsigned int numChannels = m_Matrix.getDimensionSize(0);
			const unsigned int numSamples = m_Matrix.getDimensionSize(1);

			double lastTime = 0.0;
			for (unsigned int sampleCount = 0; sampleCount < numSamples; sampleCount++)
			{
				double startingTime = 0.0;
				double endingTime = 0.0;

				// get starting and ending time
				if (m_TypeIdentifier == OV_TypeId_Signal)
				{
					const unsigned long long samplingFrequency = ((OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmNewCSVFileWriter >*)m_StreamDecoder)->getOutputSamplingRate();
					unsigned long long timeOfNthSample = ITimeArithmetics::sampleCountToTime(samplingFrequency, sampleCount); // assuming chunk start is 0
					unsigned long long sampleTime = startTime + timeOfNthSample;
					startingTime = ITimeArithmetics::timeToSeconds(sampleTime);
					sampleTime = endTime + timeOfNthSample;
					endingTime = ITimeArithmetics::timeToSeconds(sampleTime);
				}

				// get matrix values
				for (unsigned int counter = 0; counter < numChannels; counter++)
				{
					matrix.push_back(imatrix->getBuffer()[counter*numSamples + sampleCount]);
				}

				// add sample to the library
				if (!m_WriterLib->addSample({ startingTime, endingTime, matrix, m_Epoch }))
				{
					this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n";
					return false;
				}

				matrix.clear();
				lastTime = endingTime;
			}

			// write into the library
			if (!m_WriterLib->noEventsUntilDate(lastTime + 0.5))
			{
				this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n";
				return false;
			}

			if (!m_WriterLib->writeDataToFile())
			{
				this->getLogManager() << LogLevel_Error << OpenViBE::CSV::ICSVLib::getLogError(m_WriterLib->getLastLogError()).c_str() << "\n";
				return false;
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
