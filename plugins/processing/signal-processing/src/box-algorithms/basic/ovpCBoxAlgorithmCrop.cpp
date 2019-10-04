#include "ovpCBoxAlgorithmCrop.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmCrop::initialize()
{
	CIdentifier inputTypeID;
	getStaticBoxContext().getInputType(0, inputTypeID);

	if (inputTypeID == OV_TypeId_StreamedMatrix)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	}
	else if (inputTypeID == OV_TypeId_FeatureVector)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
	}
	else if (inputTypeID == OV_TypeId_Signal)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	}
	else if (inputTypeID == OV_TypeId_Spectrum)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
	}
	else { return false; }
	m_pStreamDecoder->initialize();
	m_pStreamEncoder->initialize();

	if (inputTypeID == OV_TypeId_StreamedMatrix) { }
	else if (inputTypeID == OV_TypeId_FeatureVector) { }
	else if (inputTypeID == OV_TypeId_Signal)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(
			m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
	}
	else if (inputTypeID == OV_TypeId_Spectrum)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa)->setReferenceTarget(
			m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate)->setReferenceTarget(
			m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_SamplingRate));
	}

	m_matrix = new CMatrix();
	TParameterHandler<IMatrix*>(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix)).
			setReferenceTarget(m_matrix);
	TParameterHandler<IMatrix*>(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix)).
			setReferenceTarget(m_matrix);

	m_cropMethod  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_f64MinCropValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_f64MaxCropValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	OV_ERROR_UNLESS_KRF(m_f64MinCropValue < m_f64MaxCropValue,
						"Invalid crop values: minimum crop value [" << m_f64MinCropValue << "] should be lower than the maximum crop value [" <<
						m_f64MaxCropValue << "]",
						OpenViBE::Kernel::ErrorType::BadSetting);

	return true;
}

bool CBoxAlgorithmCrop::uninitialize()
{
	delete m_matrix;

	m_pStreamEncoder->uninitialize();
	m_pStreamDecoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
	getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

bool CBoxAlgorithmCrop::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmCrop::process()
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		TParameterHandler<const IMemoryBuffer*> l_oInputMemoryBufferHandle(
			m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<IMemoryBuffer*> l_oOutputMemoryBufferHandle(
			m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		l_oInputMemoryBufferHandle  = boxContext.getInputChunk(0, i);
		l_oOutputMemoryBufferHandle = boxContext.getOutputChunk(0);

		m_pStreamDecoder->process();
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			double* buffer = m_matrix->getBuffer();
			for (uint32_t j = 0; j < m_matrix->getBufferElementCount(); j++, buffer++)
			{
				if (*buffer < m_f64MinCropValue && (m_cropMethod == OVP_TypeId_CropMethod_Min || m_cropMethod == OVP_TypeId_CropMethod_MinMax))
				{
					*buffer = m_f64MinCropValue;
				}
				if (*buffer > m_f64MaxCropValue && (m_cropMethod == OVP_TypeId_CropMethod_Max || m_cropMethod == OVP_TypeId_CropMethod_MinMax))
				{
					*buffer = m_f64MaxCropValue;
				}
			}
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}

		boxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
