#include "ovpCBoxAlgorithmIdentity.h"

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {

bool CBoxAlgorithmIdentity::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmIdentity::process()
{
	Kernel::IBoxIO* boxCtx = getBoxAlgorithmContext()->getDynamicBoxContext();
	const size_t nInput    = getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount();
	CTime tStart           = 0;
	CTime tEnd             = 0;
	size_t size            = 0;
	const uint8_t* buffer  = nullptr;

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxCtx->getInputChunkCount(i); ++j)
		{
			boxCtx->getInputChunk(i, j, tStart, tEnd, size, buffer);
			boxCtx->appendOutputChunkData(i, buffer, size);
			boxCtx->markOutputAsReadyToSend(i, tStart, tEnd);
			boxCtx->markInputAsDeprecated(i, j);
		}
	}

	return true;
}

}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
