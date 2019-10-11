#include "ovpCBoxAlgorithmCommonAverageReference.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmCommonAverageReference::initialize()
{
	// CString   l_sSettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	// uint64_t l_ui64SettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	// double l_f64SettingValue=FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	// ...

	m_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
	m_pStreamDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
	op_ui64SamplingRate.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	m_pStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
	ip_ui64SamplingRate.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_SignalStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	op_pMatrix = &m_oMatrix;
	ip_pMatrix = &m_oMatrix;
	ip_ui64SamplingRate.setReferenceTarget(op_ui64SamplingRate);

	return true;
}

bool CBoxAlgorithmCommonAverageReference::uninitialize()
{
	op_pMemoryBuffer.uninitialize();
	ip_ui64SamplingRate.uninitialize();
	ip_pMatrix.uninitialize();
	m_pStreamEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);

	op_ui64SamplingRate.uninitialize();
	op_pMatrix.uninitialize();
	ip_pMemoryBuffer.uninitialize();
	m_pStreamDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);

	return true;
}

bool CBoxAlgorithmCommonAverageReference::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmCommonAverageReference::process()
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		ip_pMemoryBuffer = boxContext.getInputChunk(0, i);
		op_pMemoryBuffer = boxContext.getOutputChunk(0);

		m_pStreamDecoder->process();
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader);
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			const uint32_t nChannel = m_oMatrix.getDimensionSize(0),
						   nSample  = m_oMatrix.getDimensionSize(1);
			for (uint32_t j = 0; j < nSample; ++j)
			{
				double* buffer = m_oMatrix.getBuffer() + j;
				double sum     = 0;
				for (uint32_t c = nChannel; c != 0; c--)
				{
					sum += *buffer;
					buffer += nSample;
				}
				const double mean = sum / nChannel;
				for (uint32_t c = nChannel; c != 0; c--)
				{
					buffer -= nSample;
					*buffer -= mean;
				}
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer);
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd);
		}

		boxContext.markInputAsDeprecated(0, i);
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
