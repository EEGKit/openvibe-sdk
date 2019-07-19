#pragma once

//___________________________________________________________________//
//                                                                   //
// Type Definitions                                                  //
//___________________________________________________________________//

// Filter method identifiers from OpenViBE 0.14.0
#define OVP_TypeId_FilterMethod															OpenViBE::CIdentifier(0x2F2C606C, 0x8512ED68)
#define OVP_TypeId_FilterMethod_Butterworth												OpenViBE::CIdentifier(0x00E4EADE, 0x9586A50E)
#define OVP_TypeId_FilterMethod_Chebyshev												OpenViBE::CIdentifier(0x0C3F5301, 0x7397A7FB)
#define OVP_TypeId_FilterMethod_YuleWalker												OpenViBE::CIdentifier(0x7BF59C86, 0xB6D459BA)

// Filter type identifiers from OpenViBE 0.14.0
#define OVP_TypeId_FilterType															OpenViBE::CIdentifier(0xFA20178E, 0x4CBA62E9)
#define OVP_TypeId_FilterType_LowPass													OpenViBE::CIdentifier(0x0C33A568, 0xE2BD46D2)
#define OVP_TypeId_FilterType_BandPass													OpenViBE::CIdentifier(0x19E89DDB, 0xD1FF7FF5)
#define OVP_TypeId_FilterType_HighPass													OpenViBE::CIdentifier(0x2F677195, 0x9F9F15B9)
#define OVP_TypeId_FilterType_BandStop													OpenViBE::CIdentifier(0x41C34DD7, 0xF70E4043)

#define OVP_TypeId_OnlineCovariance_UpdateMethod										OpenViBE::CIdentifier(0x59E83F33, 0x592F1DD0)
#define OVP_TypeId_OnlineCovariance_UpdateMethod_ChunkAverage							OpenViBE::CIdentifier(0x079E14D3, 0x784A2BD1)
#define OVP_TypeId_OnlineCovariance_UpdateMethod_Incremental		    				OpenViBE::CIdentifier(0x39E20E6D, 0x6A87073C)

#define OVP_TypeId_EpochAverageMethod													OpenViBE::CIdentifier(0x6530BDB1, 0xD057BBFE)
#define OVP_TypeId_EpochAverageMethod_MovingAverage										OpenViBE::CIdentifier(0x426377E7, 0xCF8E32CD)
#define OVP_TypeId_EpochAverageMethod_MovingAverageImmediate							OpenViBE::CIdentifier(0x1F830F88, 0xAA01A592)
#define OVP_TypeId_EpochAverageMethod_BlockAverage										OpenViBE::CIdentifier(0x2E4ACA75, 0x7E02B507)
#define OVP_TypeId_EpochAverageMethod_CumulativeAverage									OpenViBE::CIdentifier(0xC18311B7, 0x001C1953)

// Wavelet type
#define OVP_TypeId_ContinuousWaveletType												OpenViBE::CIdentifier(0x09177469, 0x52404583)
#define OVP_TypeId_ContinuousWaveletType_Morlet											OpenViBE::CIdentifier(0x40D21D50, 0x738A68ED)
#define OVP_TypeId_ContinuousWaveletType_Paul											OpenViBE::CIdentifier(0x2F007EC1, 0x2B870B4A)
#define OVP_TypeId_ContinuousWaveletType_DOG											OpenViBE::CIdentifier(0x4B2E79A5, 0x40C95CF5)

#define OVP_TypeId_CropMethod															OpenViBE::CIdentifier(0xD0643F9E, 0x8E35FE0A)
#define OVP_TypeId_CropMethod_Min														OpenViBE::CIdentifier(0x0CCC9DE4, 0x93F495D2)
#define OVP_TypeId_CropMethod_Max														OpenViBE::CIdentifier(0x2FFEB29C, 0xD8F21FB4)
#define OVP_TypeId_CropMethod_MinMax													OpenViBE::CIdentifier(0x3CEA8129, 0xA772413A)

#define OVP_TypeId_SelectionMethod														OpenViBE::CIdentifier(0x3BCF9E67, 0x0C23994D)
#define OVP_TypeId_SelectionMethod_Select												OpenViBE::CIdentifier(0x1970FAAF, 0x4FD1CC4D)
#define OVP_TypeId_SelectionMethod_Reject												OpenViBE::CIdentifier(0x4C05408D, 0x6EAC6F85)
#define OVP_TypeId_SelectionMethod_Select_EEG											OpenViBE::CIdentifier(0x600E3C4F, 0x6DD00C2D)

#define OVP_TypeId_MatchMethod															OpenViBE::CIdentifier(0x666F25E9, 0x3E5738D6)
#define OVP_TypeId_MatchMethod_Name														OpenViBE::CIdentifier(0x58032A35, 0x4393A5D2)
#define OVP_TypeId_MatchMethod_Index													OpenViBE::CIdentifier(0x0E0BF9E8, 0x3D612618)
#define OVP_TypeId_MatchMethod_Smart													OpenViBE::CIdentifier(0x2D87EF07, 0xA2828AC0)

//___________________________________________________________________//
//                                                                   //
// Global defines                                                   //
//___________________________________________________________________//
//                                                                   //

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
#include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines


#define OVP_Value_CoupledStringSeparator									'-'
//#define OVP_Value_AllSelection											'*'


