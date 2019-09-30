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
void* WindowsUtilities::utf16CompliantLoadLibrary(const char* path, HANDLE file, DWORD flags)
{
	//const HMODULE hModule = ::LoadLibraryEx(path, file, flags); // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_DEFAULT_DIRS);
	return ::LoadLibraryEx(path, file, flags); // LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_DEFAULT_DIRS);
}

BOOL WindowsUtilities::utf16CompliantSetEnvironmentVariable(const char* name, const char* value)
{
	/*std::wstring l_sUtf16EnvVarName = castToWString(name);
	std::wstring l_sUtf16EnvVarValue = castToWString(value);*/
	return SetEnvironmentVariable(name, value);
}
		
// Load a library in a matter compliant with non-ascii path
// returns the eventual error code
BOOL WindowsUtilities::utf16CompliantCreateProcess(char* applicationName, char* commandLine, LPSECURITY_ATTRIBUTES processAttributes,
												   LPSECURITY_ATTRIBUTES threadAttributes, BOOL inheritHandles, DWORD creationFlags, LPVOID environment,
												   char* currentDirectory, LPSTARTUPINFO startupInfo, LPPROCESS_INFORMATION processInformation)
{
	return CreateProcess(applicationName, const_cast<char*>(commandLine), processAttributes, threadAttributes,
						 inheritHandles, creationFlags, environment, currentDirectory, startupInfo, processInformation);
}

// Load a library in a matter compliant with non-ascii path
// returns the eventual error code
HINSTANCE WindowsUtilities::utf16CompliantShellExecute(HWND hwnd, LPCTSTR operation, LPCTSTR file, LPCTSTR parameters, LPCTSTR directory, INT nShowCmd)
{
	return ShellExecute(hwnd, operation, file, parameters, directory, nShowCmd);
}

#endif // TARGET_OS_Windows
