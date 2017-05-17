
#include "ovpCBoxAlgorithmSpatialFilter.h"

#include <system/ovCMemory.h>

#include <sstream>
#include <string>
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

#if defined TARGET_HAS_ThirdPartyEIGEN
#include <Eigen/Dense>
typedef Eigen::Matrix< double , Eigen::Dynamic , Eigen::Dynamic, Eigen::RowMajor > MatrixXdRowMajor;
#endif

OpenViBE::uint32 CBoxAlgorithmSpatialFilter::loadCoefficients(const OpenViBE::CString &rCoefficients, const char c1, const char c2, uint32 nRows, uint32 nCols)
{
	// Count the number of entries
	// @Note To avoid doing a ton of subsequent memory allocations (very slow on Windows debug builds), we first count the number of entries in the vector. If the file format had specified the vector dimension, we wouldn't have to do this step.
	uint32 l_u32count = 0;
	const char *l_sPtr = rCoefficients.toASCIIString();
	while(*l_sPtr!=0)
	{
		// Skip separator characters
		while(*l_sPtr==c1 || *l_sPtr==c2)
		{
			l_sPtr++;
		}
		if(*l_sPtr==0)
		{
			break;
		}
		// Ok, we have reached something that is not NULL or separator, assume its a number
		l_u32count++;
		// Skip the normal characters
		while(*l_sPtr!=c1 && *l_sPtr!=c2 && *l_sPtr!=0)
		{
			l_sPtr++;
		}
	}

	OV_ERROR_UNLESS_KRZ(
		l_u32count == nRows*nCols,
		"Invalid computed coefficients count [" << l_u32count << "] (expected " << nRows * nCols << " coefficients)",
		OpenViBE::Kernel::ErrorType::BadProcessing
	);

	// Resize in one step for efficiency.
	m_oFilterBank.setDimensionCount(2);
	m_oFilterBank.setDimensionSize(0,nRows);
	m_oFilterBank.setDimensionSize(1,nCols);

	float64* l_pFilter = m_oFilterBank.getBuffer();

	// Ok, convert to floats
	l_sPtr = rCoefficients.toASCIIString();
	uint32 l_ui32currentIdx = 0;
	while(*l_sPtr!=0)
	{
		const int BUFFSIZE=1024;
		char l_sBuffer[BUFFSIZE];
		// Skip separator characters
		while(*l_sPtr==c1 || *l_sPtr==c2)
		{
			l_sPtr++;
		}
		if(*l_sPtr==0)
		{
			break;
		}
		// Copy the normal characters, don't exceed buffer size
		int i=0;
		while(*l_sPtr!=c1 && *l_sPtr!=c2 && *l_sPtr!=0)
		{
			if(i<BUFFSIZE-1) {
				l_sBuffer[i++] = *l_sPtr;
			}
			else
			{
				break;
			}
			l_sPtr++;
		}
		l_sBuffer[i]=0;

		OV_ERROR_UNLESS_KRZ(
			l_ui32currentIdx < l_u32count,
			"Invalid parsed coefficient number [" << l_ui32currentIdx << "] (expected maximium " << l_u32count << " coefficients)",
			OpenViBE::Kernel::ErrorType::BadProcessing
		);

		// Finally, convert
		try
		{
			l_pFilter[l_ui32currentIdx] = std::stod(l_sBuffer);
		}
		catch(const std::exception&)
		{
			const uint32 l_ui32currentRow = l_ui32currentIdx/nRows + 1;
			const uint32 l_ui32currentCol = l_ui32currentIdx%nRows + 1;

			OV_ERROR_KRZ(
				"Failed to parse coefficient number [" << l_ui32currentIdx << "] at matrix positions [" << l_ui32currentRow << "," << l_ui32currentCol << "]",
				OpenViBE::Kernel::ErrorType::BadProcessing
			);
		}

		l_ui32currentIdx++;
	}

	return l_ui32currentIdx;
}

bool CBoxAlgorithmSpatialFilter::initialize(void)
{
	const IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	m_pStreamDecoder=NULL;
	m_pStreamEncoder=NULL;

	CIdentifier l_oIdentifier;
	l_rStaticBoxContext.getInputType(0, l_oIdentifier);

	if(l_oIdentifier==OV_TypeId_StreamedMatrix)
	{
		m_pStreamDecoder=new OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmSpatialFilter >(*this, 0);
		m_pStreamEncoder=new OpenViBEToolkit::TStreamedMatrixEncoder < CBoxAlgorithmSpatialFilter >(*this, 0);
	}
	else if(l_oIdentifier==OV_TypeId_Signal)
	{
		m_pStreamDecoder=new OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmSpatialFilter >(*this, 0);
		m_pStreamEncoder=new OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmSpatialFilter >(*this, 0);

		((OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputSamplingRate().setReferenceTarget(
			((OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputSamplingRate());
	}
	else if(l_oIdentifier==OV_TypeId_Spectrum)
	{
		m_pStreamDecoder=new OpenViBEToolkit::TSpectrumDecoder < CBoxAlgorithmSpatialFilter >(*this, 0);
		m_pStreamEncoder=new OpenViBEToolkit::TSpectrumEncoder < CBoxAlgorithmSpatialFilter >(*this, 0);

		((OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputFrequencyAbscissa().setReferenceTarget(
			((OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputFrequencyAbscissa());
		((OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputSamplingRate().setReferenceTarget(
			((OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputSamplingRate());
	}
	else
	{
		OV_ERROR_KRF("Invalid input stream type [" << l_oIdentifier.toString() << "]", OpenViBE::Kernel::ErrorType::BadInput);
	}

	// If we have a filter file, use dimensions and coefficients from that. Otherwise, use box config params.
	CString l_sFilterFile = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	if(l_sFilterFile!=CString(""))
	{
		OV_ERROR_UNLESS_KRF(
			OpenViBEToolkit::Tools::Matrix::loadFromTextFile(m_oFilterBank, l_sFilterFile),
			"Failed to load filter parameters from file at location [" << l_sFilterFile << "]",
			OpenViBE::Kernel::ErrorType::BadFileRead
		);

		OV_ERROR_UNLESS_KRF(
			m_oFilterBank.getDimensionCount() == 2,
			"Invalid filter matrix in file " << l_sFilterFile << ": found [" << m_oFilterBank.getDimensionCount() << "] dimensions (expected 2 dimension)",
			OpenViBE::Kernel::ErrorType::BadConfig
		);

#if defined(DEBUG)
		OpenViBEToolkit::Tools::Matrix::saveToTextFile(m_oFilterBank, this->getConfigurationManager().expand("${Path_UserData}/spatialfilter_debug.txt"));
#endif
	}
	else
	{
		const CString l_sCoefficient=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
		// The double cast is needed until FSettingValueAutoCast supports uint32.
		const uint32 l_ui32OutputChannelCountSetting=(uint32)(uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
		const uint32 l_ui32InputChannelCountSetting=(uint32)(uint64)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
		const uint32 l_ui32nCoefficients = loadCoefficients(l_sCoefficient, ' ', OV_Value_EnumeratedStringSeparator, l_ui32OutputChannelCountSetting, l_ui32InputChannelCountSetting);

		OV_ERROR_UNLESS_KRF(
			l_ui32nCoefficients == l_ui32OutputChannelCountSetting * l_ui32InputChannelCountSetting,
			"Invalid number of coefficients [" << l_ui32nCoefficients << "] (expected "<< l_ui32OutputChannelCountSetting * l_ui32InputChannelCountSetting <<" coefficients)",
			OpenViBE::Kernel::ErrorType::BadConfig
		);

#if defined(DEBUG)
		OpenViBEToolkit::Tools::Matrix::saveToTextFile(m_oFilterBank, this->getConfigurationManager().expand("${Path_UserData}/spatialfilter_debug.txt"));
#endif
	}

	return true;
}

bool CBoxAlgorithmSpatialFilter::uninitialize(void)
{
	if(m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		delete m_pStreamDecoder;
		m_pStreamDecoder=NULL;
	}

	if(m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		delete m_pStreamEncoder;
		m_pStreamEncoder=NULL;
	}

	return true;
}

bool CBoxAlgorithmSpatialFilter::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSpatialFilter::process(void)
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{

		m_pStreamDecoder->decode(i);
		if(m_pStreamDecoder->isHeaderReceived())
		{
			// we can treat them all as matrix decoders as they all inherit from it
			const IMatrix *l_pInputMatrix = (static_cast< OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmSpatialFilter>* >(m_pStreamDecoder))->getOutputMatrix();

			const uint32 l_ui32InputChannelCount=l_pInputMatrix->getDimensionSize(0);
			const uint32 l_ui32InputSamplesCount=l_pInputMatrix->getDimensionSize(1);

			OV_ERROR_UNLESS_KRF(
				l_ui32InputChannelCount != 0 && l_ui32InputSamplesCount != 0,
				"Invalid matrix size with zero dimension on input [" << l_ui32InputChannelCount << " x " << l_ui32InputSamplesCount << "]",
				OpenViBE::Kernel::ErrorType::BadConfig
			);

			const uint32 l_ui32FilterInputChannelCount = m_oFilterBank.getDimensionSize(1);
			const uint32 l_ui32FilterOutputChannelCount = m_oFilterBank.getDimensionSize(0);

			OV_ERROR_UNLESS_KRF(
				l_ui32InputChannelCount == l_ui32FilterInputChannelCount,
				"Invalid input channel count  [" << l_ui32InputChannelCount << "] (expected " << l_ui32FilterInputChannelCount << " channel count)",
				OpenViBE::Kernel::ErrorType::BadConfig
			);

			IMatrix *l_pOutputMatrix = ((OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputMatrix();
			l_pOutputMatrix->setDimensionCount(2);
			l_pOutputMatrix->setDimensionSize(0, l_ui32FilterOutputChannelCount);
			l_pOutputMatrix->setDimensionSize(1, l_ui32InputSamplesCount);

			// Name channels
			for(uint32 i=0;i<l_pOutputMatrix->getDimensionSize(0);i++)
			{
				char l_sBuffer[64];
				sprintf(l_sBuffer, "sFiltered %d", i);
				l_pOutputMatrix->setDimensionLabel(0, i, l_sBuffer);
			}

			m_pStreamEncoder->encodeHeader();
		}
		if(m_pStreamDecoder->isBufferReceived())
		{
			const IMatrix *l_pInputMatrix = ((OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputMatrix();
			IMatrix *l_pOutputMatrix = ((OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputMatrix();

			const float64* l_pInput=l_pInputMatrix->getBuffer();
			float64* l_pOutput=l_pOutputMatrix->getBuffer();
			const uint32 l_ui32InputChannelCount=l_pInputMatrix->getDimensionSize(0);
			const uint32 l_ui32OutputChannelCount=l_pOutputMatrix->getDimensionSize(0);
			const uint32 l_ui32SampleCount=l_pInputMatrix->getDimensionSize(1);

#if defined TARGET_HAS_ThirdPartyEIGEN
			const Eigen::Map<MatrixXdRowMajor> l_oInputMapper(const_cast<float64*>(l_pInput), l_ui32InputChannelCount, l_ui32SampleCount);
			const Eigen::Map<MatrixXdRowMajor> l_oFilterMapper(const_cast<float64*>(m_oFilterBank.getBuffer()), m_oFilterBank.getDimensionSize(0),  m_oFilterBank.getDimensionSize(1));
			Eigen::Map<MatrixXdRowMajor> l_oOutputMapper(l_pOutput, l_ui32OutputChannelCount, l_ui32SampleCount);

			l_oOutputMapper = l_oFilterMapper * l_oInputMapper;
#else
			const float64* l_pFilter = m_oFilterBank.getBuffer();

			System::Memory::set(l_pOutput, l_ui32SampleCount*l_ui32OutputChannelCount*sizeof(float64), 0);

			for(uint32 j=0; j<l_ui32OutputChannelCount; j++)
			{
				for(uint32 k=0; k<l_ui32InputChannelCount; k++)
				{
					for(uint32 l=0; l<l_ui32SampleCount; l++)
					{
						l_pOutput[j*l_ui32SampleCount+l] += l_pFilter[j*l_ui32InputChannelCount+k]*l_pInput[k*l_ui32SampleCount+l];
					}
				}
			}
#endif
			m_pStreamEncoder->encodeBuffer();
		}
		if(m_pStreamDecoder->isEndReceived())
		{
			m_pStreamEncoder->encodeEnd();
		}

		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	return true;
}

