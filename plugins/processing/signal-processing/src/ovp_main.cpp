
#include "algorithms/basic/ovpCMatrixAverage.h"
#include "algorithms/epoching/ovpCAlgorithmStimulationBasedEpoching.h"

#include "box-algorithms/basic/ovpCIdentity.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmChannelRename.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmChannelSelector.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmEpochAverage.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmCrop.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmSignalDecimation.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmReferenceChannel.h"
#include "box-algorithms/epoching/ovpCBoxAlgorithmStimulationBasedEpoching.h"
#include "box-algorithms/filters/ovpCBoxAlgorithmCommonAverageReference.h"
#include "box-algorithms/filters/ovpCBoxAlgorithmSpatialFilter.h"

#include "box-algorithms/spectral-analysis/ovpCBoxAlgorithmFrequencyBandSelector.h"
#include "box-algorithms/spectral-analysis/ovpCBoxAlgorithmSpectrumAverage.h"

#include "box-algorithms/ovpCTimeBasedEpoching.h"
#include "box-algorithms/ovpCSimpleDSP.h"
#include "box-algorithms/ovpCSignalAverage.h"

OVP_Declare_Begin()

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_EpochAverageMethod, "Epoch Average method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Moving epoch average", OVP_TypeId_EpochAverageMethod_MovingAverage.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Moving epoch average (Immediate)", OVP_TypeId_EpochAverageMethod_MovingAverageImmediate.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Epoch block average", OVP_TypeId_EpochAverageMethod_BlockAverage.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Cumulative average", OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_CropMethod, "Crop method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Min",     OVP_TypeId_CropMethod_Min.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Max",     OVP_TypeId_CropMethod_Max.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Min/Max", OVP_TypeId_CropMethod_MinMax.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_ComparisonMethod, "Comparison method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ComparisonMethod, "Ratio",            OVP_TypeId_ComparisonMethod_Ratio.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ComparisonMethod, "Substraction",     OVP_TypeId_ComparisonMethod_Substraction.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ComparisonMethod, "Laterality index", OVP_TypeId_ComparisonMethod_LateralityIndex.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_SelectionMethod, "Selection method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select", OVP_TypeId_SelectionMethod_Select.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Reject", OVP_TypeId_SelectionMethod_Reject.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select EEG", OVP_TypeId_SelectionMethod_Select_EEG.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_MatchMethod, "Match method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Name",  OVP_TypeId_MatchMethod_Name.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Index", OVP_TypeId_MatchMethod_Index.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Smart", OVP_TypeId_MatchMethod_Smart.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CIdentityDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CTimeBasedEpochingDesc);

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CMatrixAverageDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CAlgorithmStimulationBasedEpochingDesc)

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmChannelRenameDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmChannelSelectorDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmReferenceChannelDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CEpochAverageDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmCropDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSignalDecimationDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmStimulationBasedEpochingDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmCommonAverageReferenceDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSpatialFilterDesc)

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CSimpleDSPDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CSignalAverageDesc)

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmFrequencyBandSelectorDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSpectrumAverageDesc)

OVP_Declare_End()
