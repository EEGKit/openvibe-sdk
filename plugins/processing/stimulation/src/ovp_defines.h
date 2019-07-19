#pragma once

//___________________________________________________________________//
//                                                                   //
//  Type Definitions                                                 //
//___________________________________________________________________//
//                                                                   //

#define OVP_TypeId_Voting_ClearVotes                                   OpenViBE::CIdentifier(0x17AE30F8, 0x40B57661)
#define OVP_TypeId_Voting_ClearVotes_AfterOutput                       OpenViBE::CIdentifier(0x7FA81A20, 0x484023F9)
#define OVP_TypeId_Voting_ClearVotes_WhenExpires                       OpenViBE::CIdentifier(0x02766639, 0x00B155B4)
#define OVP_TypeId_Voting_OutputTime                                   OpenViBE::CIdentifier(0x48583E8F, 0x47F22462)
#define OVP_TypeId_Voting_OutputTime_Vote                              OpenViBE::CIdentifier(0x2F37507F, 0x00C06761)
#define OVP_TypeId_Voting_OutputTime_Winner                            OpenViBE::CIdentifier(0x72416689, 0x17673658)
#define OVP_TypeId_Voting_OutputTime_Last                              OpenViBE::CIdentifier(0x4F2830DB, 0x716C2930)
#define OVP_TypeId_Voting_RejectClass_CanWin                                 OpenViBE::CIdentifier(0x442F2F14, 0x7A17336C)
#define OVP_TypeId_Voting_RejectClass_CanWin_Yes                             OpenViBE::CIdentifier(0x40011974, 0x54BB3C71)
#define OVP_TypeId_Voting_RejectClass_CanWin_No                              OpenViBE::CIdentifier(0x275B746A, 0x480B302C)

#define OV_TypeId_PlayerAction                             OpenViBE::CIdentifier(0xCC14D8D6, 0xF27ECB73)
#define OV_TypeId_PlayerAction_Play                        OpenViBE::CIdentifier(0x6BD7E1B3, 0x85E75066)
#define OV_TypeId_PlayerAction_Stop                        OpenViBE::CIdentifier(0x8A72885E, 0xD537A9A4)
#define OV_TypeId_PlayerAction_Pause                       OpenViBE::CIdentifier(0xD2FA4573, 0xE7F1D11D)
#define OV_TypeId_PlayerAction_Forward                     OpenViBE::CIdentifier(0xE43E9888, 0x33A361ED)

//___________________________________________________________________//
//                                                                   //
// Global defines                                                   //
//___________________________________________________________________//
//                                                                   //

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
#include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines



