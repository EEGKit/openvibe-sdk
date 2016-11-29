#pragma once

#include "system/ovCDynamicModule.h"

#if defined TARGET_OS_Windows
	#include <system/WindowsUtilities.h> // Allowed to use utf8_to_utf16 function for os that use utf16
#endif

using namespace System;

CDynamicModule::CDynamicModule(void)
	: m_pHandle(nullptr)
	, m_ui32ErrorMode(m_ErrorModeNull)
	, m_bShouldFreeModule(true)
{
	::strcpy(m_sFilename, "");
}

CDynamicModule::~CDynamicModule(void)
{
	this->unload();
}

// --------------------------------------

bool CDynamicModule::loadFromExisting(const char* sModulePath, const char* sSymbolNameCheck)
{
	if (!m_pHandle)
	{
#if defined TARGET_OS_Windows
		m_pHandle = ::GetModuleHandle(sModulePath);

		if (m_pHandle != nullptr && sSymbolNameCheck != nullptr)
		{
			if (::GetProcAddress((HMODULE)m_pHandle, sSymbolNameCheck) == NULL)
			{
				m_pHandle = nullptr;
			}
		}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		#warning ("CDynamicModule::loadFromExisting - No implementation for Linux and MacOS")
#endif
		if (m_pHandle)
		{
			::strcpy(m_sFilename, sModulePath);
		}
	}

	return m_pHandle != nullptr ? true : false;
}

bool CDynamicModule::loadFromPath(const char* sModulePath, const char* sSymbolNameCheck)
{
	if (!m_pHandle)
	{
		// Verify empty filename
		if (sModulePath == NULL || (sModulePath != NULL && sModulePath[0] == '\0'))
		{
			return false;
		}

#if defined TARGET_OS_Windows
		uint32 l_ui32ErrorCode = ERROR_SUCCESS;

		if (m_ui32ErrorMode == m_ErrorModeNull)
		{
			UINT l_uiMode = ::SetErrorMode(m_ErrorModeNull);
			::SetErrorMode(l_uiMode);
		}
		else
		{
			::SetErrorMode(m_ui32ErrorMode);
		}

		m_pHandle = System::WindowsUtilities::utf16CompliantLoadLibrary(sModulePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		
		if (m_pHandle == NULL)
		{
			l_ui32ErrorCode = GetLastError();
		}

		if (m_pHandle != NULL && sSymbolNameCheck != nullptr)
		{
			if (::GetProcAddress((HMODULE)m_pHandle, sSymbolNameCheck) == NULL)
			{
				::FreeModule((HMODULE)m_pHandle);
				m_pHandle = NULL;
			}
		}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		m_pHandle = ::dlopen(sModulePath, RTLD_LAZY|RTLD_GLOBAL);

		if(m_pHandle != NULL && sSymbolNameCheck != NULL)
		{
			if(::dlsym(m_pHandle, sSymbolNameCheck) == NULL)
			{
				::dlclose(m_pHandle);
				m_pHandle = NULL;
			}
		}
#endif
		if (m_pHandle)
		{
			::strcpy(m_sFilename, sModulePath);
		}
	}

	return m_pHandle != NULL ? true : false;
}

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromKnownPath(int iStandardPath, const char* sModulePath, const char* sSymbolNameCheck)
{
	if (!m_pHandle)
	{
		char l_sDLLPath[1024];

		if (::SHGetFolderPath(NULL, iStandardPath, NULL, SHGFP_TYPE_CURRENT, l_sDLLPath) == S_OK)
		{
			::strcat(l_sDLLPath, "\\");
			::strcat(l_sDLLPath, sModulePath);
			return CDynamicModule::loadFromPath(l_sDLLPath, sSymbolNameCheck);
		}
	}
	return m_pHandle != NULL ? true : false;
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromEnvironment(const char* sEnvironmentPath, const char* sModulePath, const char* sSymbolNameCheck)
{
	if (!m_pHandle)
	{
		char* l_sEnvironmentPath = ::getenv(sEnvironmentPath);
		char l_sDLLPath[1024];
		::sprintf(l_sDLLPath, "%s%s", l_sEnvironmentPath ? l_sEnvironmentPath : "", sModulePath);
		return CDynamicModule::loadFromPath(l_sDLLPath, sSymbolNameCheck);
	}

	return m_pHandle != NULL ? true : false;
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromRegistry(HKEY key, const char* sRegistryPath, const char* sModulePath, const char* sSymbolNameCheck)
{
	char l_sDLLPath[1024];
	DWORD l_uiSize = sizeof(l_sDLLPath);
	l_sDLLPath[0] = '\0';

	if (::RegQueryValueEx(key, sRegistryPath, NULL, NULL, (unsigned char*)l_sDLLPath, &l_uiSize) == ERROR_SUCCESS)
	{
		::strcat(l_sDLLPath, sModulePath);
		return CDynamicModule::loadFromPath(l_sDLLPath, sSymbolNameCheck);
	}
	else
	{
		return false;
	}
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::isModuleCompatible(const std::string& sFilePath, int iVersion)
{
	IMAGE_NT_HEADERS headers;

	if (!CDynamicModule::getImageFileHeaders(sFilePath, headers))
	{
		return false;
	}

	return headers.FileHeader.Machine == iVersion;
}
#endif

// --------------------------------------

bool CDynamicModule::unload(void)
{
	// If the flag m_bShouldFreeModule, set to true per default, is set to false,
	// the module is not unloaded.
	// This flag was first set for Enobio3G driver which dll freezes when unloaded

	if (!m_bShouldFreeModule)
	{
		return true;
	}

	if (m_pHandle)
	{
#if defined TARGET_OS_Windows
		::strcpy(m_sFilename, "");
		::FreeModule((HMODULE)m_pHandle) ? true : false;
		m_pHandle = NULL;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		::dlclose(m_pHandle);
		m_pHandle = NULL;
#else
#endif
	}
	return m_pHandle == NULL;
}

bool CDynamicModule::isLoaded(void) const
{
	return m_pHandle != NULL;
}

const char* CDynamicModule::getFilename(void) const
{
	return m_sFilename;
}

// Should be used to avoid the warning "Missing dll" when loading acquisition server
// This can happen when the loaded library needs a second library that is not detected
void CDynamicModule::setDynamicModuleErrorMode(uint32 ui32ErrorMode)
{
	m_ui32ErrorMode = ui32ErrorMode;
}

void CDynamicModule::setShouldFreeModule(bool bShouldFreeModule)
{
	m_bShouldFreeModule = false;
}

CDynamicModule::pSymbol_t CDynamicModule::getSymbolGeneric(const char* sSymbolName) const
{
	CDynamicModule::pSymbol_t l_pResult = NULL;
	
	if (m_pHandle)
	{
#if defined TARGET_OS_Windows
		l_pResult = (CDynamicModule::pSymbol_t)::GetProcAddress((HMODULE)m_pHandle, sSymbolName);
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		l_pResult = (CDynamicModule::pSymbol_t)::dlsym(m_pHandle, sSymbolName);
#else
#endif
	}
	return l_pResult;
}

#ifdef TARGET_OS_Windows
bool CDynamicModule::getImageFileHeaders(const std::string& sFileName, IMAGE_NT_HEADERS& headers)
{
	HANDLE l_hFileHandle = CreateFile(
		sFileName.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
		);

	if (l_hFileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	HANDLE l_hImageHandle = CreateFileMapping(
		l_hFileHandle,
		nullptr,
		PAGE_READONLY,
		0,
		0,
		nullptr
		);

	if (l_hImageHandle == 0)
	{
		CloseHandle(l_hFileHandle);
		return false;
	}

	void* l_pImagePtr = MapViewOfFile(
		l_hImageHandle,
		FILE_MAP_READ,
		0,
		0,
		0
		);

	if (l_pImagePtr == nullptr)
	{
		CloseHandle(l_hImageHandle);
		CloseHandle(l_hFileHandle);
		return false;
	}

	PIMAGE_NT_HEADERS l_headersPtr = ImageNtHeader(l_pImagePtr);

	if (l_headersPtr == nullptr)
	{
		UnmapViewOfFile(l_pImagePtr);
		CloseHandle(l_hImageHandle);
		CloseHandle(l_hFileHandle);
		return false;
	}

	headers = *l_headersPtr;

	UnmapViewOfFile(l_pImagePtr);
	CloseHandle(l_hImageHandle);
	CloseHandle(l_hFileHandle);

	return true;
};
#endif
