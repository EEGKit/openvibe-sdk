#include "ovpCFeatureVectorEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace StreamCodecs;


bool CFeatureVectorEncoder::processHeader()
{
	OV_ERROR_UNLESS_KRF(ip_matrix->getDimensionCount() == 1, "Invalid feature vector: found " << ip_matrix->getDimensionCount() << " dimensions, 1 expected",
						Kernel::ErrorType::BadInput);

	CStreamedMatrixEncoder::processHeader();

	return true;
}
