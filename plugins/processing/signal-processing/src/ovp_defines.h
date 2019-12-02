#pragma once

// Boxes
//---------------------------------------------------------------------------------------------------
#define OVP_ClassId_Algorithm_MatrixAverage						OpenViBE::CIdentifier(0x5E5A6C1C, 0x6F6BEB03)
#define OVP_ClassId_Algorithm_MatrixAverageDesc					OpenViBE::CIdentifier(0x1992881F, 0xC938C0F2)
#define OVP_ClassId_Algorithm_OnlineCovariance					OpenViBE::CIdentifier(0x5ADD4F8E, 0x005D29C1)
#define OVP_ClassId_Algorithm_OnlineCovarianceDesc				OpenViBE::CIdentifier(0x00CD2DEA, 0x4C000CEB)
#define OVP_ClassId_BoxAlgorithm_ChannelRename					OpenViBE::CIdentifier(0x1FE50479, 0x39040F40)
#define OVP_ClassId_BoxAlgorithm_ChannelRenameDesc				OpenViBE::CIdentifier(0x20EA1F00, 0x7AED5645)
#define OVP_ClassId_BoxAlgorithm_ChannelSelector				OpenViBE::CIdentifier(0x361722E8, 0x311574E8)
#define OVP_ClassId_BoxAlgorithm_ChannelSelectorDesc			OpenViBE::CIdentifier(0x67633C1C, 0x0D610CD8)
#define OVP_ClassId_BoxAlgorithm_CommonAverageReference			OpenViBE::CIdentifier(0x009C0CE3, 0x6BDF71C3)
#define OVP_ClassId_BoxAlgorithm_CommonAverageReferenceDesc		OpenViBE::CIdentifier(0x0033EAF8, 0x09C65E4E)
#define OVP_ClassId_ContinuousWaveletAnalysis					OpenViBE::CIdentifier(0x0A43133D, 0x6EAF25A7)
#define OVP_ClassId_ContinuousWaveletAnalysisDesc				OpenViBE::CIdentifier(0x5B397A82, 0x76AE6F81)
#define OVP_ClassId_BoxAlgorithm_Crop							OpenViBE::CIdentifier(0x7F1A3002, 0x358117BA)
#define OVP_ClassId_BoxAlgorithm_CropDesc						OpenViBE::CIdentifier(0x64D619D7, 0x26CC42C9)
#define OVP_ClassId_BoxAlgorithm_EpochAverage					OpenViBE::CIdentifier(0x21283D9F, 0xE76FF640)
#define OVP_ClassId_BoxAlgorithm_EpochAverageDesc				OpenViBE::CIdentifier(0x95F5F43E, 0xBE629D82)
#define OVP_ClassId_BoxAlgorithm_FrequencyBandSelector			OpenViBE::CIdentifier(0x140C19C6, 0x4E6E187B)
#define OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc		OpenViBE::CIdentifier(0x13462C56, 0x794E3C07)
#define OVP_ClassId_BoxAlgorithm_Identity						OpenViBE::CIdentifier(0x5DFFE431, 0x35215C50)
#define OVP_ClassId_BoxAlgorithm_IdentityDesc					OpenViBE::CIdentifier(0x54743810, 0x6A1A88CC)
#define OVP_ClassId_BoxAlgorithm_ReferenceChannel				OpenViBE::CIdentifier(0x444721AD, 0x78342CF5)
#define OVP_ClassId_BoxAlgorithm_ReferenceChannelDesc			OpenViBE::CIdentifier(0x42856103, 0x45B125AD)
#define OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainer			OpenViBE::CIdentifier(0x2EC14CC0, 0x428C48BD)
#define OVP_ClassId_BoxAlgorithm_RegularizedCSPTrainerDesc		OpenViBE::CIdentifier(0x02205F54, 0x733C51EE)
#define OVP_ClassId_BoxAlgorithm_SignalAverage					OpenViBE::CIdentifier(0x00642C4D, 0x5DF7E50A)
#define OVP_ClassId_BoxAlgorithm_SignalAverageDesc				OpenViBE::CIdentifier(0x007CDCE9, 0x16034F77)
#define OVP_ClassId_BoxAlgorithm_SignalDecimation				OpenViBE::CIdentifier(0x012F4BEA, 0x3BE37C66)
#define OVP_ClassId_BoxAlgorithm_SignalDecimationDesc			OpenViBE::CIdentifier(0x1C5F1356, 0x1E685777)
#define OVP_ClassId_BoxAlgorithm_SignalResampling				OpenViBE::CIdentifier(0x0E923A5E, 0xDA474058)
#define OVP_ClassId_BoxAlgorithm_SignalResamplingDesc			OpenViBE::CIdentifier(0xA675A433, 0xC6690920)
#define OVP_ClassId_BoxAlgorithm_SimpleDSP						OpenViBE::CIdentifier(0x00E26FA1, 0x1DBAB1B2)
#define OVP_ClassId_BoxAlgorithm_SimpleDSPDesc					OpenViBE::CIdentifier(0x00C44BFE, 0x76C9269E)
#define OVP_ClassId_BoxAlgorithm_SpatialFilter					OpenViBE::CIdentifier(0xDD332C6C, 0x195B4FD4)
#define OVP_ClassId_BoxAlgorithm_SpatialFilterDesc				OpenViBE::CIdentifier(0x72A01C92, 0xF8C1FA24)
#define OVP_ClassId_SpectralAnalysis							OpenViBE::CIdentifier(0x84218FF8, 0xA87E7995)
#define OVP_ClassId_SpectralAnalysisDesc						OpenViBE::CIdentifier(0x0051E63C, 0x68E83AD1)
#define OVP_ClassId_BoxAlgorithm_SpectrumAverage				OpenViBE::CIdentifier(0x0C092665, 0x61B82641)
#define OVP_ClassId_BoxAlgorithm_SpectrumAverageDesc			OpenViBE::CIdentifier(0x24663D96, 0x71EA7295)
#define OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching		OpenViBE::CIdentifier(0x426163D1, 0x324237B0)
#define OVP_ClassId_BoxAlgorithm_StimulationBasedEpochingDesc	OpenViBE::CIdentifier(0x4F60616D, 0x468E0A8C)
#define OVP_ClassId_BoxAlgorithm_TemporalFilter					OpenViBE::CIdentifier(0xB4F9D042, 0x9D79F2E5)
#define OVP_ClassId_BoxAlgorithm_TemporalFilterDesc				OpenViBE::CIdentifier(0x7BF6BA62, 0xAF829A37)
#define OVP_ClassId_BoxAlgorithm_TimeBasedEpoching				OpenViBE::CIdentifier(0x00777FA0, 0x5DC3F560)
#define OVP_ClassId_BoxAlgorithm_TimeBasedEpochingDesc			OpenViBE::CIdentifier(0x00ABDABE, 0x41381683)
#define OVP_ClassId_Windowing									OpenViBE::CIdentifier(0x002034AE, 0x6509FD8F)
#define OVP_ClassId_WindowingDesc								OpenViBE::CIdentifier(0x602CF89F, 0x65BA6DA0)
#define OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer				OpenViBE::CIdentifier(0x27542F6E, 0x14AA3548)
#define OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainerDesc			OpenViBE::CIdentifier(0x128A6013, 0x370B5C2C)
#define OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector			OpenViBE::CIdentifier(0x0016663F, 0x096A46A6)
#define OVP_ClassId_BoxAlgorithm_ZeroCrossingDetectorDesc		OpenViBE::CIdentifier(0x63AA73A7, 0x1F0419A2)

// Type definitions
//---------------------------------------------------------------------------------------------------
// Filter method identifiers from OpenViBE 0.14.0
#define OVP_TypeId_FilterMethod									OpenViBE::CIdentifier(0x2F2C606C, 0x8512ED68)
#define OVP_TypeId_FilterMethod_Butterworth						OpenViBE::CIdentifier(0x00E4EADE, 0x9586A50E)
#define OVP_TypeId_FilterMethod_Chebyshev						OpenViBE::CIdentifier(0x0C3F5301, 0x7397A7FB)
#define OVP_TypeId_FilterMethod_YuleWalker						OpenViBE::CIdentifier(0x7BF59C86, 0xB6D459BA)

// Filter type identifiers from OpenViBE 0.14.0
#define OVP_TypeId_FilterType									OpenViBE::CIdentifier(0xFA20178E, 0x4CBA62E9)
#define OVP_TypeId_FilterType_LowPass							OpenViBE::CIdentifier(0x0C33A568, 0xE2BD46D2)
#define OVP_TypeId_FilterType_BandPass							OpenViBE::CIdentifier(0x19E89DDB, 0xD1FF7FF5)
#define OVP_TypeId_FilterType_HighPass							OpenViBE::CIdentifier(0x2F677195, 0x9F9F15B9)
#define OVP_TypeId_FilterType_BandStop							OpenViBE::CIdentifier(0x41C34DD7, 0xF70E4043)

#define OVP_TypeId_OnlineCovariance_UpdateMethod				OpenViBE::CIdentifier(0x59E83F33, 0x592F1DD0)
#define OVP_TypeId_OnlineCovariance_UpdateMethod_ChunkAverage	OpenViBE::CIdentifier(0x079E14D3, 0x784A2BD1)
#define OVP_TypeId_OnlineCovariance_UpdateMethod_Incremental	OpenViBE::CIdentifier(0x39E20E6D, 0x6A87073C)

#define OVP_TypeId_EpochAverageMethod							OpenViBE::CIdentifier(0x6530BDB1, 0xD057BBFE)
#define OVP_TypeId_EpochAverageMethod_MovingAverage				OpenViBE::CIdentifier(0x426377E7, 0xCF8E32CD)
#define OVP_TypeId_EpochAverageMethod_MovingAverageImmediate	OpenViBE::CIdentifier(0x1F830F88, 0xAA01A592)
#define OVP_TypeId_EpochAverageMethod_BlockAverage				OpenViBE::CIdentifier(0x2E4ACA75, 0x7E02B507)
#define OVP_TypeId_EpochAverageMethod_CumulativeAverage			OpenViBE::CIdentifier(0xC18311B7, 0x001C1953)

// Wavelet type
#define OVP_TypeId_ContinuousWaveletType						OpenViBE::CIdentifier(0x09177469, 0x52404583)
#define OVP_TypeId_ContinuousWaveletType_Morlet					OpenViBE::CIdentifier(0x40D21D50, 0x738A68ED)
#define OVP_TypeId_ContinuousWaveletType_Paul					OpenViBE::CIdentifier(0x2F007EC1, 0x2B870B4A)
#define OVP_TypeId_ContinuousWaveletType_DOG					OpenViBE::CIdentifier(0x4B2E79A5, 0x40C95CF5)

#define OVP_TypeId_CropMethod									OpenViBE::CIdentifier(0xD0643F9E, 0x8E35FE0A)
#define OVP_TypeId_CropMethod_Min								OpenViBE::CIdentifier(0x0CCC9DE4, 0x93F495D2)
#define OVP_TypeId_CropMethod_Max								OpenViBE::CIdentifier(0x2FFEB29C, 0xD8F21FB4)
#define OVP_TypeId_CropMethod_MinMax							OpenViBE::CIdentifier(0x3CEA8129, 0xA772413A)

#define OVP_TypeId_SelectionMethod								OpenViBE::CIdentifier(0x3BCF9E67, 0x0C23994D)
#define OVP_TypeId_SelectionMethod_Select						OpenViBE::CIdentifier(0x1970FAAF, 0x4FD1CC4D)
#define OVP_TypeId_SelectionMethod_Reject						OpenViBE::CIdentifier(0x4C05408D, 0x6EAC6F85)
#define OVP_TypeId_SelectionMethod_Select_EEG					OpenViBE::CIdentifier(0x600E3C4F, 0x6DD00C2D)

#define OVP_TypeId_MatchMethod									OpenViBE::CIdentifier(0x666F25E9, 0x3E5738D6)
#define OVP_TypeId_MatchMethod_Name								OpenViBE::CIdentifier(0x58032A35, 0x4393A5D2)
#define OVP_TypeId_MatchMethod_Index							OpenViBE::CIdentifier(0x0E0BF9E8, 0x3D612618)
#define OVP_TypeId_MatchMethod_Smart							OpenViBE::CIdentifier(0x2D87EF07, 0xA2828AC0)

// Global defines
//---------------------------------------------------------------------------------------------------
#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
#include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines


#define OVP_Value_CoupledStringSeparator									'-'
//#define OVP_Value_AllSelection											'*'


#define OVP_Algorithm_MatrixAverage_InputParameterId_Matrix							OpenViBE::CIdentifier(0x913E9C3B, 0x8A62F5E3)
#define OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount					OpenViBE::CIdentifier(0x08563191, 0xE78BB265)
#define OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod				OpenViBE::CIdentifier(0xE63CD759, 0xB6ECF6B7)
#define OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix				OpenViBE::CIdentifier(0x03CE5AE5, 0xBD9031E0)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_Reset							OpenViBE::CIdentifier(0x670EC053, 0xADFE3F5C)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix						OpenViBE::CIdentifier(0x50B6EE87, 0xDC42E660)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_ForceAverage						OpenViBE::CIdentifier(0xBF597839, 0xCD6039F0)
#define OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed				OpenViBE::CIdentifier(0x2BFF029B, 0xD932A613)
#define OVP_Algorithm_OnlineCovariance_InputParameterId_Shrinkage					OpenViBE::CIdentifier(0x16577C7B, 0x4E056BF7)
#define OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors				OpenViBE::CIdentifier(0x47E55F81, 0x27A519C4)
#define OVP_Algorithm_OnlineCovariance_InputParameterId_UpdateMethod				OpenViBE::CIdentifier(0x1C4F444F, 0x3CA213E2)
#define OVP_Algorithm_OnlineCovariance_InputParameterId_TraceNormalization			OpenViBE::CIdentifier(0x269D5E63, 0x3B6D486E)
#define OVP_Algorithm_OnlineCovariance_OutputParameterId_Mean						OpenViBE::CIdentifier(0x3F1F50A3, 0x05504D0E)
#define OVP_Algorithm_OnlineCovariance_OutputParameterId_CovarianceMatrix			OpenViBE::CIdentifier(0x203A5472, 0x67C5324C)
#define OVP_Algorithm_OnlineCovariance_Process_Reset								OpenViBE::CIdentifier(0x4C1C510C, 0x3CF56E7C) // to reset estimates to 0
#define OVP_Algorithm_OnlineCovariance_Process_Update								OpenViBE::CIdentifier(0x72BF2277, 0x2974747B) // update estimates with a new chunk of data
#define OVP_Algorithm_OnlineCovariance_Process_GetCov								OpenViBE::CIdentifier(0x2BBC4A91, 0x27050CFD) // also returns the mean estimate
#define OVP_Algorithm_OnlineCovariance_Process_GetCovRaw							OpenViBE::CIdentifier(0x0915148C, 0x5F792B2A) // also returns the mean estimate
#define OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_NewSampling				OpenViBE::CIdentifier(0x158A8EFD, 0xAA894F86)
#define OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_SampleCountPerBuffer	OpenViBE::CIdentifier(0x588783F3, 0x8E8DCF86)
#define OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_LowPassFilterSignalFlag	OpenViBE::CIdentifier(0xAFDD8EFD, 0x23EF94F6)

// Windowing: ID names and values taken from the signal-processing-gpl project
#define OVP_TypeId_WindowMethod					OpenViBE::CIdentifier(0x0A430FE4, 0x4F318280)
#define OVP_TypeId_WindowMethod_None			OpenViBE::CIdentifier(0x01DD2ACC, 0x347E581E)
#define OVP_TypeId_WindowMethod_Hamming			OpenViBE::CIdentifier(0x3A9FF7F1, 0x54E79D67)
#define OVP_TypeId_WindowMethod_Hanning			OpenViBE::CIdentifier(0x660DA3E7, 0x7BD87719)
#define OVP_TypeId_WindowMethod_Hann			OpenViBE::CIdentifier(0x9BBBDC65, 0xFDFEF7A4)
#define OVP_TypeId_WindowMethod_Blackman		OpenViBE::CIdentifier(0x9D5937A4, 0xE43A9E3D)
#define OVP_TypeId_WindowMethod_Triangular		OpenViBE::CIdentifier(0xE652A852, 0xFE3CBC46)
#define OVP_TypeId_WindowMethod_SquareRoot		OpenViBE::CIdentifier(0xFE80BE5C, 0xAE59A27A)
