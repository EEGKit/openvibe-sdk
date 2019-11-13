#include "ovpCBoxAlgorithmEpochAverage.h"

#include "../../algorithms/basic/ovpCAlgorithmMatrixAverage.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmEpochAverage::initialize()
{
	CIdentifier inputTypeId;
	getStaticBoxContext().getInputType(0, inputTypeId);
	if (inputTypeId == OV_TypeId_StreamedMatrix || inputTypeId == OV_TypeId_TimeFrequency)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixEncoder));
	}
	else if (inputTypeId == OV_TypeId_FeatureVector)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorEncoder));
	}
	else if (inputTypeId == OV_TypeId_Signal)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalEncoder));
	}
	else if (inputTypeId == OV_TypeId_Spectrum)
	{
		m_pStreamDecoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumDecoder));
		m_pStreamEncoder = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumEncoder));
	}
	else { return false; }
	m_pStreamDecoder->initialize();
	m_pStreamEncoder->initialize();

	m_matrixAverage = &getAlgorithmManager().getAlgorithm(getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_MatrixAverage));
	m_matrixAverage->initialize();

	if (inputTypeId == OV_TypeId_StreamedMatrix) { }
	else if (inputTypeId == OV_TypeId_FeatureVector) { }
	else if (inputTypeId == OV_TypeId_Signal)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalEncoder_InputParameterId_Sampling)->setReferenceTarget(
			m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalDecoder_OutputParameterId_Sampling));
	}
	else if (inputTypeId == OV_TypeId_Spectrum)
	{
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumEncoder_InputParameterId_FrequencyAbscissa)->setReferenceTarget(
			m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumDecoder_OutputParameterId_FrequencyAbscissa));
		m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SpectrumEncoder_InputParameterId_Sampling)->setReferenceTarget(
			m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumDecoder_OutputParameterId_Sampling));
	}

	ip_ui64AveragingMethod.initialize(m_matrixAverage->getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod));
	ip_ui64MatrixCount.initialize(m_matrixAverage->getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount));

	ip_ui64AveragingMethod = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));
	ip_ui64MatrixCount     = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));

	m_matrixAverage->getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix)->setReferenceTarget(
		m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputParameterId_Matrix));
	m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix)->setReferenceTarget(
		m_matrixAverage->getOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix));

	OV_ERROR_UNLESS_KRF(ip_ui64MatrixCount > 0, "Invalid number of epochs (expected value > 0)", OpenViBE::Kernel::ErrorType::BadSetting);

	return true;
}

bool CBoxAlgorithmEpochAverage::uninitialize()
{
	CIdentifier inputTypeID;
	getStaticBoxContext().getInputType(0, inputTypeID);
	if (inputTypeID == OV_TypeId_StreamedMatrix || inputTypeID == OV_TypeId_FeatureVector || inputTypeID == OV_TypeId_Signal ||
		inputTypeID == OV_TypeId_Spectrum)
	{
		ip_ui64AveragingMethod.uninitialize();
		ip_ui64MatrixCount.uninitialize();

		m_matrixAverage->uninitialize();
		m_pStreamEncoder->uninitialize();
		m_pStreamDecoder->uninitialize();

		getAlgorithmManager().releaseAlgorithm(*m_matrixAverage);
		getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
		getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
	}

	return true;
}

bool CBoxAlgorithmEpochAverage::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmEpochAverage::process()
{
	IBoxIO& boxContext  = getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	for (uint32_t i = 0; i < nInput; ++i)
	{
		for (uint32_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
		{
			TParameterHandler<const IMemoryBuffer*> l_oInputMemoryBufferHandle(
				m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_InputParameterId_MemoryBufferToDecode));
			TParameterHandler<IMemoryBuffer*> l_oOutputMemoryBufferHandle(
				m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_OutputParameterId_EncodedMemoryBuffer));
			l_oInputMemoryBufferHandle  = boxContext.getInputChunk(i, j);
			l_oOutputMemoryBufferHandle = boxContext.getOutputChunk(i);

			m_pStreamDecoder->process();
			if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedHeader))
			{
				m_matrixAverage->process(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset);
				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeHeader);
				boxContext.markOutputAsReadyToSend(i, boxContext.getInputChunkStartTime(i, j), boxContext.getInputChunkEndTime(i, j));
			}
			if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedBuffer))
			{
				m_matrixAverage->process(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix);
				if (m_matrixAverage->isOutputTriggerActive(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed))
				{
					m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeBuffer);
					boxContext.markOutputAsReadyToSend(i, boxContext.getInputChunkStartTime(i, j), boxContext.getInputChunkEndTime(i, j));
				}
			}
			if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedEnd))
			{
				m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeEnd);
				boxContext.markOutputAsReadyToSend(i, boxContext.getInputChunkStartTime(i, j), boxContext.getInputChunkEndTime(i, j));
			}

			boxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
