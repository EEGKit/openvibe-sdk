#pragma once

#include "system/ovCDynamicModule.h"

#if defined TARGET_OS_Windows
	#include <system/WindowsUtilities.h> // Allowed to use utf8_to_utf16 function for os that use utf16
#endif

#include <map>
#include <vector>
#include <cstring>

using namespace System;

namespace
{
	const std::map<CDynamicModule::ELogErrorCodes, std::string> s_ErrorMap =
	{
		{ CDynamicModule::LogErrorCodes_ModuleAlreadyLoaded, "A module is already loaded." },
		{ CDynamicModule::LogErrorCodes_NoModuleLoaded, "No module loaded." },
		{ CDynamicModule::LogErrorCodes_FilenameEmpty, "The filename is empty." },
		{ CDynamicModule::LogErrorCodes_FolderPathInvalid, "The folder path is invalid." },
		{ CDynamicModule::LogErrorCodes_RegistryQueryFailed, "The registry query is invalid." },
		{ CDynamicModule::LogErrorCodes_UnloadModuleFailed, "Fail to unload the module." },
		{ CDynamicModule::LogErrorCodes_FailToLoadModule, "Fail to load the module." },
		{ CDynamicModule::LogErrorCodes_InvalidSymbol, "The symbol is invalid." },
		{ CDynamicModule::LogErrorCodes_ModuleNotFound, "Module not found." }
		
	};

	std::vector<std::string> split(char* str, const char* delim)
	{
		char* token = strtok(str, delim);

		std::vector<std::string> result;

		while (token != NULL)
		{
			result.push_back(token);
			token = strtok(NULL, delim);
		}

		return result;
	}
}

const char* CDynamicModule::getErrorString(unsigned int errorCode) const
{
	if (s_ErrorMap.count(CDynamicModule::ELogErrorCodes(errorCode)) == 0)
	{
		return "Invalid error code";
	}
	else
	{
		return s_ErrorMap.at(CDynamicModule::ELogErrorCodes(errorCode)).c_str();
	}
}

const char* CDynamicModule::getErrorDetails(void) const
{
	return &m_ErrorDetails[0];
}

unsigned int CDynamicModule::getLastError(void) const
{
	return m_ErrorCode;
}

CDynamicModule::CDynamicModule(void)
	: m_Handle(nullptr)
	, m_ErrorMode(m_ErrorModeNull)
	, m_ShouldFreeModule(true)
	, m_ErrorCode(LogErrorCodes_NoError)
{
	::strcpy(m_ErrorDetails, "");
	::strcpy(m_Filename, "");
}

CDynamicModule::~CDynamicModule(void)
{
	this->unload();
}

// --------------------------------------

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromExisting(const char* modulePath, const char* symbolNameCheck)
{
	if (m_Handle)
	{
		this->setError(LogErrorCodes_ModuleAlreadyLoaded, "Module [" + std::string(m_Filename) + "] is already loaded");
		return false;
	}

	m_Handle = ::GetModuleHandle(modulePath);

	if (m_Handle == NULL)
	{
		this->setError(LogErrorCodes_FailToLoadModule, "Windows error code: " + std::to_string(::GetLastError()));
		return false;
	}

	if (m_Handle != nullptr && symbolNameCheck != nullptr)
	{
		if (::GetProcAddress((HMODULE)m_Handle, symbolNameCheck) == NULL)
		{
			this->setError(LogErrorCodes_InvalidSymbol, "Windows error code: " + std::to_string(::GetLastError()));
			this->unload();
			return false;
		}
	}
	if (m_Handle)
	{
		::strcpy(m_Filename, modulePath);
	}

	return true ;
}
#endif

bool CDynamicModule::loadFromPath(const char* modulePath, const char* symbolNameCheck)
{
	if (m_Handle)
	{
		this->setError(LogErrorCodes_ModuleAlreadyLoaded, "Module [" + std::string(m_Filename) + "] is already loaded");
		return false;
	}

	// Verify empty filename
	if (modulePath == NULL || (modulePath != NULL && modulePath[0] == '\0'))
	{
		this->setError(LogErrorCodes_FilenameEmpty);
		return false;
	}

#if defined TARGET_OS_Windows
	DWORD l_ErrorCode = ERROR_SUCCESS;

	if (m_ErrorMode == m_ErrorModeNull)
	{
		UINT l_Mode = ::SetErrorMode(m_ErrorModeNull);
		::SetErrorMode(l_Mode);
	}
	else
	{
		::SetErrorMode(m_ErrorMode);
	}

	m_Handle = System::WindowsUtilities::utf16CompliantLoadLibrary(modulePath, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		
	if (m_Handle == NULL)
	{
		l_ErrorCode = GetLastError();
		this->setError(LogErrorCodes_FailToLoadModule);
		return false;
	}

	if (m_Handle != NULL && symbolNameCheck != nullptr)
	{
		if (::GetProcAddress((HMODULE)m_Handle, symbolNameCheck) == NULL)
		{
			this->setError(LogErrorCodes_InvalidSymbol, "Symbol invalid: [" + std::string(symbolNameCheck) + "].Windows error code : " + std::to_string(GetLastError()));
			this->unload();
			return false;
		}
	}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	m_Handle = ::dlopen(modulePath, RTLD_LAZY|RTLD_GLOBAL);

	if (m_Handle == NULL)
	{
		this->setError(LogErrorCodes_FailToLoadModule);
		return false;
	}

	if(m_Handle != NULL && symbolNameCheck != NULL)
	{
		if(::dlsym(m_Handle, symbolNameCheck) == NULL)
		{
			::dlclose(m_Handle);
			m_Handle = NULL;
			this->setError(LogErrorCodes_FailToLoadModule);
			return false;
		}
	}
#endif
	if (m_Handle)
	{
		::strcpy(m_Filename, modulePath);
	}

	return true;
}

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromKnownPath(int standardPath, const char* modulePath, const char* symbolNameCheck)
{
	if (m_Handle)
	{
		this->setError(LogErrorCodes_ModuleAlreadyLoaded, "Module [" + std::string(m_Filename) + "] is already loaded");
		return false;
	}

	char l_sDLLPath[MAX_PATH];

	HRESULT result = ::SHGetFolderPath(NULL, standardPath, NULL, SHGFP_TYPE_CURRENT, l_sDLLPath);

	if (result != S_OK)
	{
		this->setError(LogErrorCodes_FolderPathInvalid, "Windows error code: " + std::to_string(result));
		return false;
	}

	::strcat(l_sDLLPath, "\\");
	::strcat(l_sDLLPath, modulePath);
	return CDynamicModule::loadFromPath(l_sDLLPath, symbolNameCheck); // Error set in the loadFromPath function
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromEnvironment(const char* environmentPath, const char* modulePath, const char* symbolNameCheck)
{
	if (m_Handle)
	{
		this->setError(LogErrorCodes_ModuleAlreadyLoaded, "Module [" + std::string(m_Filename) + "] is already loaded");
		return false;
	}

	char* l_sEnvironmentPath = ::getenv(environmentPath);

	if (l_sEnvironmentPath == NULL)
	{
		this->setError(LogErrorCodes_EnvironmentVariableInvalid);
		return false;
	}

	std::vector<std::string> paths = split(l_sEnvironmentPath, ";");

	for (const std::string& path : paths)
	{
		char l_sDLLPath[1024];
		::sprintf(l_sDLLPath, "%s\\%s", path.c_str(), modulePath);

		if (CDynamicModule::loadFromPath(l_sDLLPath, symbolNameCheck))
		{
			return true;
		}
	}

	this->setError(LogErrorCodes_ModuleNotFound);
	return false;
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromRegistry(HKEY key, const char* registryPath, const char* modulePath, const char* symbolNameCheck)
{
	char l_sDLLPath[1024];
	DWORD l_uiSize = sizeof(l_sDLLPath);
	l_sDLLPath[0] = '\0';

	HKEY l_hKey = 0;
	LONG result = RegOpenKeyEx(key, TEXT(registryPath), NULL, KEY_ALL_ACCESS | KEY_WOW64_32KEY, &l_hKey);

	if (result != ERROR_SUCCESS)
	{
		this->setError(LogErrorCodes_RegistryQueryFailed, "Fail to open key. Windows error code: " + std::to_string(::GetLastError()));
		RegCloseKey(l_hKey);
		return false;
	}

	result = ::RegQueryValueEx(l_hKey, NULL, NULL, NULL, (unsigned char*)l_sDLLPath, &l_uiSize);
	RegCloseKey(l_hKey);

	if (result == ERROR_SUCCESS)
	{
		::strcat(l_sDLLPath, modulePath);
		return CDynamicModule::loadFromPath(l_sDLLPath, symbolNameCheck); // Error set in the loadFromPath function
	}
	else
	{
		this->setError(LogErrorCodes_RegistryQueryFailed, "Fail to query value. Windows error code: " + std::to_string(::GetLastError()));
		return false;
	}
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::isModuleCompatible(const std::string& filePath, int architecture)
{
	IMAGE_NT_HEADERS headers;

	if (!CDynamicModule::getImageFileHeaders(filePath, headers))
	{
		return false; // Error set in the getImageFileHeaders function
	}

	return headers.FileHeader.Machine == architecture;
}
#endif

// --------------------------------------

bool CDynamicModule::unload(void)
{
	if (!m_Handle)
	{
		this->setError(LogErrorCodes_NoModuleLoaded);
		return false;
	}
	
	// If the flag m_bShouldFreeModule, set to true per default, is set to false,
	// the module is not unloaded.
	// This flag was first set for Enobio3G driver which dll freezes when unloaded
	if (!m_ShouldFreeModule)
	{
		return true;
	}

#if defined TARGET_OS_Windows		
	if (::FreeModule(reinterpret_cast<HMODULE>(m_Handle)) == 0)
	{
		this->setError(LogErrorCodes_UnloadModuleFailed, "Windows error code: " + std::to_string(::GetLastError()));
		return false;
	}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	if(::dlclose(m_Handle) != 0)
	{
		this->setError(LogErrorCodes_UnloadModuleFailed);
		return false;
	}
#else
#endif

	::strcpy(m_Filename, "");
	m_Handle = NULL;

	return true;
}

bool CDynamicModule::isLoaded(void) const
{
	return m_Handle != NULL;
}

const char* CDynamicModule::getFilename(void) const
{
	return m_Filename;
}

// Should be used to avoid the warning "Missing dll" when loading acquisition server
// This can happen when the loaded library needs a second library that is not detected
void CDynamicModule::setDynamicModuleErrorMode(uint32 errorMode)
{
	m_ErrorMode = errorMode;
}

void CDynamicModule::setShouldFreeModule(bool shouldFreeModule)
{
	m_ShouldFreeModule = shouldFreeModule;
}

CDynamicModule::pSymbol_t CDynamicModule::getSymbolGeneric(const char* symbolName) const
{
	CDynamicModule::pSymbol_t l_pResult = NULL;

	if (!m_Handle)
	{
		m_ErrorCode = LogErrorCodes_NoModuleLoaded;
		return l_pResult;
	}
	
	if (m_Handle)
	{
#if defined TARGET_OS_Windows
		l_pResult = (CDynamicModule::pSymbol_t)::GetProcAddress(reinterpret_cast<HMODULE>(m_Handle), symbolName);

		if (!l_pResult)
		{
			//m_ErrorDetails = "Windows error code: " + GetLastError();
			m_ErrorCode = LogErrorCodes_InvalidSymbol;
			return l_pResult;
		}

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		l_pResult = (CDynamicModule::pSymbol_t)::dlsym(m_Handle, symbolName);

		if (!l_pResult)
		{
			//m_ErrorDetails = "Linux error: " + std::string(::dlerror());
			m_ErrorCode = LogErrorCodes_InvalidSymbol;
			return l_pResult;
		}
#else
#endif
	}

	return l_pResult;
}

#ifdef TARGET_OS_Windows
bool CDynamicModule::getImageFileHeaders(const std::string& fileName, IMAGE_NT_HEADERS& headers)
{
	HANDLE l_hFileHandle = CreateFile(
		fileName.c_str(),
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

void CDynamicModule::setError(CDynamicModule::ELogErrorCodes errorCode, const std::string& details)
{
	m_ErrorCode = errorCode;
	::strcpy(m_ErrorDetails, details.c_str());
}
