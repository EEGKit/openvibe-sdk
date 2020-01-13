#include "ovpCBoxAlgorithmSignalMerger.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Streaming;

bool CBoxAlgorithmSignalMerger::initialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	for (size_t i = 0; i < nInput; ++i) { m_decoders.push_back(new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalMerger>(*this, i)); }

	m_encoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalMerger>(*this, 0);

	return true;
}

bool CBoxAlgorithmSignalMerger::uninitialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	m_encoder->uninitialize();
	delete m_encoder;

	for (size_t i = 0; i < nInput; ++i)
	{
		m_decoders[i]->uninitialize();
		delete m_decoders[i];
	}
	m_decoders.clear();

	return true;
}

bool CBoxAlgorithmSignalMerger::processInput(const size_t index)
{
	IDynamicBoxContext& boxContext = this->getDynamicBoxContext();
	const size_t nInput            = this->getStaticBoxContext().getInputCount();

	if (boxContext.getInputChunkCount(0) == 0) { return true; }

	const uint64_t tStart = boxContext.getInputChunkStartTime(0, 0);
	const uint64_t tEnd   = boxContext.getInputChunkEndTime(0, 0);
	for (size_t i = 1; i < nInput; ++i)
	{
		if (boxContext.getInputChunkCount(i) == 0) { return true; }

		OV_ERROR_UNLESS_KRF(tStart == boxContext.getInputChunkStartTime(i, 0),
							"Invalid start time [" << boxContext.getInputChunkStartTime(i, 0) << "] on input [" << i
							<< "] (expected value must match start time on input 0 [" << tStart << "])",
							OpenViBE::Kernel::ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(tEnd == boxContext.getInputChunkEndTime(i, 0),
							"Invalid end time [" << boxContext.getInputChunkEndTime(i, 0) << "] on input [" << i
							<< "] (expected value must match end time on input 0 [" << tEnd << "])",
							OpenViBE::Kernel::ErrorType::BadInput);
	}

	if (index == nInput - 1)
	{
		for (size_t i = 1; i < nInput; ++i)
		{
			OV_ERROR_UNLESS_KRF(boxContext.getInputChunkCount(0) >= boxContext.getInputChunkCount(i),
								"Invalid input chunk count [" << boxContext.getInputChunkCount(i) << "] on input [" << i
								<< "] (expected value must be <= to chunk count on input 0 [" << boxContext.getInputChunkCount(0) << "])",
								OpenViBE::Kernel::ErrorType::BadInput);
		}
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalMerger::process()
{
	IBoxIO& boxContext  = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	size_t nChunk = boxContext.getInputChunkCount(0);

	for (size_t input = 1; input < nInput; ++input) { if (boxContext.getInputChunkCount(input) < nChunk) { nChunk = boxContext.getInputChunkCount(input); } }

	for (size_t c = 0; c < nChunk; ++c)
	{
		size_t nSamplePerBlock = 0;
		size_t nChannel        = 0;
		size_t nHeader         = 0;
		size_t nBuffer         = 0;
		size_t nEnd            = 0;

		for (size_t i = 0; i < nInput; ++i)
		{
			m_decoders[i]->decode(c);

			const IMatrix* op_pMatrix = m_decoders[i]->getOutputMatrix();
			if (m_decoders[i]->isHeaderReceived())
			{
				nHeader++;
				if (i == 0)
				{
					nSamplePerBlock = op_pMatrix->getDimensionSize(1);
					nChannel        = op_pMatrix->getDimensionSize(0);
				}
				else
				{
					// Check that properties agree
					OV_ERROR_UNLESS_KRF(nSamplePerBlock == op_pMatrix->getDimensionSize(1),
										"Output matrix dimension [" << op_pMatrix->getDimensionSize(1) << "] on input [" << i
										<< "] must match sample count per block [" << nSamplePerBlock << "]",
										OpenViBE::Kernel::ErrorType::BadInput);

					OV_ERROR_UNLESS_KRF(m_decoders[0]->getOutputSamplingRate() == m_decoders[i]->getOutputSamplingRate(),
										"Output sampling rate [" << m_decoders[i]->getOutputSamplingRate() << "] on input [" << i
										<< "] must match the sampling rate on input 0 [" << m_decoders[0]->getOutputSamplingRate() << "]",
										OpenViBE::Kernel::ErrorType::BadInput);

					nChannel += op_pMatrix->getDimensionSize(0);
				}
			}
			if (m_decoders[i]->isBufferReceived()) { nBuffer++; }
			if (m_decoders[i]->isEndReceived()) { nEnd++; }
		}

		OV_ERROR_UNLESS_KRF(!nHeader || nHeader == nInput,
							"Received [" << nHeader << "] headers for [" << nInput << "] declared inputs", OpenViBE::Kernel::ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(!nBuffer || nBuffer == nInput,
							"Received [" << nBuffer << "] buffers for [" << nInput << "] declared inputs", OpenViBE::Kernel::ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(!nEnd || nEnd == nInput,
							"Received [" << nEnd << "] ends for [" << nInput << "] declared inputs", OpenViBE::Kernel::ErrorType::BadInput);

		if (nHeader)
		{
			// We have received headers from all inputs
			IMatrix* ip_pMatrix = m_encoder->getInputMatrix();

			ip_pMatrix->setDimensionCount(2);
			ip_pMatrix->setDimensionSize(0, nChannel);
			ip_pMatrix->setDimensionSize(1, nSamplePerBlock);
			for (size_t i = 0, k = 0; i < nInput; ++i)
			{
				const IMatrix* op_pMatrix = m_decoders[i]->getOutputMatrix();
				for (size_t j = 0; j < op_pMatrix->getDimensionSize(0); j++, k++) { ip_pMatrix->setDimensionLabel(0, k, op_pMatrix->getDimensionLabel(0, j)); }
			}
			const uint64_t sampling           = m_decoders[0]->getOutputSamplingRate();
			m_encoder->getInputSamplingRate() = sampling;

			this->getLogManager() << LogLevel_Debug << "Setting sampling rate to " << sampling << "\n";

			m_encoder->encodeHeader();

			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, c), boxContext.getInputChunkEndTime(0, c));
		}

		if (nBuffer)
		{
			// We have received one buffer from each input
			IMatrix* ip_pMatrix = m_encoder->getInputMatrix();

			nSamplePerBlock = ip_pMatrix->getDimensionSize(1);

			for (size_t i = 0, k = 0; i < nInput; ++i)
			{
				IMatrix* op_pMatrix = m_decoders[i]->getOutputMatrix();
				for (size_t j = 0; j < op_pMatrix->getDimensionSize(0); j++, k++)
				{
					System::Memory::copy(ip_pMatrix->getBuffer() + k * nSamplePerBlock, op_pMatrix->getBuffer() + j * nSamplePerBlock,
										 nSamplePerBlock * sizeof(double));
				}
			}
			m_encoder->encodeBuffer();

			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, c), boxContext.getInputChunkEndTime(0, c));
		}

		if (nEnd)
		{
			// We have received one end from each input
			m_encoder->encodeEnd();
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, c), boxContext.getInputChunkEndTime(0, c));
		}
	}

	return true;
}
