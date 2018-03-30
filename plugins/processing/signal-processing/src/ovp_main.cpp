
#include "algorithms/basic/ovpCAlgorithmMatrixAverage.h"

#include "box-algorithms/basic/ovpCBoxAlgorithmIdentity.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmChannelRename.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmChannelSelector.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmEpochAverage.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmCrop.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmSignalDecimation.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmReferenceChannel.h"
#include "box-algorithms/basic/ovpCBoxAlgorithmZeroCrossingDetector.h"
#include "box-algorithms/epoching/ovpCBoxAlgorithmStimulationBasedEpoching.h"
#include "box-algorithms/epoching/ovpCBoxAlgorithmTimeBasedEpoching.h"
#include "box-algorithms/filters/ovpCBoxAlgorithmCommonAverageReference.h"
#include "box-algorithms/filters/ovpCBoxAlgorithmSpatialFilter.h"
#include "box-algorithms/filters/ovpCBoxAlgorithmTemporalFilter.h"

#include "box-algorithms/filters/ovpCBoxAlgorithmRegularizedCSPTrainer.h"
#include "algorithms/basic/ovpCAlgorithmOnlineCovariance.h"

#include "box-algorithms/spectral-analysis/ovpCBoxAlgorithmContinuousWaveletAnalysis.h"
#include "box-algorithms/spectral-analysis/ovpCBoxAlgorithmSpectralAnalysis.h"
#include "box-algorithms/spectral-analysis/ovpCBoxAlgorithmFrequencyBandSelector.h"
#include "box-algorithms/spectral-analysis/ovpCBoxAlgorithmSpectrumAverage.h"

#include "box-algorithms/resampling/ovpCBoxAlgorithmSignalResampling.h"

#include "box-algorithms/ovpCBoxAlgorithmSimpleDSP.h"
#include "box-algorithms/ovpCBoxAlgorithmSignalAverage.h"
#include "box-algorithms/ovpCBoxAlgorithmWindowing.h"
#include "box-algorithms/ovpCBoxAlgorithmXDAWNTrainer.h"

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

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_SelectionMethod, "Selection method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select", OVP_TypeId_SelectionMethod_Select.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Reject", OVP_TypeId_SelectionMethod_Reject.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select EEG", OVP_TypeId_SelectionMethod_Select_EEG.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_MatchMethod, "Match method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Name",  OVP_TypeId_MatchMethod_Name.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Index", OVP_TypeId_MatchMethod_Index.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Smart", OVP_TypeId_MatchMethod_Smart.toUInteger());


	// Temporal filter
	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_FilterMethod, "Filter method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Butterworth", OVP_TypeId_FilterMethod_Butterworth.toUInteger());
//	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Chebishev", OVP_TypeId_FilterMethod_Chebyshev.toUInteger());
//	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Yule Walked", OVP_TypeId_FilterMethod_YuleWalker.toUInteger());

	rPluginModuleContext.getTypeManager().registerEnumerationType (OVP_TypeId_FilterType, "Filter type");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Low Pass", OVP_TypeId_FilterType_LowPass.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "High Pass", OVP_TypeId_FilterType_HighPass.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band Pass", OVP_TypeId_FilterType_BandPass.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band Stop", OVP_TypeId_FilterType_BandStop.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CAlgorithmMatrixAverageDesc)

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmIdentityDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmTimeBasedEpochingDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmChannelRenameDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmChannelSelectorDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmReferenceChannelDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmEpochAverageDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmCropDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSignalDecimationDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmZeroCrossingDetectorDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmStimulationBasedEpochingDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmCommonAverageReferenceDesc)

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSpatialFilterDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmTemporalFilterDesc)

#if defined TARGET_HAS_ThirdPartyEIGEN
	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_OnlineCovariance_UpdateMethod, "Update method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_OnlineCovariance_UpdateMethod,"Chunk average",OVP_TypeId_OnlineCovariance_UpdateMethod_ChunkAverage.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_OnlineCovariance_UpdateMethod,"Per sample",OVP_TypeId_OnlineCovariance_UpdateMethod_Incremental.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmRegularizedCSPTrainerDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CAlgorithmOnlineCovarianceDesc)

#endif

#if defined TARGET_HAS_R8BRAIN
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSignalResamplingDesc)
#endif

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSimpleDSPDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CSignalAverageDesc)

	// Wavelet Type
	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_ContinuousWaveletType, "Continuous Wavelet Type");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Morlet wavelet", OVP_TypeId_ContinuousWaveletType_Morlet.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Paul wavelet", OVP_TypeId_ContinuousWaveletType_Paul.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Derivative of Gaussian wavelet", OVP_TypeId_ContinuousWaveletType_DOG.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmContinuousWaveletAnalysisDesc);

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmFrequencyBandSelectorDesc)
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSpectrumAverageDesc)

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSpectralAnalysisDesc);
	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmWindowingDesc);
	rPluginModuleContext.getTypeManager().registerEnumerationType(OVP_TypeId_WindowMethod, "Window method");
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "None", OVP_TypeId_WindowMethod_None.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hamming", OVP_TypeId_WindowMethod_Hamming.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hanning", OVP_TypeId_WindowMethod_Hanning.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hann", OVP_TypeId_WindowMethod_Hann.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Blackman", OVP_TypeId_WindowMethod_Blackman.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Triangular", OVP_TypeId_WindowMethod_Triangular.toUInteger());
	rPluginModuleContext.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Square root", OVP_TypeId_WindowMethod_SquareRoot.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::SignalProcessing::CBoxAlgorithmXDAWNTrainerDesc);

OVP_Declare_End()
