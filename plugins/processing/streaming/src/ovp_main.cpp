#include "ovp_defines.h"

#include "box-algorithms/ovpCBoxAlgorithmStreamedMatrixMultiplexer.h"
#include "box-algorithms/ovpCBoxAlgorithmSignalMerger.h"

#include <vector>

namespace OpenViBE {
namespace Plugins {
namespace Streaming {

OVP_Declare_Begin()
	OVP_Declare_New(CBoxAlgorithmStreamedMatrixMultiplexerDesc)
	OVP_Declare_New(CBoxAlgorithmSignalMergerDesc)
OVP_Declare_End()

}  // namespace Streaming
}  // namespace Plugins
}  // namespace OpenViBE
