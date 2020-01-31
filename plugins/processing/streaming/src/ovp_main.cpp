#include "ovp_defines.h"

#include "box-algorithms/ovpCBoxAlgorithmStreamedMatrixMultiplexer.h"
#include "box-algorithms/ovpCBoxAlgorithmSignalMerger.h"

#include <vector>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;

OVP_Declare_Begin()
	OVP_Declare_New(Streaming::CBoxAlgorithmStreamedMatrixMultiplexerDesc);
	OVP_Declare_New(Streaming::CBoxAlgorithmSignalMergerDesc);
OVP_Declare_End()
