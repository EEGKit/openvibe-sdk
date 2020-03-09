#include "ovpCBoxAlgorithmStreamedMatrixMultiplexer.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::Plugins::*/Streaming;

bool CBoxAlgorithmStreamedMatrixMultiplexer::initialize()
{
	m_lastStartTime = 0;
	m_lastEndTime   = 0;
	m_headerSent    = false;

	return true;
}

bool CBoxAlgorithmStreamedMatrixMultiplexer::uninitialize() { return true; }

bool CBoxAlgorithmStreamedMatrixMultiplexer::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStreamedMatrixMultiplexer::process()
{
	IBoxIO& boxContext  = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
		{
			const IMemoryBuffer* iBuffer = boxContext.getInputChunk(i, j);
			const uint64_t tStart        = boxContext.getInputChunkStartTime(i, j);
			const uint64_t tEnd          = boxContext.getInputChunkEndTime(i, j);

			if ((!m_headerSent && tStart == tEnd) || (m_headerSent && tStart != tEnd))
			{
				IMemoryBuffer* oBuffer = boxContext.getOutputChunk(0);
				oBuffer->setSize(iBuffer->getSize(), true);

				memcpy(oBuffer->getDirectPointer(), iBuffer->getDirectPointer(), iBuffer->getSize());

				OV_ERROR_UNLESS_KRF(tStart >= m_lastStartTime && tEnd >= m_lastEndTime,
									"Invalid chunk times with start = [" << tStart << "] and end = [" << tEnd << "] while last chunk has start = [" <<
									m_lastStartTime << "] and end = [" << m_lastEndTime << "]",
									ErrorType::BadInput);

				m_lastStartTime = tStart;
				m_lastEndTime   = tEnd;

				boxContext.markOutputAsReadyToSend(0, tStart, tEnd);
				m_headerSent = true;
			}

			boxContext.markInputAsDeprecated(i, j);
		}
	}

	return true;
}
