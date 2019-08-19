#include "system/WindowsUtilities.h"
#if defined TARGET_OS_Windows
#include "m_ConverterUtf8.h"
#include <ShellAPI.h>

#ifndef UNICODE
#define UNICODE
#endif

using namespace System;

// Load a library in a matter compliant with non-ascii path
// returns the eventual error code
void* WindowsUtilities::utf16CompliantLoadLibrary(const char* sLibraryPath, HANDLE hFile, DWORD dwFlags)
{
	//const HMODULE hModule = ::LoadLibraryEx(sLibraryPath, hFile, dwFlags); // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_DEFAULT_DIRS);
	return ::LoadLibraryEx(sLibraryPath, hFile, dwFlags); // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_DEFAULT_DIRS);
}

BOOL WindowsUtilities::utf16CompliantSetEnvironmentVariable(const char* sEnvVarName, const char* sEnvVarValue)
{
	/*std::wstring l_sUtf16EnvVarName = castToWString(sEnvVarName);
	std::wstring l_sUtf16EnvVarValue = castToWString(sEnvVarValue);*/
	return SetEnvironmentVariable(sEnvVarName, sEnvVarValue);
}
		
// Load a library in a matter compliant with non-ascii path
// returns the eventual error code
BOOL WindowsUtilities::utf16CompliantCreateProcess(char* sApplicationName, char* sCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
												   LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, char* sCurrentDirectory,
												   LPSTARTUPINFO l_pStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
	return CreateProcess(sApplicationName, const_cast<char*>(sCommandLine), lpProcessAttributes, lpThreadAttributes,
						 bInheritHandles, dwCreationFlags, lpEnvironment, sCurrentDirectory, l_pStartupInfo, lpProcessInformation);
}

// Load a library in a matter compliant with non-ascii path
// returns the eventual error code
HINSTANCE WindowsUtilities::utf16CompliantShellExecute(HWND l_pHWND, LPCTSTR lpOperation, LPCTSTR lpFile,
													   LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd)
{
	return ShellExecute(l_pHWND, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd);
}

#endif // TARGET_OS_Windows
