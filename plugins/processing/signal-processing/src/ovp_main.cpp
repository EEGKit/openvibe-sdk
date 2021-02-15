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

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {

OVP_Declare_Begin()
	context.getTypeManager().registerEnumerationType(OVP_TypeId_EpochAverageMethod, "Epoch Average method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Moving epoch average", size_t(EEpochAverageMethod::Moving));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Moving epoch average (Immediate)",
													  size_t(EEpochAverageMethod::MovingImmediate));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Epoch block average", size_t(EEpochAverageMethod::Block));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_EpochAverageMethod, "Cumulative average", size_t(EEpochAverageMethod::Cumulative));

	context.getTypeManager().registerEnumerationType(OVP_TypeId_CropMethod, "Crop method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Min", size_t(ECropMethod::Min));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Max", size_t(ECropMethod::Max));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_CropMethod, "Min/Max", size_t(ECropMethod::MinMax));

	context.getTypeManager().registerEnumerationType(OVP_TypeId_SelectionMethod, "Selection method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select", size_t(ESelectionMethod::Select));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Reject", size_t(ESelectionMethod::Reject));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_SelectionMethod, "Select EEG", size_t(ESelectionMethod::Select_EEG));

	context.getTypeManager().registerEnumerationType(OVP_TypeId_MatchMethod, "Match method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Name", size_t(EMatchMethod::Name));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Index", size_t(EMatchMethod::Index));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_MatchMethod, "Smart", size_t(EMatchMethod::Smart));


	// Temporal filter
	context.getTypeManager().registerEnumerationType(OVP_TypeId_FilterMethod, "Filter method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Butterworth", size_t(EFilterMethod::Butterworth));
	// context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Chebishev", size_t(EFilterMethod::Chebyshev));
	// context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterMethod, "Yule Walked", size_t(EFilterMethod::YuleWalker));

	context.getTypeManager().registerEnumerationType(OVP_TypeId_FilterType, "Filter type");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Low Pass", size_t(EFilterType::LowPass));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "High Pass", size_t(EFilterType::HighPass));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band Pass", size_t(EFilterType::BandPass));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_FilterType, "Band Stop", size_t(EFilterType::BandStop));

	OVP_Declare_New(CAlgorithmMatrixAverageDesc)

	OVP_Declare_New(CBoxAlgorithmIdentityDesc);
	OVP_Declare_New(CBoxAlgorithmTimeBasedEpochingDesc);
	OVP_Declare_New(CBoxAlgorithmChannelRenameDesc)
	OVP_Declare_New(CBoxAlgorithmChannelSelectorDesc)
	OVP_Declare_New(CBoxAlgorithmReferenceChannelDesc)
	OVP_Declare_New(CBoxAlgorithmEpochAverageDesc)
	OVP_Declare_New(CBoxAlgorithmCropDesc)
	OVP_Declare_New(CBoxAlgorithmSignalDecimationDesc)
	OVP_Declare_New(CBoxAlgorithmZeroCrossingDetectorDesc)
	OVP_Declare_New(CBoxAlgorithmStimulationBasedEpochingDesc)
	OVP_Declare_New(CBoxAlgorithmCommonAverageReferenceDesc)

	OVP_Declare_New(CBoxAlgorithmSpatialFilterDesc)
	OVP_Declare_New(CBoxAlgorithmTemporalFilterDesc)

#if defined TARGET_HAS_ThirdPartyEIGEN
	context.getTypeManager().registerEnumerationType(OVP_TypeId_OnlineCovariance_UpdateMethod, "Update method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_OnlineCovariance_UpdateMethod, "Chunk average", size_t(EUpdateMethod::ChunkAverage));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_OnlineCovariance_UpdateMethod, "Per sample", size_t(EUpdateMethod::Incremental));

	OVP_Declare_New(CBoxAlgorithmRegularizedCSPTrainerDesc)
	OVP_Declare_New(CAlgorithmOnlineCovarianceDesc)

#endif

#if defined TARGET_HAS_R8BRAIN
	OVP_Declare_New(CBoxAlgorithmSignalResamplingDesc)
#endif

	OVP_Declare_New(CBoxAlgorithmSimpleDSPDesc)
	OVP_Declare_New(CSignalAverageDesc)

	// Wavelet Type
	context.getTypeManager().registerEnumerationType(OVP_TypeId_ContinuousWaveletType, "Continuous Wavelet Type");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Morlet wavelet", size_t(EContinuousWaveletType::Morlet));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Paul wavelet", size_t(EContinuousWaveletType::Paul));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ContinuousWaveletType, "Derivative of Gaussian wavelet", size_t(EContinuousWaveletType::DOG));

	OVP_Declare_New(CBoxAlgorithmContinuousWaveletAnalysisDesc);

	OVP_Declare_New(CBoxAlgorithmFrequencyBandSelectorDesc)
	OVP_Declare_New(CBoxAlgorithmSpectrumAverageDesc)

	OVP_Declare_New(CBoxAlgorithmSpectralAnalysisDesc);
	OVP_Declare_New(CBoxAlgorithmWindowingDesc);
	context.getTypeManager().registerEnumerationType(OVP_TypeId_WindowMethod, "Window method");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "None", size_t(EWindowMethod::None));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hamming", size_t(EWindowMethod::Hamming));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hanning", size_t(EWindowMethod::Hanning));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Hann", size_t(EWindowMethod::Hann));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Blackman", size_t(EWindowMethod::Blackman));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Triangular", size_t(EWindowMethod::Triangular));
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_WindowMethod, "Square root", size_t(EWindowMethod::SquareRoot));

	OVP_Declare_New(CBoxAlgorithmXDAWNTrainerDesc);

OVP_Declare_End()

}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
