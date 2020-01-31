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

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;

OVP_Declare_Begin()
	context.getTypeManager().registerEnumerationType(OVP_TypeId_EpochAverageMethod, "Epoch Average method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Moving epoch average", Moving);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Moving epoch average (Immediate)", MovingImmediate);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Epoch block average", Block);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Cumulative average", Cumulative);

	context.getTypeManager().registerEnumerationType(OVP_TypeId_CropMethod, "Crop method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Min", Min);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Max", Max);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Min/Max", MinMax);

	context.getTypeManager().registerEnumerationType(OVP_TypeId_SelectionMethod, "Selection method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select", Select);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Reject", Reject);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select EEG", Select_EEG);

	context.getTypeManager().registerEnumerationType(OVP_TypeId_MatchMethod, "Match method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Name", Name);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Index", Index);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Smart", Smart);


	// Temporal filter
	context.getTypeManager().registerEnumerationType(OVP_TypeId_FilterMethod, "Filter method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Butterworth", Butterworth);
	// context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Chebishev", EFilterMethod::Chebyshev);
	// context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Yule Walked", EFilterMethod::YuleWalker);

	context.getTypeManager().registerEnumerationType(OVP_TypeId_FilterType, "Filter type");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Low Pass", LowPass);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "High Pass", HighPass);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band Pass", BandPass);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band Stop", BandStop);

	OVP_Declare_New(SignalProcessing::CAlgorithmMatrixAverageDesc)

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmIdentityDesc);
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmTimeBasedEpochingDesc);
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmChannelRenameDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmChannelSelectorDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmReferenceChannelDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmEpochAverageDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmCropDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmSignalDecimationDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmZeroCrossingDetectorDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmStimulationBasedEpochingDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmCommonAverageReferenceDesc)

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmSpatialFilterDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmTemporalFilterDesc)

#if defined TARGET_HAS_ThirdPartyEIGEN
	context.getTypeManager().registerEnumerationType(OVP_TypeId_OnlineCovariance_UpdateMethod, "Update method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_OnlineCovariance_UpdateMethod, "Chunk average", ChunkAverage);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_OnlineCovariance_UpdateMethod, "Per sample",Incremental);

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmRegularizedCSPTrainerDesc)
	OVP_Declare_New(SignalProcessing::CAlgorithmOnlineCovarianceDesc)

#endif

#if defined TARGET_HAS_R8BRAIN
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmSignalResamplingDesc)
#endif

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmSimpleDSPDesc)
	OVP_Declare_New(SignalProcessing::CSignalAverageDesc)

	// Wavelet Type
	context.getTypeManager().registerEnumerationType(OVP_TypeId_ContinuousWaveletType, "Continuous Wavelet Type");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Morlet wavelet", Morlet);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Paul wavelet", Paul);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Derivative of Gaussian wavelet", DOG);

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmContinuousWaveletAnalysisDesc);

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmFrequencyBandSelectorDesc)
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmSpectrumAverageDesc)

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmSpectralAnalysisDesc);
	OVP_Declare_New(SignalProcessing::CBoxAlgorithmWindowingDesc);
	context.getTypeManager().registerEnumerationType(OVP_TypeId_WindowMethod, "Window method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "None", None);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hamming", Hamming);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hanning", Hanning);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hann", Hann);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Blackman", Blackman);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Triangular", Triangular);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Square root", SquareRoot);

	OVP_Declare_New(SignalProcessing::CBoxAlgorithmXDAWNTrainerDesc);

OVP_Declare_End()
