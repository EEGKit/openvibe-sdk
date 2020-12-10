#include "ovpCStreamStructureDecoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
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

bool CStreamStructureDecoder::isMasterChild(const EBML::CIdentifier& identifier) { return CEBMLBaseDecoder::isMasterChild(identifier); }
void CStreamStructureDecoder::openChild(const EBML::CIdentifier& identifier) { CEBMLBaseDecoder::openChild(identifier); }
void CStreamStructureDecoder::processChildData(const void* buffer, const size_t size) { CEBMLBaseDecoder::processChildData(buffer, size); }
void CStreamStructureDecoder::closeChild() { CEBMLBaseDecoder::closeChild(); }
