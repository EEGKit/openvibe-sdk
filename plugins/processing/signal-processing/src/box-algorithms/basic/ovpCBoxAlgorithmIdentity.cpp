#include "ovpCBoxAlgorithmIdentity.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

void CBoxAlgorithmIdentity::release() { delete this; }

bool CBoxAlgorithmIdentity::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmIdentity::process()
{
	IBoxIO* boxContext    = getBoxAlgorithmContext()->getDynamicBoxContext();
	const size_t nInput   = getBoxAlgorithmContext()->getStaticBoxContext()->getInputCount();
	uint64_t tStart       = 0;
	uint64_t tEnd         = 0;
	size_t size           = 0;
	const uint8_t* buffer = nullptr;

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxContext->getInputChunkCount(i); ++j)
		{
			boxContext->getInputChunk(i, j, tStart, tEnd, size, buffer);
			boxContext->appendOutputChunkData(i, buffer, size);
			boxContext->markOutputAsReadyToSend(i, tStart, tEnd);
			boxContext->markInputAsDeprecated(i, j);
		}
	}

	return true;
}
