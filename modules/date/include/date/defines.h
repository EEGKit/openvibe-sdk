#pragma once

#include <ov_common_defines.h>

#define OV_APPEND_TO_NAMESPACE IDateParser
#include <ov_common_types.h>
#undef OV_APPEND_TO_NAMESPACE

#if defined Date_Shared
#if defined TARGET_OS_Windows
#define Date_API_Export __declspec(dllexport)
#define Date_API_Import __declspec(dllimport)
#elif defined TARGET_OS_Linux
#define Date_API_Export __attribute__((visibility("default")))
#define Date_API_Import __attribute__((visibility("default")))
#else
#define Date_API_Export
#define Date_API_Import
#endif
#else
#define Date_API_Export
#define Date_API_Import
#endif

#if defined Date_Exports
#define Date_API Date_API_Export
#else
#define Date_API Date_API_Import
#endif
