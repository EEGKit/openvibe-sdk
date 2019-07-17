#ifndef __Socket_Defines_H__
#define __Socket_Defines_H__

#include <ov_common_defines.h>

#define OV_APPEND_TO_NAMESPACE Socket
#include <ov_common_types.h>
#undef OV_APPEND_TO_NAMESPACE

// Taken from
// - http://people.redhat.com/drepper/dsohowto.pdf
// - http://www.nedprod.com/programs/gccvisibility.html
#if defined Socket_Shared
#if defined TARGET_OS_Windows
#define	Socket_API_Export __declspec(dllexport)
#define	Socket_API_Import __declspec(dllimport)
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		#define Socket_API_Export __attribute__((visibility("default")))
		#define Socket_API_Import __attribute__((visibility("default")))
#else
		#define Socket_API_Export
		#define Socket_API_Import
#endif
#else
#define Socket_API_Export
#define Socket_API_Import
#endif

#if	defined Socket_Exports
#define Socket_API Socket_API_Export
#else
	#define Socket_API Socket_API_Import
#endif


#endif // __Socket_Defines_H__
