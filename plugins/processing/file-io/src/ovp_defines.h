#ifndef __OpenViBEPlugins_Defines_H__
#define __OpenViBEPlugins_Defines_H__

//___________________________________________________________________//
//                                                                   //
// Type definitions				                                     //
//___________________________________________________________________//
//                                                                   //

#define OVP_NodeId_OpenViBEStream_Header              EBML::CIdentifier(0xF59505AB, 0x3684C8D8)
#define OVP_NodeId_OpenViBEStream_Header_Compression  EBML::CIdentifier(0x40358769, 0x166380D1)
#define OVP_NodeId_OpenViBEStream_Header_StreamType   EBML::CIdentifier(0x732EC1D1, 0xFE904087)
#define OVP_NodeId_OpenViBEStream_Header_ChannelType  OVP_NodeId_OpenViBEStream_Header_StreamType  // deprecated old name
#define OVP_NodeId_OpenViBEStream_Buffer              EBML::CIdentifier(0x2E60AD18, 0x87A29BDF)
#define OVP_NodeId_OpenViBEStream_Buffer_StreamIndex  EBML::CIdentifier(0x30A56D8A, 0xB9C12238)
#define OVP_NodeId_OpenViBEStream_Buffer_ChannelIndex OVP_NodeId_OpenViBEStream_Buffer_StreamIndex // deprecated old name
#define OVP_NodeId_OpenViBEStream_Buffer_StartTime    EBML::CIdentifier(0x093E6A0A, 0xC5A9467B)
#define OVP_NodeId_OpenViBEStream_Buffer_EndTime      EBML::CIdentifier(0x8B5CCCD9, 0xC5024F29)
#define OVP_NodeId_OpenViBEStream_Buffer_Content      EBML::CIdentifier(0x8D4B0BE8, 0x7051265C)

//___________________________________________________________________//
//                                                                   //
// Global defines                                                   //
//___________________________________________________________________//
//                                                                   //

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
#include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#endif // __OpenViBEPlugins_Defines_H__
