#include "ovpCBoxAlgorithmSignalMerger.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Streaming;

bool CBoxAlgorithmSignalMerger::initialize(void)
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();

	for (uint32 i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder.push_back(new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalMerger>(*this, i));
	}

	m_pStreamEncoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalMerger>(*this, 0);

	return true;
}

bool CBoxAlgorithmSignalMerger::uninitialize(void)
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();

	m_pStreamEncoder->uninitialize();
	delete m_pStreamEncoder;

	for (uint32 i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i]->uninitialize();
		delete m_vStreamDecoder[i];
	}
	m_vStreamDecoder.clear();

	return true;
}

bool CBoxAlgorithmSignalMerger::processInput(uint32 ui32InputIndex)
{
	const IBox& l_rStaticBoxContext          = this->getStaticBoxContext();
	IDynamicBoxContext& l_rDynamicBoxContext = this->getDynamicBoxContext();

	if (l_rDynamicBoxContext.getInputChunkCount(0) == 0) { return true; }

	const uint64 l_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(0, 0);
	const uint64 l_ui64EndTime   = l_rDynamicBoxContext.getInputChunkEndTime(0, 0);
	for (uint32 i = 1; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		if (l_rDynamicBoxContext.getInputChunkCount(i) == 0) { return true; }

		OV_ERROR_UNLESS_KRF(
			l_ui64StartTime == l_rDynamicBoxContext.getInputChunkStartTime(i, 0),
			"Invalid start time [" << l_rDynamicBoxContext.getInputChunkStartTime(i, 0) << "] on input [" << i
			<< "] (expected value must match start time on input 0 [" << l_ui64StartTime << "])",
			OpenViBE::Kernel::ErrorType::BadInput
		);

		OV_ERROR_UNLESS_KRF(
			l_ui64EndTime == l_rDynamicBoxContext.getInputChunkEndTime(i, 0),
			"Invalid end time [" << l_rDynamicBoxContext.getInputChunkEndTime(i, 0) << "] on input [" << i
			<< "] (expected value must match end time on input 0 [" << l_ui64EndTime << "])",
			OpenViBE::Kernel::ErrorType::BadInput
		);
	}

	if (ui32InputIndex == l_rStaticBoxContext.getInputCount() - 1)
	{
		for (uint32 i = 1; i < l_rStaticBoxContext.getInputCount(); i++)
		{
			OV_ERROR_UNLESS_KRF(
				l_rDynamicBoxContext.getInputChunkCount(0) >= l_rDynamicBoxContext.getInputChunkCount(i),
				"Invalid input chunk count [" << l_rDynamicBoxContext.getInputChunkCount(i) << "] on input [" << i
				<< "] (expected value must be <= to chunk count on input 0 [" << l_rDynamicBoxContext.getInputChunkCount(0) << "])",
				OpenViBE::Kernel::ErrorType::BadInput
			);
		}
	}

	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalMerger::process(void)
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext    = this->getDynamicBoxContext();

	uint32 l_ui32NumChunks = l_rDynamicBoxContext.getInputChunkCount(0);

	for (uint32 input = 1; input < l_rStaticBoxContext.getInputCount(); input++)
	{
		if (l_rDynamicBoxContext.getInputChunkCount(input) < l_ui32NumChunks)
		{
			l_ui32NumChunks = l_rDynamicBoxContext.getInputChunkCount(input);
		}
	}

	for (uint32 c = 0; c < l_ui32NumChunks; c++)
	{
		uint32 l_ui32SampleCountPerSentBlock = 0;
		uint32 l_ui32ChannelCount            = 0;
		uint32 l_ui32HeaderCount             = 0;
		uint32 l_ui32BufferCount             = 0;
		uint32 l_ui32EndCount                = 0;

		for (uint32 i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
		{
			m_vStreamDecoder[i]->decode(c);

			const IMatrix* op_pMatrix = m_vStreamDecoder[i]->getOutputMatrix();
			if (m_vStreamDecoder[i]->isHeaderReceived())
			{
				l_ui32HeaderCount++;
				if (i == 0)
				{
					l_ui32SampleCountPerSentBlock = op_pMatrix->getDimensionSize(1);
					l_ui32ChannelCount            = op_pMatrix->getDimensionSize(0);
				}
				else
				{
					// Check that properties agree
					OV_ERROR_UNLESS_KRF(
						l_ui32SampleCountPerSentBlock == op_pMatrix->getDimensionSize(1),
						"Output matrix dimension [" << op_pMatrix->getDimensionSize(1) << "] on input [" << i
						<< "] must match sample count per block [" << l_ui32SampleCountPerSentBlock << "]",
						OpenViBE::Kernel::ErrorType::BadInput
					);

					OV_ERROR_UNLESS_KRF(
						m_vStreamDecoder[0]->getOutputSamplingRate() == m_vStreamDecoder[i]->getOutputSamplingRate(),
						"Output sampling rate [" << m_vStreamDecoder[i]->getOutputSamplingRate() << "] on input [" << i
						<< "] must match the sampling rate on input 0 [" << m_vStreamDecoder[0]->getOutputSamplingRate() << "]",
						OpenViBE::Kernel::ErrorType::BadInput
					);

					l_ui32ChannelCount += op_pMatrix->getDimensionSize(0);
				}
			}
			if (m_vStreamDecoder[i]->isBufferReceived())
			{
				l_ui32BufferCount++;
			}
			if (m_vStreamDecoder[i]->isEndReceived())
			{
				l_ui32EndCount++;
			}
		}

		OV_ERROR_UNLESS_KRF(
			!l_ui32HeaderCount || l_ui32HeaderCount == l_rStaticBoxContext.getInputCount(),
			"Received [" << l_ui32HeaderCount << "] headers for [" << l_rStaticBoxContext.getInputCount() << "] declared inputs",
			OpenViBE::Kernel::ErrorType::BadInput
		);

		OV_ERROR_UNLESS_KRF(
			!l_ui32BufferCount || l_ui32BufferCount == l_rStaticBoxContext.getInputCount(),
			"Received [" << l_ui32BufferCount << "] buffers for [" << l_rStaticBoxContext.getInputCount() << "] declared inputs",
			OpenViBE::Kernel::ErrorType::BadInput
		);

		OV_ERROR_UNLESS_KRF(
			!l_ui32EndCount || l_ui32EndCount == l_rStaticBoxContext.getInputCount(),
			"Received [" << l_ui32EndCount << "] ends for [" << l_rStaticBoxContext.getInputCount() << "] declared inputs",
			OpenViBE::Kernel::ErrorType::BadInput
		);

		if (l_ui32HeaderCount)
		{
			// We have received headers from all inputs
			IMatrix* ip_pMatrix = m_pStreamEncoder->getInputMatrix();

			ip_pMatrix->setDimensionCount(2);
			ip_pMatrix->setDimensionSize(0, l_ui32ChannelCount);
			ip_pMatrix->setDimensionSize(1, l_ui32SampleCountPerSentBlock);
			for (uint32 i = 0, k = 0; i < l_rStaticBoxContext.getInputCount(); i++)
			{
				const IMatrix* op_pMatrix = m_vStreamDecoder[i]->getOutputMatrix();
				for (uint32 j = 0; j < op_pMatrix->getDimensionSize(0); j++, k++)
				{
					ip_pMatrix->setDimensionLabel(0, k, op_pMatrix->getDimensionLabel(0, j));
				}
			}
			const uint64 l_ui64SamplingRate          = m_vStreamDecoder[0]->getOutputSamplingRate();
			m_pStreamEncoder->getInputSamplingRate() = l_ui64SamplingRate;

			this->getLogManager() << LogLevel_Debug << "Setting sampling rate to " << l_ui64SamplingRate << "\n";

			m_pStreamEncoder->encodeHeader();

			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, c), l_rDynamicBoxContext.getInputChunkEndTime(0, c));
		}

		if (l_ui32BufferCount)
		{
			// We have received one buffer from each input
			IMatrix* ip_pMatrix = m_pStreamEncoder->getInputMatrix();

			l_ui32SampleCountPerSentBlock = ip_pMatrix->getDimensionSize(1);

			for (uint32 i = 0, k = 0; i < l_rStaticBoxContext.getInputCount(); i++)
			{
				IMatrix* op_pMatrix = m_vStreamDecoder[i]->getOutputMatrix();
				for (uint32 j = 0; j < op_pMatrix->getDimensionSize(0); j++, k++)
				{
					System::Memory::copy(ip_pMatrix->getBuffer() + k * l_ui32SampleCountPerSentBlock, op_pMatrix->getBuffer() + j * l_ui32SampleCountPerSentBlock, l_ui32SampleCountPerSentBlock * sizeof(double));
				}
			}
			m_pStreamEncoder->encodeBuffer();

			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, c), l_rDynamicBoxContext.getInputChunkEndTime(0, c));
		}

		if (l_ui32EndCount)
		{
			// We have received one end from each input
			m_pStreamEncoder->encodeEnd();
			l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, c), l_rDynamicBoxContext.getInputChunkEndTime(0, c));
		}
	}

	return true;
}
