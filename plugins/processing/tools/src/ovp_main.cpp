#include "ovp_defines.h"

#include "box-algorithms/ovpCBoxAlgorithmEBMLStreamSpy.h"
#include "box-algorithms/ovpCBoxAlgorithmStimulationListener.h"

#include "box-algorithms/ovpCBoxAlgorithmMatrixValidityChecker.h"

OVP_Declare_Begin();

	// ValidityCheckerType: this type registration enables the choice between different action to do on an invalid stream
	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_ValidityCheckerType, "Action to do");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Log warning", OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Stop player", OVP_TypeId_ValidityCheckerType_StopPlayer.toUInteger());		
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ValidityCheckerType, "Interpolate", OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmStimulationListenerDesc);
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmEBMLStreamSpyDesc);
	OVP_Declare_New(OpenViBEPlugins::Tools::CBoxAlgorithmMatrixValidityCheckerDesc);
OVP_Declare_End();
