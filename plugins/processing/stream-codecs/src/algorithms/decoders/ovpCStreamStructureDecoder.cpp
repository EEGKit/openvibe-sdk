#include "ovpCStreamStructureDecoder.h"

#include <cstring>
#include <cstdlib>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CStreamStructureDecoder::CStreamStructureDecoder() {}

// ________________________________________________________________________________________________________________
//

bool CStreamStructureDecoder::initialize()
{
	CEBMLBaseDecoder::initialize();
	return true;
}

bool CStreamStructureDecoder::uninitialize()
{
	CEBMLBaseDecoder::uninitialize();
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStreamStructureDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier) { return CEBMLBaseDecoder::isMasterChild(rIdentifier); }

void CStreamStructureDecoder::openChild(const EBML::CIdentifier& rIdentifier) { CEBMLBaseDecoder::openChild(rIdentifier); }

void CStreamStructureDecoder::processChildData(const void* pBuffer, const uint64_t size)
{
	CEBMLBaseDecoder::processChildData(pBuffer, size);
}

void CStreamStructureDecoder::closeChild() { CEBMLBaseDecoder::closeChild(); }
