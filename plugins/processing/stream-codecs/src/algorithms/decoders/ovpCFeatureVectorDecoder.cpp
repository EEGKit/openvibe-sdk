#include "ovpCFeatureVectorDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;


void CFeatureVectorDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_oTop = identifier;
	CStreamedMatrixDecoder::openChild(identifier);
}

void CFeatureVectorDecoder::processChildData(const void* buffer, const uint64_t size)
{
	// Check for conforming dimension count, then pass to matrix decoder
	if (m_oTop == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
	{
		const uint32_t nDim = uint32_t(m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size));
		OV_ERROR_UNLESS_KRV(nDim == 1, "Invalid feature vector: found " << nDim << " dimensions, 1 expected", OpenViBE::Kernel::ErrorType::BadInput);
	}

	CStreamedMatrixDecoder::processChildData(buffer, size);
}
