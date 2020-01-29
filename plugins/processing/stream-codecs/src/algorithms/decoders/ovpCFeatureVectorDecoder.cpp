#include "ovpCFeatureVectorDecoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;


void CFeatureVectorDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_oTop = identifier;
	CStreamedMatrixDecoder::openChild(identifier);
}

void CFeatureVectorDecoder::processChildData(const void* buffer, const size_t size)
{
	// Check for conforming dimension count, then pass to matrix decoder
	if (m_oTop == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
	{
		const size_t nDim = size_t(m_readerHelper->getUInt(buffer, size));
		OV_ERROR_UNLESS_KRV(nDim == 1, "Invalid feature vector: found " << nDim << " dimensions, 1 expected", OpenViBE::Kernel::ErrorType::BadInput);
	}

	CStreamedMatrixDecoder::processChildData(buffer, size);
}
