#include "ovpCBoxAlgorithmCrop.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;

bool CBoxAlgorithmCrop::initialize()
{
	CIdentifier inputTypeID;
	getStaticBoxContext().getInputType(0, inputTypeID);

	if (inputTypeID == OV_TypeId_StreamedMatrix)
	{
		m_decoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixDecoder));
		m_encoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixEncoder));
	}
	else if (inputTypeID == OV_TypeId_FeatureVector)
	{
		m_decoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorDecoder));
		m_encoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorEncoder));
	}
	else if (inputTypeID == OV_TypeId_Signal)
	{
		m_decoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalDecoder));
		m_encoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalEncoder));
	}
	else if (inputTypeID == OV_TypeId_Spectrum)
	{
		m_decoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumDecoder));
		m_encoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumEncoder));
	}
	else { return false; }
	m_decoder->initialize();
	m_encoder->initialize();

	if (inputTypeID == OV_TypeId_StreamedMatrix) { }
	else if (inputTypeID == OV_TypeId_FeatureVector) { }
	else if (inputTypeID == OV_TypeId_Signal)
	{
		m_encoder->getInputParameter(OVP_GD_Algorithm_SignalEncoder_InputParameterId_Sampling)->setReferenceTarget(
			m_decoder->getOutputParameter(OVP_GD_Algorithm_SignalDecoder_OutputParameterId_Sampling));
	}
	else if (inputTypeID == OV_TypeId_Spectrum)
	{
		m_encoder->getInputParameter(OVP_GD_Algorithm_SpectrumEncoder_InputParameterId_FrequencyAbscissa)->setReferenceTarget(
			m_decoder->getOutputParameter(OVP_GD_Algorithm_SpectrumDecoder_OutputParameterId_FrequencyAbscissa));
		m_encoder->getInputParameter(OVP_GD_Algorithm_SpectrumEncoder_InputParameterId_Sampling)->setReferenceTarget(
			m_decoder->getOutputParameter(OVP_GD_Algorithm_SpectrumDecoder_OutputParameterId_Sampling));
	}

	m_matrix = new CMatrix();
	TParameterHandler<CMatrix*>(m_encoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix)).setReferenceTarget(m_matrix);
	TParameterHandler<CMatrix*>(m_decoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputParameterId_Matrix)).setReferenceTarget(m_matrix);

	m_cropMethod   = ECropMethod(uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0)));
	m_minCropValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_maxCropValue = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	OV_ERROR_UNLESS_KRF(m_minCropValue < m_maxCropValue,
						"Invalid crop values: minimum crop value [" << m_minCropValue << "] should be lower than the maximum crop value ["
						<< m_maxCropValue << "]", ErrorType::BadSetting);

	return true;
}

bool CBoxAlgorithmCrop::uninitialize()
{
	delete m_matrix;

	m_encoder->uninitialize();
	m_decoder->uninitialize();

	getAlgorithmManager().releaseAlgorithm(*m_encoder);
	getAlgorithmManager().releaseAlgorithm(*m_decoder);

	return true;
}

bool CBoxAlgorithmCrop::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmCrop::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		TParameterHandler<const IMemoryBuffer*> iHandle(
			m_decoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<IMemoryBuffer*> oHandle(m_encoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_OutputParameterId_EncodedMemoryBuffer));
		iHandle = boxContext.getInputChunk(0, i);
		oHandle = boxContext.getOutputChunk(0);

		m_decoder->process();
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeHeader);
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedBuffer))
		{
			double* buffer = m_matrix->getBuffer();
			for (size_t j = 0; j < m_matrix->getBufferElementCount(); j++, buffer++)
			{
				if (*buffer < m_minCropValue && (m_cropMethod == ECropMethod::Min || m_cropMethod == ECropMethod::MinMax)) { *buffer = m_minCropValue; }
				if (*buffer > m_maxCropValue && (m_cropMethod == ECropMethod::Max || m_cropMethod == ECropMethod::MinMax)) { *buffer = m_maxCropValue; }
			}
			m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeBuffer);
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeEnd);
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}

		boxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
