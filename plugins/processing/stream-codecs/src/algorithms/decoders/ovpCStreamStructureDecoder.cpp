#include "ovpCStreamStructureDecoder.h"

#include <cstring>
#include <cstdlib>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CStreamStructureDecoder::CStreamStructureDecoder(void) {}

// ________________________________________________________________________________________________________________
//

bool CStreamStructureDecoder::initialize(void)
{
	CEBMLBaseDecoder::initialize();
	return true;
}

bool CStreamStructureDecoder::uninitialize(void)
{
	CEBMLBaseDecoder::uninitialize();
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStreamStructureDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	return CEBMLBaseDecoder::isMasterChild(rIdentifier);
}

void CStreamStructureDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	CEBMLBaseDecoder::openChild(rIdentifier);
}

void CStreamStructureDecoder::processChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	CEBMLBaseDecoder::processChildData(pBuffer, ui64BufferSize);
}

void CStreamStructureDecoder::closeChild(void)
{
	CEBMLBaseDecoder::closeChild();
}
