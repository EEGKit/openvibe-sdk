#pragma once

#include "defines.h"
#if defined TARGET_OS_Windows
#include <windows.h>

namespace System
{
	class System_API WindowsUtilities
	{
	public:
		// Load a library in a matter compliant with non-ascii path
		// returns the eventual error code
		static void* utf16CompliantLoadLibrary(const char* sLibraryPath, HANDLE hFile = nullptr, DWORD dwFlags = LOAD_WITH_ALTERED_SEARCH_PATH);

		static BOOL utf16CompliantSetEnvironmentVariable(const char* sEnvVarName, const char* sEnvVarValue);
		
		// Load a library in a matter compliant with non-ascii path
		// returns the eventual error code
		static BOOL utf16CompliantCreateProcess(char* sApplicationName, char* sCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
												LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment,
												char* sCurrentDirectory,
												LPSTARTUPINFO pStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);
				
		// Load a library in a matter compliant with non-ascii path
		// returns the eventual error code
		static HINSTANCE utf16CompliantShellExecute(HWND l_pHWND, LPCTSTR lpOperation, LPCTSTR lpFile,
													LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);
	private:
		WindowsUtilities();
	};
} // namespace System

#endif // TARGET_OS_Windows
