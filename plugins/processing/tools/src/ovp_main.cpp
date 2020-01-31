#include "ovp_defines.h"

#include "box-algorithms/ovpCBoxAlgorithmEBMLStreamSpy.h"
#include "box-algorithms/ovpCBoxAlgorithmStimulationListener.h"
#include "box-algorithms/ovpCBoxAlgorithmMatrixValidityChecker.h"
#include "box-algorithms/ovpCBoxAlgorithmExternalProcessing.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;

OVP_Declare_Begin()

	// ValidityCheckerType: this type registration enables the choice between different action to do on an invalid stream
	context.getTypeManager().registerEnumerationType(OVP_TypeId_ValidityCheckerType, "Action to do");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Log warning", OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Stop player", OVP_TypeId_ValidityCheckerType_StopPlayer.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Interpolate", OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger());

	OVP_Declare_New(Tools::CBoxAlgorithmStimulationListenerDesc);
	OVP_Declare_New(Tools::CBoxAlgorithmEBMLStreamSpyDesc);
	OVP_Declare_New(Tools::CBoxAlgorithmMatrixValidityCheckerDesc);
	OVP_Declare_New(Tools::CBoxAlgorithmExternalProcessingDesc);

OVP_Declare_End()
