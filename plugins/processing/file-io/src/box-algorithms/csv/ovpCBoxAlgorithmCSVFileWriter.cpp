#include "ovpCBoxAlgorithmCSVFileWriter.h"

#include <string>
#include <iostream>

#include "openvibe/ovTimeArithmetics.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace FileIO;

bool CBoxAlgorithmCSVFileWriter::initialize()
{
	this->getStaticBoxContext().getInputType(0, m_typeID);

	m_sSeparator = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if (this->getTypeManager().isDerivedFromStream(m_typeID, OV_TypeId_StreamedMatrix))
	{
		if (m_typeID == OV_TypeId_Signal)
		{
			m_pStreamDecoder = new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmCSVFileWriter>();
			m_pStreamDecoder->initialize(*this, 0);
		}
			//else if(m_typeID==OV_TypeId_Spectrum)
			//{
			//	m_pStreamDecoder=new OpenViBEToolkit::TSpectrumDecoder < CBoxAlgorithmCSVFileWriter >();
			//	m_pStreamDecoder->initialize(*this,0);
			//}
		else if (m_typeID == OV_TypeId_FeatureVector)
		{
			m_pStreamDecoder = new OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmCSVFileWriter>();
			m_pStreamDecoder->initialize(*this, 0);
		}
		else
		{
			if (m_typeID != OV_TypeId_StreamedMatrix)
			{
				this->getLogManager() << LogLevel_Info << "Input is a type derived from matrix that the box doesn't recognize, decoding as Streamed Matrix\n";
			}
			m_pStreamDecoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmCSVFileWriter>();
			m_pStreamDecoder->initialize(*this, 0);
		}
		m_fpRealProcess = &CBoxAlgorithmCSVFileWriter::processStreamedMatrix;
	}
	else if (m_typeID == OV_TypeId_Stimulations)
	{
		m_pStreamDecoder = new OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmCSVFileWriter>();
		m_pStreamDecoder->initialize(*this, 0);
		m_fpRealProcess = &CBoxAlgorithmCSVFileWriter::processStimulation;
	}
	else { OV_ERROR_KRF("Invalid input type identifier " << this->getTypeManager().getTypeName(m_typeID), OpenViBE::Kernel::ErrorType::BadInput); }

	m_nSample = 0;

	m_bFirstBuffer    = true;
	m_bHeaderReceived = false;

	return true;
}

bool CBoxAlgorithmCSVFileWriter::uninitialize()
{
	if (m_oFileStream.is_open()) { m_oFileStream.close(); }

	if (m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		delete m_pStreamDecoder;
	}

	return true;
}

bool CBoxAlgorithmCSVFileWriter::initializeFile()
{
	const CString l_sFilename      = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const uint64_t l_ui64Precision = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	m_oFileStream.open(l_sFilename.toASCIIString(), std::ios::trunc);

	OV_ERROR_UNLESS_KRF(m_oFileStream.is_open(), "Error opening file [" << l_sFilename << "] for writing", OpenViBE::Kernel::ErrorType::BadFileWrite);

	m_oFileStream << std::scientific;
	m_oFileStream.precision(std::streamsize(l_ui64Precision));

	return true;
}

bool CBoxAlgorithmCSVFileWriter::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmCSVFileWriter::process()
{
	if (!m_oFileStream.is_open()) { if (!initializeFile()) { return false; } }
	return (this->*m_fpRealProcess)();
}

bool CBoxAlgorithmCSVFileWriter::processStreamedMatrix()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		const uint64_t tStart = boxContext.getInputChunkStartTime(0, i);
		const uint64_t tEnd   = boxContext.getInputChunkEndTime(0, i);

		m_pStreamDecoder->decode(i);

		if (m_pStreamDecoder->isHeaderReceived())
		{
			if (!m_bHeaderReceived)
			{
				m_bHeaderReceived = true;

				const IMatrix* l_pMatrix = static_cast<OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmCSVFileWriter>*>(m_pStreamDecoder)->getOutputMatrix();

				OV_ERROR_UNLESS_KRF(l_pMatrix->getDimensionCount() == 1 || l_pMatrix->getDimensionCount() == 2,
									"Invalid input matrix: must have 1 or 2 dimensions", ErrorType::BadInput);

				if (l_pMatrix->getDimensionCount() == 1 || m_typeID == OV_TypeId_FeatureVector)
				{
					// The matrix is a vector, make a matrix to represent it
					m_oMatrix.setDimensionCount(2);

					// This [n X 1] will get written as a single row due to transpose later
					m_oMatrix.setDimensionSize(0, l_pMatrix->getDimensionSize(0));
					m_oMatrix.setDimensionSize(1, 1);
					for (uint32_t j = 0; j < l_pMatrix->getDimensionSize(0); j++) { m_oMatrix.setDimensionLabel(0, j, l_pMatrix->getDimensionLabel(0, j)); }
				}
				else
				{
					// As-is
					OpenViBEToolkit::Tools::Matrix::copyDescription(m_oMatrix, *l_pMatrix);
				}
				//			std::cout<<&m_Matrix<<" "<<&op_pMatrix<<"\n";
				m_oFileStream << "Time (s)";
				for (uint32_t c = 0; c < m_oMatrix.getDimensionSize(0); c++)
				{
					std::string l_sLabel(m_oMatrix.getDimensionLabel(0, c));
					while (l_sLabel.length() > 0 && l_sLabel[l_sLabel.length() - 1] == ' ') { l_sLabel.erase(l_sLabel.length() - 1); }
					m_oFileStream << m_sSeparator.toASCIIString() << l_sLabel.c_str();
				}

				if (m_typeID == OV_TypeId_Signal) { m_oFileStream << m_sSeparator.toASCIIString() << "Sampling Rate"; }
				else if (m_typeID == OV_TypeId_Spectrum)
				{
					m_oFileStream << m_sSeparator << "Min frequency band";
					m_oFileStream << m_sSeparator << "Max frequency band";
				}
				else { }

				m_oFileStream << "\n";
			}
			else { OV_ERROR_KRF("Multiple streamed matrix headers received", ErrorType::BadInput); }
		}
		if (m_pStreamDecoder->isBufferReceived())
		{
			const IMatrix* l_pMatrix = static_cast<OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmCSVFileWriter>*>(m_pStreamDecoder)->getOutputMatrix();

			const uint32_t l_ui32NumChannels = m_oMatrix.getDimensionSize(0);
			const uint32_t l_ui32NumSamples  = m_oMatrix.getDimensionSize(1);

			//this->getLogManager() << LogLevel_Info << " dimsIn " << l_pMatrix->getDimensionSize(0) << "," << l_pMatrix->getDimensionSize(1) << "\n";
			//this->getLogManager() << LogLevel_Info << " dimsBuf " << m_oMatrix.getDimensionSize(0) << "," << m_oMatrix.getDimensionSize(1) << "\n";

			for (uint32_t s = 0; s < l_ui32NumSamples; s++)
			{
				if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_FeatureVector)
				{
					m_oFileStream << TimeArithmetics::timeToSeconds(tStart);
				}
				else if (m_typeID == OV_TypeId_Signal)
				{
					const uint64_t l_ui64SamplingFrequency = static_cast<OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmCSVFileWriter>*>(m_pStreamDecoder)->
							getOutputSamplingRate();
					const uint64_t l_ui64TimeOfNthSample = TimeArithmetics::sampleCountToTime(l_ui64SamplingFrequency, s); // assuming chunk start is 0
					const uint64_t l_ui64SampleTime      = tStart + l_ui64TimeOfNthSample;

					m_oFileStream << TimeArithmetics::timeToSeconds(l_ui64SampleTime);
				}
				else if (m_typeID == OV_TypeId_Spectrum) { m_oFileStream << TimeArithmetics::timeToSeconds(tEnd); }
				for (uint32_t c = 0; c < l_ui32NumChannels; c++)
				{
					m_oFileStream << m_sSeparator.toASCIIString() << l_pMatrix->getBuffer()[c * l_ui32NumSamples + s];
				}

				if (m_bFirstBuffer)
				{
					if (m_typeID == OV_TypeId_Signal)
					{
						const uint64_t l_ui64SamplingFrequency = static_cast<OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmCSVFileWriter>*>(m_pStreamDecoder)->
								getOutputSamplingRate();

						m_oFileStream << m_sSeparator.toASCIIString() << uint64_t(l_ui64SamplingFrequency);

						m_bFirstBuffer = false;
					}
					else if (m_typeID == OV_TypeId_Spectrum)
					{
						// This should not be supported anymore
						// This is not the correct formula
						const IMatrix* freq = static_cast<OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmCSVFileWriter>*>(m_pStreamDecoder)->getOutputFrequencyAbscissa();
						const double half = s > 0 ? (freq->getBuffer()[s] - freq->getBuffer()[s - 1]) / 2. : (freq->getBuffer()[s + 1] - freq->getBuffer()[s]) / 2.;
						m_oFileStream << m_sSeparator.toASCIIString() << (freq->getBuffer()[s] - half);
						m_oFileStream << m_sSeparator.toASCIIString() << (freq->getBuffer()[s] + half);
					}
					else { }
				}
				else
				{
					if (m_typeID == OV_TypeId_Signal) { m_oFileStream << m_sSeparator.toASCIIString(); }
					else if (m_typeID == OV_TypeId_Spectrum) { m_oFileStream << m_sSeparator.toASCIIString() << m_sSeparator.toASCIIString(); }
					else { }
				}

				m_oFileStream << "\n";
			}
			m_nSample += l_ui32NumSamples;

			m_bFirstBuffer = false;
		}
		if (m_pStreamDecoder->isEndReceived()) { }
		boxContext.markInputAsDeprecated(0, i);
	}

	return true;
}

bool CBoxAlgorithmCSVFileWriter::processStimulation()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		m_pStreamDecoder->decode(i);
		if (m_pStreamDecoder->isHeaderReceived())
		{
			if (!m_bHeaderReceived)
			{
				m_bHeaderReceived = true;
				m_oFileStream << "Time (s)" << m_sSeparator.toASCIIString() << "Identifier" << m_sSeparator.toASCIIString() << "Duration\n";
			}
			else { OV_ERROR_KRF("Multiple stimulation headers received", ErrorType::BadInput); }
		}
		if (m_pStreamDecoder->isBufferReceived())
		{
			const IStimulationSet* stimSet = dynamic_cast<OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmCSVFileWriter>*>(m_pStreamDecoder)->getOutputStimulationSet();
			for (size_t j = 0; j < stimSet->getStimulationCount(); j++)
			{
				m_oFileStream << TimeArithmetics::timeToSeconds(stimSet->getStimulationDate(j))
						<< m_sSeparator.toASCIIString() << stimSet->getStimulationIdentifier(j) << m_sSeparator.toASCIIString()
						<< TimeArithmetics::timeToSeconds(stimSet->getStimulationDuration(j)) << "\n";
			}
		}
		if (m_pStreamDecoder->isEndReceived()) { }
		boxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
