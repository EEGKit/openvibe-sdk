#include "ovpCBoxAlgorithmSpectrumAverage.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmSpectrumAverage::initialize()
{
	m_bZeroCare = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	m_decoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumDecoder));
	m_decoder->initialize();

	m_encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixEncoder));
	m_encoder->initialize();

	ip_buffer.initialize(m_decoder->getInputParameter(OVP_GD_Algorithm_SpectrumDecoder_InputParameterId_MemoryBufferToDecode));
	op_matrix.initialize(m_decoder->getOutputParameter(OVP_GD_Algorithm_SpectrumDecoder_OutputParameterId_Matrix));
	ip_matrix.initialize(m_encoder->getInputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	op_buffer.initialize(m_encoder->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_OutputParameterId_EncodedMemoryBuffer));

	return true;
}

bool CBoxAlgorithmSpectrumAverage::uninitialize()
{
	ip_matrix.uninitialize();
	op_matrix.uninitialize();

	if (m_encoder)
	{
		m_encoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_encoder);
		m_encoder = nullptr;
	}

	if (m_decoder)
	{
		m_decoder->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_decoder);
		m_decoder = nullptr;
	}

	return true;
}

bool CBoxAlgorithmSpectrumAverage::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSpectrumAverage::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		ip_buffer = boxContext.getInputChunk(0, i);
		op_buffer = boxContext.getOutputChunk(0);

		m_decoder->process();
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumDecoder_OutputTriggerId_ReceivedHeader))
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(*ip_matrix, *op_matrix);
			ip_matrix->setDimensionSize(1, 1);

			m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeHeader);
		}
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumDecoder_OutputTriggerId_ReceivedBuffer))
		{
			double* iMatrix       = ip_matrix->getBuffer();
			double* oMatrix       = op_matrix->getBuffer();
			const size_t nChannel = op_matrix->getDimensionSize(0);
			const size_t nBand    = op_matrix->getDimensionSize(1);
			for (size_t j = 0; j < nChannel; ++j)
			{
				double mean = 0;
				size_t n    = 0;
				for (size_t k = 0; k < nBand; ++k)
				{
					mean += *oMatrix;
					n += (m_bZeroCare || *oMatrix != 0) ? 1 : 0;
					oMatrix++;
				}
				*iMatrix = (n == 0 ? 0 : mean / n);
				iMatrix++;
			}
			m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeBuffer);
		}
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumDecoder_OutputTriggerId_ReceivedEnd))
		{
			m_encoder->process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeEnd);
		}

		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markInputAsDeprecated(0, i);
	}

	return true;
}
