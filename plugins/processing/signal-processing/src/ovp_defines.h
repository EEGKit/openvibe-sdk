#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

//___________________________________________________________________//
//                                                                   //
//                                                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_TimeBasedEpoching                                                  OpenViBE::CIdentifier(0x00777FA0, 0x5DC3F560)
#define OVP_ClassId_TimeBasedEpochingDesc                                              OpenViBE::CIdentifier(0x00ABDABE, 0x41381683)

#define OVP_ClassId_Algorithm_ApplySpatialFilter                                       OpenViBE::CIdentifier(0xC5DC28FF, 0x2394AFBC)
#define OVP_ClassId_Algorithm_ApplySpatialFilterDesc                                   OpenViBE::CIdentifier(0x0601FABE, 0x85692BFD)
#define OVP_Algorithm_ApplySpatialFilter_InputParameterId_SignalMatrix                 OpenViBE::CIdentifier(0x6FF50741, 0xB9757B1F)
#define OVP_Algorithm_ApplySpatialFilter_InputParameterId_FilterCoefficientsMatrix     OpenViBE::CIdentifier(0x43DC06E5, 0x48E413BA)
#define OVP_Algorithm_ApplySpatialFilter_OutputParameterId_FilteredSignalMatrix        OpenViBE::CIdentifier(0x695B69B2, 0x02DBE696)
#define OVP_Algorithm_ApplySpatialFilter_InputTriggerId_Initialize                     OpenViBE::CIdentifier(0x06D0E2D6, 0xE1E9C082)
#define OVP_Algorithm_ApplySpatialFilter_InputTriggerId_ApplyFilter                    OpenViBE::CIdentifier(0x316BBE5D, 0xBEE0F747)

#define OVP_TypeId_EpochAverageMethod                                                  OpenViBE::CIdentifier(0x6530BDB1, 0xD057BBFE)
#define OVP_TypeId_EpochAverageMethod_MovingAverage                                    OpenViBE::CIdentifier(0x426377E7, 0xCF8E32CD)
#define OVP_TypeId_EpochAverageMethod_MovingAverageImmediate                           OpenViBE::CIdentifier(0x1F830F88, 0xAA01A592)
#define OVP_TypeId_EpochAverageMethod_BlockAverage                                     OpenViBE::CIdentifier(0x2E4ACA75, 0x7E02B507)
#define OVP_TypeId_EpochAverageMethod_CumulativeAverage                                OpenViBE::CIdentifier(0xC18311B7, 0x001C1953)

#define OVP_ClassId_BoxAlgorithm_EpochAverage                                          OpenViBE::CIdentifier(0x21283D9F, 0xE76FF640)
#define OVP_ClassId_BoxAlgorithm_EpochAverageDesc                                      OpenViBE::CIdentifier(0x95F5F43E, 0xBE629D82)
#define OVP_ClassId_Algorithm_MatrixAverage                                            OpenViBE::CIdentifier(0x5E5A6C1C, 0x6F6BEB03)
#define OVP_ClassId_Algorithm_MatrixAverageDesc                                        OpenViBE::CIdentifier(0x1992881F, 0xC938C0F2)
#define OVP_Algorithm_MatrixAverage_InputParameterId_Matrix                            OpenViBE::CIdentifier(0x913E9C3B, 0x8A62F5E3)
#define OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount                       OpenViBE::CIdentifier(0x08563191, 0xE78BB265)
#define OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod                   OpenViBE::CIdentifier(0xE63CD759, 0xB6ECF6B7)
#define OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix                   OpenViBE::CIdentifier(0x03CE5AE5, 0xBD9031E0)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_Reset                               OpenViBE::CIdentifier(0x670EC053, 0xADFE3F5C)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix                          OpenViBE::CIdentifier(0x50B6EE87, 0xDC42E660)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_ForceAverage                        OpenViBE::CIdentifier(0xBF597839, 0xCD6039F0)
#define OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed                   OpenViBE::CIdentifier(0x2BFF029B, 0xD932A613)

#define OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching                              OpenViBE::CIdentifier(0x426163D1, 0x324237B0)
#define OVP_ClassId_BoxAlgorithm_StimulationBasedEpochingDesc                          OpenViBE::CIdentifier(0x4F60616D, 0x468E0A8C)
#define OVP_ClassId_Algorithm_StimulationBasedEpoching                                 OpenViBE::CIdentifier(0x421E3F46, 0x12003E6C)
#define OVP_ClassId_Algorithm_StimulationBasedEpochingDesc                             OpenViBE::CIdentifier(0x2EAF37FC, 0x63195EB8)
#define OVP_Algorithm_StimulationBasedEpoching_InputParameterId_InputSignal            OpenViBE::CIdentifier(0x0ED5C92B, 0xE16BEF25)
#define OVP_Algorithm_StimulationBasedEpoching_InputParameterId_OffsetSampleCount      OpenViBE::CIdentifier(0x7646CE65, 0xE128FC4E)
#define OVP_Algorithm_StimulationBasedEpoching_OutputParameterId_OutputSignal          OpenViBE::CIdentifier(0x00D331A2, 0xC13DF043)
#define OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_Reset                    OpenViBE::CIdentifier(0x6BA44128, 0x418CF901)
#define OVP_Algorithm_StimulationBasedEpoching_InputTriggerId_PerformEpoching          OpenViBE::CIdentifier(0xD05579B5, 0x2649A4B2)
#define OVP_Algorithm_StimulationBasedEpoching_OutputTriggerId_EpochingDone            OpenViBE::CIdentifier(0x755BC3FE, 0x24F7B50F)
#define OVP_Algorithm_StimulationBasedEpoching_InputParameterId_EndTimeChunkToProcess  OpenViBE::CIdentifier(0x8B552604, 0x10CD1F94)

#define OVP_TypeId_CropMethod                                                          OpenViBE::CIdentifier(0xD0643F9E, 0x8E35FE0A)
#define OVP_TypeId_CropMethod_Min                                                      OpenViBE::CIdentifier(0x0CCC9DE4, 0x93F495D2)
#define OVP_TypeId_CropMethod_Max                                                      OpenViBE::CIdentifier(0x2FFEB29C, 0xD8F21FB4)
#define OVP_TypeId_CropMethod_MinMax                                                   OpenViBE::CIdentifier(0x3CEA8129, 0xA772413A)
#define OVP_ClassId_BoxAlgorithm_Crop                                                  OpenViBE::CIdentifier(0x7F1A3002, 0x358117BA)
#define OVP_ClassId_BoxAlgorithm_CropDesc                                              OpenViBE::CIdentifier(0x64D619D7, 0x26CC42C9)

#define OVP_TypeId_SelectionMethod                         OpenViBE::CIdentifier(0x3BCF9E67, 0x0C23994D)
#define OVP_TypeId_SelectionMethod_Select                  OpenViBE::CIdentifier(0x1970FAAF, 0x4FD1CC4D)
#define OVP_TypeId_SelectionMethod_Reject                  OpenViBE::CIdentifier(0x4C05408D, 0x6EAC6F85)

#define OVP_TypeId_MatchMethod                             OpenViBE::CIdentifier(0x666F25E9, 0x3E5738D6)
#define OVP_TypeId_MatchMethod_Name                        OpenViBE::CIdentifier(0x58032A35, 0x4393A5D2)
#define OVP_TypeId_MatchMethod_Index                       OpenViBE::CIdentifier(0x0E0BF9E8, 0x3D612618)
#define OVP_TypeId_MatchMethod_Smart                       OpenViBE::CIdentifier(0x2D87EF07, 0xA2828AC0)

#define OVP_ClassId_BoxAlgorithm_DifferentialIntegral         OpenViBE::CIdentifier(0xCE490CBF, 0xDF7BA2E2)
#define OVP_ClassId_BoxAlgorithm_DifferentialIntegralDesc     OpenViBE::CIdentifier(0xCE490CBF, 0xDF7BA2E2)
#define OVP_TypeId_DifferentialIntegralOperation              OpenViBE::CIdentifier(0x6E6AD85D, 0x14FD203A)
#define OVP_TypeId_DifferentialIntegralOperation_Differential OpenViBE::CIdentifier(0x8EEF4E01, 0x1F9823C2)
#define OVP_TypeId_DifferentialIntegralOperation_Integral     OpenViBE::CIdentifier(0x44269C45, 0x77988564)

#define OVP_ClassId_Algorithm_ARBurgMethod                        			OpenViBE::CIdentifier(0x3EC6A165, 0x2823A034)
#define OVP_ClassId_Algorithm_ARBurgMethodDesc                    			OpenViBE::CIdentifier(0xD7234DFF, 0x55447A14)
#define OVP_Algorithm_ARBurgMethod_InputParameterId_Matrix       			OpenViBE::CIdentifier(0x36A69669, 0x3651271D)
#define OVP_Algorithm_ARBurgMethod_OutputParameterId_Matrix      			OpenViBE::CIdentifier(0x55EF8C81, 0x178A51B2)
#define OVP_Algorithm_ARBurgMethod_InputParameterId_UInteger     			OpenViBE::CIdentifier(0x33139BC1, 0x03D30D3B)
#define OVP_Algorithm_ARBurgMethod_InputTriggerId_Initialize 	 			OpenViBE::CIdentifier(0xC27B06C6, 0xB8EB5F8D)
#define OVP_Algorithm_ARBurgMethod_InputTriggerId_Process		     		OpenViBE::CIdentifier(0xBEEBBE84, 0x4F14F8F8)
#define OVP_Algorithm_ARBurgMethod_OutputTriggerId_ProcessDone           	OpenViBE::CIdentifier(0xA5AAD435, 0x9EC3DB80) 

#define OVP_ClassId_BoxAlgorithm_ConnectivityMeasure 					OpenViBE::CIdentifier(0x8E3A1AEF, 0x7CACD368)
#define OVP_ClassId_BoxAlgorithm_ConnectivityMeasureDesc 				OpenViBE::CIdentifier(0xA20B0A40, 0x1A92D645)
#define OVP_TypeId_Algorithm_SingleTrialPhaseLockingValue				OpenViBE::CIdentifier(0x740E1284, 0x24107052)
#define OVP_TypeId_Algorithm_SingleTrialPhaseLockingValueDesc			OpenViBE::CIdentifier(0x71CA3FD4, 0x0ED22534)

#define OVP_ClassId_Algorithm_HilbertTransform								OpenViBE::CIdentifier(0x344B79DE, 0x89EAAABB)
#define OVP_ClassId_Algorithm_HilbertTransformDesc							OpenViBE::CIdentifier(0x8CAB236A, 0xA789800D)
#define OVP_Algorithm_HilbertTransform_InputParameterId_Matrix				OpenViBE::CIdentifier(0xC117CE9A, 0x3FFCB156)
#define OVP_Algorithm_HilbertTransform_OutputParameterId_HilbertMatrix		OpenViBE::CIdentifier(0xDAE13CB8, 0xEFF82E69)
#define OVP_Algorithm_HilbertTransform_OutputParameterId_EnvelopeMatrix		OpenViBE::CIdentifier(0x9D0A023A, 0x7690C48E)
#define OVP_Algorithm_HilbertTransform_OutputParameterId_PhaseMatrix		OpenViBE::CIdentifier(0x495B55E2, 0x8CAAC08E)
#define OVP_Algorithm_HilbertTransform_InputTriggerId_Initialize			OpenViBE::CIdentifier(0xE4B3CB4A, 0xF0121A20)
#define OVP_Algorithm_HilbertTransform_InputTriggerId_Process				OpenViBE::CIdentifier(0xC3DC087D, 0x4AAFC1F0)
#define OVP_Algorithm_HilbertTransform_OutputTriggerId_ProcessDone			OpenViBE::CIdentifier(0xB0B2A2DD, 0x73529B46)

#define OVP_ClassId_BoxAlgorithm_Hilbert				OpenViBE::CIdentifier(0x7878A47F, 0x9A8FE349)
#define OVP_ClassId_BoxAlgorithm_HilbertDesc			OpenViBE::CIdentifier(0x2DB54E2F, 0x435675EF)


//___________________________________________________________________//
//                                                                   //
// Plugin Object Descriptor Class Identifiers                        //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_ReferenceChannelDesc                                               OpenViBE::CIdentifier(0x1873B151, 0x969DD4E4)
#define OVP_ClassId_ChannelSelectorDesc                                                OpenViBE::CIdentifier(0x34893489, 0x44934897)
#define OVP_ClassId_SimpleDSPDesc                                                      OpenViBE::CIdentifier(0x00C44BFE, 0x76C9269E)
#define OVP_ClassId_SignalAverageDesc                                                  OpenViBE::CIdentifier(0x007CDCE9, 0x16034F77)
#define OVP_ClassId_SignalConcatenationDesc                                            OpenViBE::CIdentifier(0x3921BACD, 0x1E9546FE)
#define OVP_ClassId_FirstDifferenceDetrendingDesc                                      OpenViBE::CIdentifier(0x0039EF62, 0x11DA5967)
#define OVP_ClassId_SecondDifferenceDetrendingDesc                                     OpenViBE::CIdentifier(0x000B30B6, 0x29E85D42)
#define OVP_ClassId_OverlappingEpochAverageDesc                                        OpenViBE::CIdentifier(0x000C8040, 0x7A4F177E)
#define OVP_ClassId_BoxAlgorithm_QuadraticFormDesc                                     OpenViBE::CIdentifier(0x31C11856, 0x3E4F7B67)
//___________________________________________________________________//
//                                                                   //
// Plugin Object Class Identifiers                                   //
//___________________________________________________________________//
//                                                                   //

#define OVP_ClassId_ReferenceChannel                                                   OpenViBE::CIdentifier(0xEFA8E95B, 0x4F22551B)
#define OVP_ClassId_ChannelSelector                                                    OpenViBE::CIdentifier(0x39484563, 0x46386889)
#define OVP_ClassId_SimpleDSP                                                          OpenViBE::CIdentifier(0x00E26FA1, 0x1DBAB1B2)
#define OVP_ClassId_SignalAverage                                                      OpenViBE::CIdentifier(0x00642C4D, 0x5DF7E50A)
#define OVP_ClassId_SignalConcatenation                                                OpenViBE::CIdentifier(0x6568D29B, 0x0D753CCA)
#define OVP_ClassId_FirstDifferenceDetrending                                          OpenViBE::CIdentifier(0x001B4E3D, 0x7FEE9CB1)
#define OVP_ClassId_SecondDifferenceDetrending                                         OpenViBE::CIdentifier(0x00D283A5, 0x40D76DD3)
#define OVP_ClassId_OverlappingEpochAverage                                            OpenViBE::CIdentifier(0x0042232E, 0x6F5B5E21)
#define OVP_ClassId_BoxAlgorithm_QuadraticForm                                         OpenViBE::CIdentifier(0x54E73B81, 0x1AD356C6)

//Dieter p300 stuff
#define OVP_ClassId_BoxAlgorithm_TwoSampleTTestDesc		OpenViBE::CIdentifier(0x020300BF, 0xB4BAC0A1)
#define OVP_ClassId_BoxAlgorithm_LikelinessDistributorDesc      OpenViBE::CIdentifier(0xE5103C63, 0x08D825E0)
#define OVP_ClassId_BoxAlgorithm_EpochVarianceDesc              OpenViBE::CIdentifier(0xA15EAEC5, 0xAB0CE73D)
#define OVP_ClassId_Algorithm_MatrixVarianceDesc 		OpenViBE::CIdentifier(0xE405260B, 0x59EEFAE4)

#define OVP_ClassId_BoxAlgorithm_TwoSampleTTest			OpenViBE::CIdentifier(0x16DFF805, 0x0BC431BF)
#define OVP_ClassId_BoxAlgorithm_LikelinessDistributor		OpenViBE::CIdentifier(0x781F51CA, 0xE6E3B0B8)
#define OVP_ClassId_BoxAlgorithm_EpochVariance                  OpenViBE::CIdentifier(0x335384EA, 0x88C917D9)
#define OVP_ClassId_Algorithm_MatrixVariance 			OpenViBE::CIdentifier(0x7FEFDCA9, 0x816ED903)

#define OVP_Algorithm_MatrixVariance_InputParameterId_Matrix			OpenViBE::CIdentifier(0x781F51CA, 0xE6E3B0B8)
#define OVP_Algorithm_MatrixVariance_InputParameterId_MatrixCount		OpenViBE::CIdentifier(0xE5103C63, 0x08D825E0)
#define OVP_Algorithm_MatrixVariance_InputParameterId_AveragingMethod		OpenViBE::CIdentifier(0x043A1BC4, 0x925D3CD6)
#define OVP_Algorithm_MatrixVariance_InputParameterId_SignificanceLevel		OpenViBE::CIdentifier(0x1E1065B2, 0x2CA32013)
#define OVP_Algorithm_MatrixVariance_OutputParameterId_AveragedMatrix		OpenViBE::CIdentifier(0x5CF66A73, 0xF5BBF0BF)
#define OVP_Algorithm_MatrixVariance_OutputParameterId_Variance			OpenViBE::CIdentifier(0x1BD67420, 0x587600E6)
#define OVP_Algorithm_MatrixVariance_OutputParameterId_ConfidenceBound		OpenViBE::CIdentifier(0x1E1065B2, 0x2CA32013)
#define OVP_Algorithm_MatrixVariance_InputTriggerId_Reset			OpenViBE::CIdentifier(0xD5C5EF91, 0xE1B1C4F4)
#define OVP_Algorithm_MatrixVariance_InputTriggerId_FeedMatrix			OpenViBE::CIdentifier(0xEBAEB213, 0xDD4735A0)
#define OVP_Algorithm_MatrixVariance_InputTriggerId_ForceAverage		OpenViBE::CIdentifier(0x344A52F5, 0x489DB439)
#define OVP_Algorithm_MatrixVariance_OutputTriggerId_AveragePerformed		OpenViBE::CIdentifier(0x2F9ECA0B, 0x8D3CA7BD)

#define OVP_ClassId_BoxAlgorithm_XDAWNSpatialFilterTrainerCoAdapt     OpenViBE::CIdentifier(0xEE31A115, 0x00B25FE1)
#define OVP_ClassId_BoxAlgorithm_XDAWNSpatialFilterTrainerCoAdaptDesc OpenViBE::CIdentifier(0xB59256B0, 0xC3389515)
#define OVP_ClassId_BoxAlgorithm_SpatialFilterWithUpdate     OpenViBE::CIdentifier(0x8E7CD4B3, 0xDAAD41C1)
#define OVP_ClassId_BoxAlgorithm_SpatialFilterWithUpdateDesc OpenViBE::CIdentifier(0xE2E6A1D6, 0xCBCEEE3A)

#define OVP_ClassId_ConditionalIdentity OpenViBE::CIdentifier(0xE4CE5D77, 0xBCC64C3F)
#define OVP_ClassId_ConditionalIdentityDesc OpenViBE::CIdentifier(0xDDE86CE9, 0xE75B0A29)

#define OVP_ClassId_BoxAlgorithm_MultipleSpatialFilters	 	OpenViBE::CIdentifier(0x04C1466C, 0xFC89B739)
#define OVP_ClassId_BoxAlgorithm_MultipleSpatialFiltersDesc	OpenViBE::CIdentifier(0xB1A96C51, 0xD75136A6)
//*/End of Dieter's stuff



//___________________________________________________________________//
//                                                                   //
// Global defines                                                   //
//___________________________________________________________________//
//                                                                   //

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
 #include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines


#define OVP_Value_CoupledStringSeparator                                '-'
//#define OVP_Value_AllSelection											'*'

#endif // __OpenViBEPlugins_Defines_H__
