#include "ovpCBoxAlgorithmStreamedMatrixMultiplexer.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Streaming;

bool CBoxAlgorithmStreamedMatrixMultiplexer::initialize(void)
{
	m_ui64LastStartTime = 0;
	m_ui64LastEndTime = 0;
	m_bHeaderSent = false;

	return true;
}

bool CBoxAlgorithmStreamedMatrixMultiplexer::uninitialize(void)
{
	return true;
}

bool CBoxAlgorithmStreamedMatrixMultiplexer::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStreamedMatrixMultiplexer::process(void)
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	for (uint32 i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		for (uint32 j = 0; j < l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			const IMemoryBuffer* l_pInputMemoryBuffer = l_rDynamicBoxContext.getInputChunk(i, j);
			uint64 l_ui64StartTime = l_rDynamicBoxContext.getInputChunkStartTime(i, j);
			uint64 l_ui64EndTime = l_rDynamicBoxContext.getInputChunkEndTime(i, j);

			if ((!m_bHeaderSent && l_ui64StartTime == l_ui64EndTime) || (m_bHeaderSent && l_ui64StartTime != l_ui64EndTime))
			{
				IMemoryBuffer* l_pOutputMemoryBuffer = l_rDynamicBoxContext.getOutputChunk(0);
				l_pOutputMemoryBuffer->setSize(l_pInputMemoryBuffer->getSize(), true);

				System::Memory::copy(
					l_pOutputMemoryBuffer->getDirectPointer(),
					l_pInputMemoryBuffer->getDirectPointer(),
					l_pInputMemoryBuffer->getSize());

				OV_ERROR_UNLESS_KRF(
					l_ui64StartTime >= m_ui64LastStartTime && l_ui64EndTime >= m_ui64LastEndTime,
					"Invalid chunk times with start = [" << l_ui64StartTime << "] and end = [" << l_ui64EndTime
					<< "] while last chunk has start = [" << m_ui64LastStartTime << "] and end = [" << m_ui64LastEndTime << "]",
					OpenViBE::Kernel::ErrorType::BadInput
				);

				m_ui64LastStartTime = l_ui64StartTime;
				m_ui64LastEndTime = l_ui64EndTime;

				l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
				m_bHeaderSent = true;
			}

			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
