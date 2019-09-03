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
		const uint32_t l_ui32DimensionCount = uint32_t(m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size));

		OV_ERROR_UNLESS_KRV(l_ui32DimensionCount == 1, "Invalid feature vector: found " << l_ui32DimensionCount << " dimensions, 1 expected",
							OpenViBE::Kernel::ErrorType::BadInput);
	}

	CStreamedMatrixDecoder::processChildData(buffer, size);
}
