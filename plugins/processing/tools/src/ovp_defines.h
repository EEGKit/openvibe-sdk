#pragma once

// Boxes
//---------------------------------------------------------------------------------------------------
#define OVP_ClassId_BoxAlgorithm_EBMLStreamSpy				OpenViBE::CIdentifier(0x0ED76695, 0x01A69CC3)
#define OVP_ClassId_BoxAlgorithm_EBMLStreamSpyDesc			OpenViBE::CIdentifier(0x354A6864, 0x06BC570C)
#define OVP_ClassId_BoxAlgorithm_ExternalProcessing			OpenViBE::CIdentifier(0x15422959, 0x16304449)
#define OVP_ClassId_BoxAlgorithm_ExternalProcessingDesc		OpenViBE::CIdentifier(0x63386942, 0x61D42502)
#define OVP_ClassId_BoxAlgorithm_MatrixValidityChecker		OpenViBE::CIdentifier(0x60210579, 0x6F7519B6)
#define OVP_ClassId_BoxAlgorithm_MatrixValidityCheckerDesc	OpenViBE::CIdentifier(0x6AFC2671, 0x1D8C493C)
#define OVP_ClassId_BoxAlgorithm_StimulationListener		OpenViBE::CIdentifier(0x65731E1D, 0x47DE5276)
#define OVP_ClassId_BoxAlgorithm_StimulationListenerDesc	OpenViBE::CIdentifier(0x0EC013FD, 0x5DD23E44)

// Global defines
//---------------------------------------------------------------------------------------------------
#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
#include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#define OVP_TypeId_ValidityCheckerType						OpenViBE::CIdentifier(0x32EA493A, 0x11E56D82)
#define OVP_TypeId_ValidityCheckerType_LogWarning			OpenViBE::CIdentifier(0x747A0F84, 0x1097253A)
#define OVP_TypeId_ValidityCheckerType_StopPlayer			OpenViBE::CIdentifier(0x4EC06D50, 0x5B131CE2)
#define OVP_TypeId_ValidityCheckerType_Interpolate			OpenViBE::CIdentifier(0x1DE96E02, 0x53767550)
