#include "ovpCBoxAlgorithmStreamedMatrixMultiplexer.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Streaming;

bool CBoxAlgorithmStreamedMatrixMultiplexer::initialize()
{
	m_lastStartTime = 0;
	m_lastEndTime   = 0;
	m_bHeaderSent   = false;

	return true;
}

bool CBoxAlgorithmStreamedMatrixMultiplexer::uninitialize() { return true; }

bool CBoxAlgorithmStreamedMatrixMultiplexer::processInput(const uint32_t ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStreamedMatrixMultiplexer::process()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext    = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		for (uint32_t j = 0; j < l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			const IMemoryBuffer* l_pInputMemoryBuffer = l_rDynamicBoxContext.getInputChunk(i, j);
			uint64_t startTime                        = l_rDynamicBoxContext.getInputChunkStartTime(i, j);
			uint64_t endTime                          = l_rDynamicBoxContext.getInputChunkEndTime(i, j);

			if ((!m_bHeaderSent && startTime == endTime) || (m_bHeaderSent && startTime != endTime))
			{
				IMemoryBuffer* l_pOutputMemoryBuffer = l_rDynamicBoxContext.getOutputChunk(0);
				l_pOutputMemoryBuffer->setSize(l_pInputMemoryBuffer->getSize(), true);

				System::Memory::copy(l_pOutputMemoryBuffer->getDirectPointer(), l_pInputMemoryBuffer->getDirectPointer(), l_pInputMemoryBuffer->getSize());

				OV_ERROR_UNLESS_KRF(startTime >= m_lastStartTime && endTime >= m_lastEndTime,
									"Invalid chunk times with start = [" << startTime << "] and end = [" << endTime << "] while last chunk has start = [" << m_lastStartTime << "] and end = [" << m_lastEndTime << "]",
									OpenViBE::Kernel::ErrorType::BadInput);

				m_lastStartTime = startTime;
				m_lastEndTime   = endTime;

				l_rDynamicBoxContext.markOutputAsReadyToSend(0, startTime, endTime);
				m_bHeaderSent = true;
			}

			l_rDynamicBoxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
