#include "ovpCBoxAlgorithmSignalDecimation.h"

#include <system/ovCMemory.h>

#include <openvibe/ovITimeArithmetics.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmSignalDecimation::initialize()
{
	m_pStreamDecoder = nullptr;
	m_pStreamEncoder = nullptr;

	m_i64DecimationFactor = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	OV_ERROR_UNLESS_KRF(m_i64DecimationFactor > 1, "Invalid decimation factor [" << m_i64DecimationFactor << "] (expected value > 1)",
						OpenViBE::Kernel::ErrorType::BadSetting);

	m_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_ui64SamplingRate.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	m_pStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_ui64SamplingRate.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	m_nChannel                  = 0;
	m_ui32InputSampleIndex              = 0;
	m_ui32InputSampleCountPerSentBlock  = 0;
	m_ui64OutputSamplingFrequency       = 0;
	m_ui32OutputSampleIndex             = 0;
	m_ui32OutputSampleCountPerSentBlock = 0;

	m_nTotalSample = 0;
	m_ui64StartTimeBase    = 0;
	m_ui64LastStartTime    = 0;
	m_ui64LastEndTime      = 0;

	return true;
}

bool CBoxAlgorithmSignalDecimation::uninitialize()
{
	op_pMemoryBuffer.uninitialize();
	ip_pMatrix.uninitialize();
	ip_ui64SamplingRate.uninitialize();

	if (m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
		m_pStreamEncoder = nullptr;
	}

	op_ui64SamplingRate.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();

	if (m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
		m_pStreamDecoder = nullptr;
	}

	return true;
}

bool CBoxAlgorithmSignalDecimation::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalDecimation::process()
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		ip_pMemoryBuffer = boxContext.getInputChunk(0, i);
		op_pMemoryBuffer = boxContext.getOutputChunk(0);

		uint64_t tStart = boxContext.getInputChunkStartTime(0, i);
		uint64_t tEnd   = boxContext.getInputChunkEndTime(0, i);

		if (tStart != m_ui64LastEndTime)
		{
			m_ui64StartTimeBase     = tStart;
			m_ui32InputSampleIndex  = 0;
			m_ui32OutputSampleIndex = 0;
			m_nTotalSample  = 0;
		}

		m_ui64LastStartTime = tStart;
		m_ui64LastEndTime   = tEnd;

		m_pStreamDecoder->process();
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_ui32InputSampleIndex             = 0;
			m_ui32InputSampleCountPerSentBlock = op_pMatrix->getDimensionSize(1);
			m_ui64InputSamplingFrequency       = op_ui64SamplingRate;

			OV_ERROR_UNLESS_KRF(m_ui64InputSamplingFrequency%m_i64DecimationFactor == 0,
								"Failed to decimate: input sampling frequency [" << m_ui64InputSamplingFrequency << "] not multiple of decimation factor [" <<
								m_i64DecimationFactor << "]",
								OpenViBE::Kernel::ErrorType::BadSetting);

			m_ui32OutputSampleIndex             = 0;
			m_ui32OutputSampleCountPerSentBlock = uint32_t(m_ui32InputSampleCountPerSentBlock / m_i64DecimationFactor);
			m_ui32OutputSampleCountPerSentBlock = (m_ui32OutputSampleCountPerSentBlock ? m_ui32OutputSampleCountPerSentBlock : 1);
			m_ui64OutputSamplingFrequency       = op_ui64SamplingRate / m_i64DecimationFactor;

			OV_ERROR_UNLESS_KRF(m_ui64OutputSamplingFrequency != 0, "Failed to decimate: output sampling frequency is 0",
								OpenViBE::Kernel::ErrorType::BadOutput);

			m_nChannel     = op_pMatrix->getDimensionSize(0);
			m_nTotalSample = 0;

			OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_pMatrix, *op_pMatrix);
			ip_pMatrix->setDimensionSize(1, m_ui32OutputSampleCountPerSentBlock);
			ip_ui64SamplingRate = m_ui64OutputSamplingFrequency;
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
			OpenViBEToolkit::Tools::Matrix::clearContent(*ip_pMatrix);

			boxContext.markOutputAsReadyToSend(0, tStart, tStart); // $$$ supposes we have one node per chunk
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			double* iBuffer = op_pMatrix->getBuffer();
			double* oBuffer = ip_pMatrix->getBuffer() + m_ui32OutputSampleIndex;

			for (uint32_t j = 0; j < m_ui32InputSampleCountPerSentBlock; j++)
			{
				double* iBufferTmp = iBuffer;
				double* oBufferTmp = oBuffer;
				for (uint32_t k = 0; k < m_nChannel; k++)
				{
					*oBufferTmp += *iBufferTmp;
					oBufferTmp += m_ui32OutputSampleCountPerSentBlock;
					iBufferTmp += m_ui32InputSampleCountPerSentBlock;
				}

				m_ui32InputSampleIndex++;
				if (m_ui32InputSampleIndex == m_i64DecimationFactor)
				{
					m_ui32InputSampleIndex = 0;
					oBufferTmp             = oBuffer;
					for (uint32_t k = 0; k < m_nChannel; k++)
					{
						*oBufferTmp /= m_i64DecimationFactor;
						oBufferTmp += m_ui32OutputSampleCountPerSentBlock;
					}

					oBuffer++;
					m_ui32OutputSampleIndex++;
					if (m_ui32OutputSampleIndex == m_ui32OutputSampleCountPerSentBlock)
					{
						oBuffer                 = ip_pMatrix->getBuffer();
						m_ui32OutputSampleIndex = 0;
						m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
						const uint64_t tStartSample = m_ui64StartTimeBase + ITimeArithmetics::sampleCountToTime(
														  m_ui64OutputSamplingFrequency, m_nTotalSample);
						const uint64_t tEndSample = m_ui64StartTimeBase + ITimeArithmetics::sampleCountToTime(
														m_ui64OutputSamplingFrequency, m_nTotalSample + m_ui32OutputSampleCountPerSentBlock);
						boxContext.markOutputAsReadyToSend(0, tStartSample, tEndSample);
						m_nTotalSample += m_ui32OutputSampleCountPerSentBlock;

						OpenViBEToolkit::Tools::Matrix::clearContent(*ip_pMatrix);
					}
				}

				iBuffer++;
			}
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
			boxContext.markOutputAsReadyToSend(0, tStart, tStart); // $$$ supposes we have one node per chunk
		}

		boxContext.markInputAsDeprecated(0, i);
	}
	return true;
}
