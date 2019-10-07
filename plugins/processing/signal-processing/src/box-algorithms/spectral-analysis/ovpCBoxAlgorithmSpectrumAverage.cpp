#include "ovpCBoxAlgorithmSpectrumAverage.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmSpectrumAverage::initialize()
{
	m_bZeroCare = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	m_pStreamDecoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
	m_pStreamDecoder->initialize();

	m_pStreamEncoder = &this->getAlgorithmManager().getAlgorithm(
		this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
	m_pStreamEncoder->initialize();

	ip_pMemoryBuffer.initialize(m_pStreamDecoder->getInputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pMatrix.initialize(m_pStreamDecoder->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix));
	ip_pMatrix.initialize(m_pStreamEncoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	op_pMemoryBuffer.initialize(m_pStreamEncoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

	return true;
}

bool CBoxAlgorithmSpectrumAverage::uninitialize()
{
	ip_pMatrix.uninitialize();
	op_pMatrix.uninitialize();

	if (m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamEncoder);
		m_pStreamEncoder = nullptr;
	}

	if (m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pStreamDecoder);
		m_pStreamDecoder = nullptr;
	}

	return true;
}

bool CBoxAlgorithmSpectrumAverage::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSpectrumAverage::process()
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		ip_pMemoryBuffer = boxContext.getInputChunk(0, i);
		op_pMemoryBuffer = boxContext.getOutputChunk(0);

		m_pStreamDecoder->process();
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader))
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_pMatrix, *op_pMatrix);
			ip_pMatrix->setDimensionSize(1, 1);

			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			double* iMatrix      = ip_pMatrix->getBuffer();
			double* oMatrix     = op_pMatrix->getBuffer();
			const uint32_t nChannel = op_pMatrix->getDimensionSize(0);
			const uint32_t nBand    = op_pMatrix->getDimensionSize(1);
			for (uint32_t j = 0; j < nChannel; ++j)
			{
				double mean     = 0;
				uint32_t n = 0;
				for (uint32_t k = 0; k < nBand; ++k)
				{
					mean += *oMatrix;
					n += (m_bZeroCare || *oMatrix != 0) ? 1 : 0;
					oMatrix++;
				}
				*iMatrix = (n == 0 ? 0 : mean / n);
				iMatrix++;
			}

			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
		}
		if (m_pStreamDecoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_pStreamEncoder->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
		}

		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
