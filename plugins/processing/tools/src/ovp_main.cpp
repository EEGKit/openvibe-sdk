#include "ovp_defines.h"

#include "box-algorithms/ovpCBoxAlgorithmEBMLStreamSpy.h"
#include "box-algorithms/ovpCBoxAlgorithmStimulationListener.h"
#include "box-algorithms/ovpCBoxAlgorithmMatrixValidityChecker.h"
#include "box-algorithms/ovpCBoxAlgorithmExternalProcessing.h"

namespace OpenViBE {
namespace Plugins {
namespace Tools {

OVP_Declare_Begin()
	// ValidityCheckerType: this type registration enables the choice between different action to do on an invalid stream
	context.getTypeManager().registerEnumerationType(OVP_TypeId_ValidityCheckerType, "Action to do");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Log warning", OVP_TypeId_ValidityCheckerType_LogWarning.id());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Stop player", OVP_TypeId_ValidityCheckerType_StopPlayer.id());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Interpolate", OVP_TypeId_ValidityCheckerType_Interpolate.id());

	OVP_Declare_New(CBoxAlgorithmStimulationListenerDesc);
	OVP_Declare_New(CBoxAlgorithmEBMLStreamSpyDesc);
	OVP_Declare_New(CBoxAlgorithmMatrixValidityCheckerDesc);
	OVP_Declare_New(CBoxAlgorithmExternalProcessingDesc);

OVP_Declare_End()

}  // namespace Tools
}  // namespace Plugins
}  // namespace OpenViBE
