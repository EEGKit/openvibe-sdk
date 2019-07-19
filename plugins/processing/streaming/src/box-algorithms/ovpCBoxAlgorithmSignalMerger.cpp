#include "ovpCBoxAlgorithmSignalMerger.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Streaming;

bool CBoxAlgorithmSignalMerger::initialize()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();

	for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder.push_back(new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalMerger>(*this, i));
	}

	m_pStreamEncoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalMerger>(*this, 0);

	return true;
}

bool CBoxAlgorithmSignalMerger::uninitialize()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();

	m_pStreamEncoder->uninitialize();
	delete m_pStreamEncoder;

	for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]->uninitialize();
		delete m_vStreamDecoder[i];
	}
	m_vStreamDecoder.clear();

	return true;
}

bool CBoxAlgorithmSignalMerger::processInput(uint32_t index)
{
	const IBox& l_rStaticBoxContext          = this->getStaticBoxContext();
	IDynamicBoxContext& l_rDynamicBoxContext = this->getDynamicBoxContext();

	if (l_rDynamicBoxContext.getInputChunkCount(0) == 0) { return true; }

	const uint64_t l_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(0, 0);
	const uint64_t l_ui64EndTime   = l_rDynamicBoxContext.getInputChunkEndTime(0, 0);
	for (uint32_t i = 1; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		if (l_rDynamicBoxContext.getInputChunkCount(i) == 0) { return true; }

		OV_ERROR_UNLESS_KRF(l_ui64StartTime == l_rDynamicBoxContext.getInputChunkStartTime(i, 0),
							"Invalid start time [" << l_rDynamicBoxContext.getInputChunkStartTime(i, 0) << "] on input [" << i
							<< "] (expected value must match start time on input 0 [" << l_ui64StartTime << "])",
							OpenViBE::Kernel::ErrorType::BadInput
		);

		OV_ERROR_UNLESS_KRF(l_ui64EndTime == l_rDynamicBoxContext.getInputChunkEndTime(i, 0),
							"Invalid end time [" << l_rDynamicBoxContext.getInputChunkEndTime(i, 0) << "] on input [" << i
							<< "] (expected value must match end time on input 0 [" << l_ui64EndTime << "])",
							OpenViBE::Kernel::ErrorType::BadInput
		);
	}

	if (index == l_rStaticBoxContext.getInputCount() - 1)
	{
		for (uint32_t i = 1; i < l_rStaticBoxContext.getInputCount(); i++)
		{
			OV_ERROR_UNLESS_KRF(l_rDynamicBoxContext.getInputChunkCount(0) >= l_rDynamicBoxContext.getInputChunkCount(i),
								"Invalid input chunk count [" << l_rDynamicBoxContext.getInputChunkCount(i) << "] on input [" << i
								<< "] (expected value must be <= to chunk count on input 0 [" << l_rDynamicBoxContext.getInputChunkCount(0) << "])",
								OpenViBE::Kernel::ErrorType::BadInput
			);
		}
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalMerger::process()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext    = this->getDynamicBoxContext();

	uint32_t l_ui32NumChunks = l_rDynamicBoxContext.getInputChunkCount(0);

	for (uint32_t input = 1; input < l_rStaticBoxContext.getInputCount(); input++)
	{
		if (l_rDynamicBoxContext.getInputChunkCount(input) < l_ui32NumChunks)
		{
			l_ui32NumChunks = l_rDynamicBoxContext.getInputChunkCount(input);
		}
	}

	for (uint32_t c = 0; c < l_ui32NumChunks; c++)
	{
		uint32_t sampleCountPerBlock = 0;
		uint32_t channelCount        = 0;
		uint32_t headerCount         = 0;
		uint32_t bufferCount         = 0;
		uint32_t endCount            = 0;

		for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
		{
			m_vStreamDecoder[i]->decode(c);

			const IMatrix* op_pMatrix = m_vStreamDecoder[i]->getOutputMatrix();
			if (m_vStreamDecoder[i]->isHeaderReceived())
			{
				headerCount++;
				if (i == 0)
				{
					sampleCountPerBlock = op_pMatrix->getDimensionSize(1);
					channelCount            = op_pMatrix->getDimensionSize(0);
				}
				else
				{
					// Check that properties agree
					OV_ERROR_UNLESS_KRF(sampleCountPerBlock == op_pMatrix->getDimensionSize(1),
										"Output matrix dimension [" << op_pMatrix->getDimensionSize(1) << "] on input [" << i 
										<< "] must match sample count per block [" << sampleCountPerBlock << "]",
										OpenViBE::Kernel::ErrorType::BadInput);

					OV_ERROR_UNLESS_KRF(m_vStreamDecoder[0]->getOutputSamplingRate() == m_vStreamDecoder[i]->getOutputSamplingRate(),
										"Output sampling rate [" << m_vStreamDecoder[i]->getOutputSamplingRate() << "] on input [" << i 
										<< "] must match the sampling rate on input 0 [" << m_vStreamDecoder[0]->getOutputSamplingRate() << "]",
										OpenViBE::Kernel::ErrorType::BadInput);

					channelCount += op_pMatrix->getDimensionSize(0);
				}
			}
			if (m_vStreamDecoder[i]->isBufferReceived()) { bufferCount++; }
			if (m_vStreamDecoder[i]->isEndReceived()) { endCount++; }
		}

		OV_ERROR_UNLESS_KRF(!headerCount || headerCount == l_rStaticBoxContext.getInputCount(), 
							"Received [" << headerCount << "] headers for [" << l_rStaticBoxContext.getInputCount() << "] declared inputs", OpenViBE::Kernel::ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(!bufferCount || bufferCount == l_rStaticBoxContext.getInputCount(),
							"Received [" << bufferCount << "] buffers for [" << l_rStaticBoxContext.getInputCount() << "] declared inputs", OpenViBE::Kernel::ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(!endCount || endCount == l_rStaticBoxContext.getInputCount(),
							"Received [" << endCount << "] ends for [" << l_rStaticBoxContext.getInputCount() << "] declared inputs", OpenViBE::Kernel::ErrorType::BadInput);

		if (headerCount)
		{
			// We have received headers from all inputs
			IMatrix* ip_pMatrix = m_pStreamEncoder->getInputMatrix();

			ip_pMatrix->setDimensionCount(2);
			ip_pMatrix->setDimensionSize(0, channelCount);
			ip_pMatrix->setDimensionSize(1, sampleCountPerBlock);
			for (uint32_t i = 0, k = 0; i < l_rStaticBoxContext.getInputCount(); i++)
			{
				const IMatrix* op_pMatrix = m_vStreamDecoder[i]->getOutputMatrix();
				for (uint32_t j = 0; j < op_pMatrix->getDimensionSize(0); j++, k++)
				{
					ip_pMatrix->setDimensionLabel(0, k, op_pMatrix->getDimensionLabel(0, j));
				}
			}
			const uint64_t l_ui64SamplingRate          = m_vStreamDecoder[0]->getOutputSamplingRate();
			m_pStreamEncoder->getInputSamplingRate() = l_ui64SamplingRate;

			this->getLogManager() << LogLevel_Debug << "Setting sampling rate to " << l_ui64SamplingRate << "\n";

			m_pStreamEncoder->encodeHeader();

			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, c), l_rDynamicBoxContext.getInputChunkEndTime(0, c));
		}

		if (bufferCount)
		{
			// We have received one buffer from each input
			IMatrix* ip_pMatrix = m_pStreamEncoder->getInputMatrix();

			sampleCountPerBlock = ip_pMatrix->getDimensionSize(1);

			for (uint32_t i = 0, k = 0; i < l_rStaticBoxContext.getInputCount(); i++)
			{
				IMatrix* op_pMatrix = m_vStreamDecoder[i]->getOutputMatrix();
				for (uint32_t j = 0; j < op_pMatrix->getDimensionSize(0); j++, k++)
				{
					System::Memory::copy(ip_pMatrix->getBuffer() + k * sampleCountPerBlock, op_pMatrix->getBuffer() + j * sampleCountPerBlock, sampleCountPerBlock * sizeof(double));
				}
			}
			m_pStreamEncoder->encodeBuffer();

			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, c), l_rDynamicBoxContext.getInputChunkEndTime(0, c));
		}

		if (endCount)
		{
			// We have received one end from each input
			m_pStreamEncoder->encodeEnd();
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, c), l_rDynamicBoxContext.getInputChunkEndTime(0, c));
		}
	}

	return true;
}
