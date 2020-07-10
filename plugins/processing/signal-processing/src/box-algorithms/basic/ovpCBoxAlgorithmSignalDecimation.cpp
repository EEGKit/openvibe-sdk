#include "ovpCBoxAlgorithmSignalDecimation.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::Plugins::*/SignalProcessing;

bool CBoxAlgorithmSignalDecimation::initialize()
{
	m_decoder = nullptr;
	m_encoder = nullptr;

	m_decimationFactor = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	OV_ERROR_UNLESS_KRF(m_decimationFactor > 1, "Invalid decimation factor [" << m_decimationFactor << "] (expected value > 1)",
						Kernel::ErrorType::BadSetting);

	m_decoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalDecoder));
	m_decoder->initialize();

	ip_buffer.initialize(m_decoder->getInputParameter(OVP_GD_Algorithm_SignalDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_decoder->getOutputParameter(OVP_GD_Algorithm_SignalDecoder_OutputParameterId_Matrix));
	op_sampling.initialize(m_decoder->getOutputParameter(OVP_GD_Algorithm_SignalDecoder_OutputParameterId_Sampling));

	m_encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalEncoder));
	m_encoder->initialize();

	ip_sampling.initialize(m_encoder->getInputParameter(OVP_GD_Algorithm_SignalEncoder_InputParameterId_Sampling));
	ip_matrix.initialize(m_encoder->getInputParameter(OVP_GD_Algorithm_SignalEncoder_InputParameterId_Matrix));
	op_buffer.initialize(m_encoder->getOutputParameter(OVP_GD_Algorithm_SignalEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_nChannel         = 0;
	m_iSampleIdx       = 0;
	m_iNSamplePerBlock = 0;
	m_oSampling        = 0;
	m_oSampleIdx       = 0;
	m_oNSamplePerBlock = 0;

	m_nTotalSample  = 0;
	m_startTimeBase = 0;
	m_lastStartTime = 0;
	m_lastEndTime   = 0;

	return true;
}

bool CBoxAlgorithmSignalDecimation::uninitialize()
{
	op_buffer.uninitialize();
	ip_matrix.uninitialize();
	ip_sampling.uninitialize();

	if (m_encoder)
	{
		m_encoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_encoder);
		m_encoder = nullptr;
	}

	op_sampling.uninitialize();
	op_pMatrix.uninitialize();
	ip_buffer.uninitialize();

	if (m_decoder)
	{
		m_decoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_decoder);
		m_decoder = nullptr;
	}

	return true;
}

bool CBoxAlgorithmSignalDecimation::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalDecimation::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		ip_buffer = boxContext.getInputChunk(0, i);
		op_buffer = boxContext.getOutputChunk(0);

		const CTime tStart = boxContext.getInputChunkStartTime(0, i);
		const CTime tEnd   = boxContext.getInputChunkEndTime(0, i);

		if (tStart != m_lastEndTime)
		{
			m_startTimeBase = tStart;
			m_iSampleIdx    = 0;
			m_oSampleIdx    = 0;
			m_nTotalSample  = 0;
		}

		m_lastStartTime = tStart;
		m_lastEndTime   = tEnd;

		m_decoder->process();
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_iSampleIdx       = 0;
			m_iNSamplePerBlock = op_pMatrix->getDimensionSize(1);
			m_iSampling        = op_sampling;

			OV_ERROR_UNLESS_KRF(m_iSampling%m_decimationFactor == 0,
								"Failed to decimate: input sampling frequency [" << m_iSampling << "] not multiple of decimation factor [" <<
								m_decimationFactor << "]", Kernel::ErrorType::BadSetting);

			m_oSampleIdx       = 0;
			m_oNSamplePerBlock = size_t(m_iNSamplePerBlock / m_decimationFactor);
			m_oNSamplePerBlock = (m_oNSamplePerBlock ? m_oNSamplePerBlock : 1);
			m_oSampling        = op_sampling / m_decimationFactor;

			OV_ERROR_UNLESS_KRF(m_oSampling != 0, "Failed to decimate: output sampling frequency is 0", Kernel::ErrorType::BadOutput);

			m_nChannel     = op_pMatrix->getDimensionSize(0);
			m_nTotalSample = 0;

			ip_matrix->copyDescription(*op_pMatrix);
			ip_matrix->setDimensionSize(1, m_oNSamplePerBlock);
			ip_sampling = m_oSampling;
			m_encoder->process(OVP_GD_Algorithm_SignalEncoder_InputTriggerId_EncodeHeader);
			ip_matrix->reset();

			boxContext.markOutputAsReadyToSend(0, tStart, tStart); // $$$ supposes we have one node per chunk
		}
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalDecoder_OutputTriggerId_ReceivedBuffer))
		{
			double* iBuffer = op_pMatrix->getBuffer();
			double* oBuffer = ip_matrix->getBuffer() + m_oSampleIdx;

			for (size_t j = 0; j < m_iNSamplePerBlock; ++j)
			{
				double* iBufferTmp = iBuffer;
				double* oBufferTmp = oBuffer;
				for (size_t k = 0; k < m_nChannel; ++k)
				{
					*oBufferTmp += *iBufferTmp;
					oBufferTmp += m_oNSamplePerBlock;
					iBufferTmp += m_iNSamplePerBlock;
				}

				m_iSampleIdx++;
				if (m_iSampleIdx == m_decimationFactor)
				{
					m_iSampleIdx = 0;
					oBufferTmp   = oBuffer;
					for (size_t k = 0; k < m_nChannel; ++k)
					{
						*oBufferTmp /= m_decimationFactor;
						oBufferTmp += m_oNSamplePerBlock;
					}

					oBuffer++;
					m_oSampleIdx++;
					if (m_oSampleIdx == m_oNSamplePerBlock)
					{
						oBuffer      = ip_matrix->getBuffer();
						m_oSampleIdx = 0;
						m_encoder->process(OVP_GD_Algorithm_SignalEncoder_InputTriggerId_EncodeBuffer);
						const CTime tStartSample = m_startTimeBase + CTime(m_oSampling, m_nTotalSample);
						const CTime tEndSample   = m_startTimeBase + CTime(m_oSampling, m_nTotalSample + m_oNSamplePerBlock);
						boxContext.markOutputAsReadyToSend(0, tStartSample, tEndSample);
						m_nTotalSample += m_oNSamplePerBlock;

						ip_matrix->reset();
					}
				}

				iBuffer++;
			}
		}
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_encoder->process(OVP_GD_Algorithm_SignalEncoder_InputTriggerId_EncodeEnd);
			boxContext.markOutputAsReadyToSend(0, tStart, tStart); // $$$ supposes we have one node per chunk
		}

		boxContext.markInputAsDeprecated(0, i);
	}
	return true;
}
