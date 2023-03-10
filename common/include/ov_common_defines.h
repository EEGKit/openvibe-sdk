#pragma once

#include <cstdlib>	// For Unix Compatibility (size_t)
#include <cstdint>  // For Uint standard

//
// This file checks the presence of several defines that are supposed to
// be set by the build system. In addition, it defines some additional ones.
//

#if defined(_MSC_VER)
// We need some magic to get #warning behavior on MSVC
#define STRINGISE(N) #N
#define EXPAND_THEN_STRINGISE(N) STRINGISE(N)
#define __LINE_STR__ EXPAND_THEN_STRINGISE(__LINE__)

// Format #pragma message
#define __LOC__ __FILE__ "(" __LINE_STR__ ")"
#define __OUTPUT_FORMAT__(type) __LOC__ " : " type " : "

// Actual warning message type
#define __WARNING__ __OUTPUT_FORMAT__("Warning")
#endif


//___________________________________________________________________//
//                                                                   //
// Operating System identification                                   //
//___________________________________________________________________//
//                                                                   //

#if !(defined(TARGET_OS_Windows) || defined(TARGET_OS_Linux) || defined(TARGET_OS_MacOS))
#if defined(_MSC_VER)
 #pragma message( __WARNING__ "No known target OS specified!")
#else
 #warning "No known target OS specified!"
#endif
#endif

//___________________________________________________________________//
//                                                                   //
// Build type identification                                         //
//___________________________________________________________________//
//                                                                   //

#if !(defined(TARGET_BUILDTYPE_Debug) || defined(TARGET_BUILDTYPE_Release))
#if defined(_MSC_VER)
 #pragma message( __WARNING__ "No known build type defined!")
#else
 #warning "No known build type defined!"
#endif
#endif

//___________________________________________________________________//
//                                                                   //
// Compiler software identification                                  //
//___________________________________________________________________//
//                                                                   //

#if !(defined(TARGET_COMPILER_GCC) || defined(TARGET_COMPILER_VisualStudio) || defined(TARGET_COMPILER_LLVM))
#if defined(_MSC_VER)
 #pragma message( __WARNING__ "No known compiler defined!")
#else
 #warning "No known compiler defined!"
#endif
#endif

//___________________________________________________________________//
//                                                                   //
// API Definition                                                    //
//___________________________________________________________________//
//                                                                   //

// Taken from
// - http://people.redhat.com/drepper/dsohowto.pdf
// - http://www.nedprod.com/programs/gccvisibility.html
#if defined OV_Shared
#if defined TARGET_OS_Windows
#define OV_API_Export __declspec(dllexport)
#define OV_API_Import __declspec(dllimport)
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
  #define OV_API_Export __attribute__((visibility("default")))
  #define OV_API_Import __attribute__((visibility("default")))
#else
  #define OV_API_Export
  #define OV_API_Import
#endif
#else
#define OV_API_Export
#define OV_API_Import
#endif

#if defined OV_Exports
 #define OV_API OV_API_Export
#else
#define OV_API OV_API_Import
#endif


//[[deprecated(message)]] after c++14 for all OS
#if defined TARGET_OS_Windows
#define OV_Deprecated(message) __declspec(deprecated(message))
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#define OV_Deprecated(message) __attribute__((deprecated))
#else
#define OV_Deprecated(message)
#endif


//___________________________________________________________________//
//                                                                   //
// API Definition for plugins                                        //
//___________________________________________________________________//
//                                                                   //


// Taken from
// - http://people.redhat.com/drepper/dsohowto.pdf
// - http://www.nedprod.com/programs/gccvisibility.html
#if defined OVP_Shared
#if defined TARGET_OS_Windows
#define OVP_API_Export __declspec(dllexport)
#define OVP_API_Import __declspec(dllimport)
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		#define OVP_API_Export __attribute__((visibility("default")))
		#define OPV_API_Import __attribute__((visibility("default")))
#else
		#define OVP_API_Export
		#define OVP_API_Import
#endif
#else
#	define OVP_API_Export
#	define OVP_API_Import
#endif

#if defined OVP_Exports
#	define OVP_API OVP_API_Export
#else
#	define OVP_API OVP_API_Import
#endif
//___________________________________________________________________//
//                                                                   //
// NULL Definition                                                   //
//___________________________________________________________________//
//                                                                   //

#ifndef NULL
#define NULL 0
#endif

#if defined(__cplusplus) && (__cplusplus >= 201402L)
  #define OV_DEPRECATED(since) [[deprecated("Since " #since)]]
  #define OV_DEPRECATED_FOR(since, replacement) [[deprecated("Since " #since "; use " #replacement)]]
#elif defined(_MSC_VER)
#define OV_DEPRECATED(since) __declspec(deprecated("Since " # since))
#define OV_DEPRECATED_FOR(since, replacement) __declspec(deprecated("Since " #since "; use " #replacement))
#else
  #define OV_DEPRECATED(since) __attribute__((__deprecated__))
  #define OV_DEPRECATED_FOR(since, replacement) __attribute__((__deprecated__))
#endif
