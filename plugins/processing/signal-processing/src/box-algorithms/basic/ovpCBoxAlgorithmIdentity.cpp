#include "ovpCBoxAlgorithmIdentity.h"

#include <iostream>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace SignalProcessing;
using namespace OpenViBEToolkit;
using namespace std;

void CBoxAlgorithmIdentity::release()
{
	delete this;
}

bool CBoxAlgorithmIdentity::processInput(uint32_t ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmIdentity::process()
{
	const IBox* l_pStaticBoxContext = getBoxAlgorithmContext()->getStaticBoxContext();
	IBoxIO* l_pDynamicBoxContext    = getBoxAlgorithmContext()->getDynamicBoxContext();

	uint64_t l_ui64StartTime      = 0;
	uint64_t l_ui64EndTime        = 0;
	uint64_t l_ui64ChunkSize      = 0;
	const uint8_t* l_pChunkBuffer = NULL;

	for (uint32_t i = 0; i < l_pStaticBoxContext->getInputCount(); i++)
	{
		for (uint32_t j = 0; j < l_pDynamicBoxContext->getInputChunkCount(i); j++)
		{
			l_pDynamicBoxContext->getInputChunk(i, j, l_ui64StartTime, l_ui64EndTime, l_ui64ChunkSize, l_pChunkBuffer);
			l_pDynamicBoxContext->appendOutputChunkData(i, l_pChunkBuffer, l_ui64ChunkSize);
			l_pDynamicBoxContext->markOutputAsReadyToSend(i, l_ui64StartTime, l_ui64EndTime);
			l_pDynamicBoxContext->markInputAsDeprecated(i, j);
		}
	}

	return true;
}
