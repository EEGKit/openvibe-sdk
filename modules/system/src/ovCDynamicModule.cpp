#include "system/ovCDynamicModule.h"

#if defined TARGET_OS_Windows
	#include <system/WindowsUtilities.h> // Allowed to use utf8_to_utf16 function for os that use utf16
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	#include <dlfcn.h>
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

#if defined TARGET_OS_Windows
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

	std::string formatWindowsError(DWORD errorCode)
	{
		LPTSTR l_ErrorText;

		FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM |                 // use system message tables to retrieve error text
			FORMAT_MESSAGE_ALLOCATE_BUFFER |             // allocate buffer on local heap for error text
			FORMAT_MESSAGE_IGNORE_INSERTS,               // Important! will fail otherwise, since we're not (and CANNOT) pass insertion parameters
			NULL,                                        // unused with FORMAT_MESSAGE_FROM_SYSTEM
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR)&l_ErrorText,                        // output
			0,                                           // minimum size for output buffer
			NULL
			);                                           // arguments - see note

		return std::string(l_ErrorText);
	}
#endif
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
	: m_Handle(NULL)
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
		this->setError(LogErrorCodes_FailToLoadModule, "Windows error: " + ::formatWindowsError(::GetLastError()));
		return false;
	}

	if (symbolNameCheck != NULL)
	{
		if (::GetProcAddress((HMODULE)m_Handle, symbolNameCheck) == NULL)
		{
			this->unload();
			this->setError(LogErrorCodes_InvalidSymbol, "Windows error: " + ::formatWindowsError(::GetLastError()));
			return false;
		}
	}

	::strcpy(m_Filename, modulePath);

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
		this->setError(LogErrorCodes_FailToLoadModule, "Fail to load [" + std::string(modulePath) + "]. Windows error:" + ::formatWindowsError(::GetLastError()));
		return false;
	}

	if (symbolNameCheck != NULL)
	{
		if (::GetProcAddress((HMODULE)m_Handle, symbolNameCheck) == NULL)
		{
			this->unload();
			this->setError(LogErrorCodes_InvalidSymbol, "Symbol invalid: [" + std::string(symbolNameCheck) + "]. Windows error: " + ::formatWindowsError(::GetLastError()));
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

	if(symbolNameCheck != NULL)
	{
		if(::dlsym(m_Handle, symbolNameCheck) == NULL)
		{
			::dlclose(m_Handle);
			m_Handle = NULL;

			this->setError(LogErrorCodes_InvalidSymbol, "Linux error: " + std::string(::dlerror()));
			return false;
		}
	}
#endif

	::strcpy(m_Filename, modulePath);

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

	char l_DLLPath[MAX_PATH];

	HRESULT result = ::SHGetFolderPath(NULL, standardPath, NULL, SHGFP_TYPE_CURRENT, l_DLLPath);

	if (result != S_OK)
	{
		this->setError(LogErrorCodes_FolderPathInvalid, "Windows error code: " + std::to_string(result));
		return false;
	}

	::strcat(l_DLLPath, "\\");
	::strcat(l_DLLPath, modulePath);
	return CDynamicModule::loadFromPath(l_DLLPath, symbolNameCheck); // Error set in the loadFromPath function
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

	char* l_EnvironmentPath = ::getenv(environmentPath);

	if (l_EnvironmentPath == NULL)
	{
		this->setError(LogErrorCodes_EnvironmentVariableInvalid);
		return false;
	}

	std::vector<std::string> paths = split(l_EnvironmentPath, ";");

	for (const std::string& path : paths)
	{
		char l_DLLPath[MAX_PATH];
		::sprintf(l_DLLPath, "%s\\%s", path.c_str(), modulePath);

		if (CDynamicModule::loadFromPath(l_DLLPath, symbolNameCheck))
		{
			return true;
		}
	}

	this->setError(LogErrorCodes_ModuleNotFound);
	return false;
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::loadFromRegistry(HKEY key, const char* registryPath, const char* registryKeyName, REGSAM samDesired, const char* modulePath, const char* symbolNameCheck)
{
	char l_DLLPath[MAX_PATH];
	DWORD l_Size = sizeof(l_DLLPath);
	l_DLLPath[0] = '\0';

	HKEY l_Key = 0;

	LONG result = RegOpenKeyEx(key, TEXT(registryPath), NULL, samDesired, &l_Key);

	if (result != ERROR_SUCCESS)
	{
		this->setError(LogErrorCodes_RegistryQueryFailed, "Fail to open registry key. Windows error code: " + std::to_string(result));
		RegCloseKey(l_Key);
		return false;
	}

	result = ::RegQueryValueEx(l_Key, registryKeyName, 0, NULL, (unsigned char*)l_DLLPath, &l_Size);

	if (result == ERROR_SUCCESS)
	{
		::strcat(l_DLLPath, modulePath);
		return CDynamicModule::loadFromPath(l_DLLPath, symbolNameCheck); // Error set in the loadFromPath function
	}
	else
	{
		this->setError(LogErrorCodes_RegistryQueryFailed, "Fail to query registry value. Windows error code: " + std::to_string(result));
		return false;
	}
}
#endif

#if defined TARGET_OS_Windows
bool CDynamicModule::isModuleCompatible(const char* filePath, int architecture)
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
		this->setError(LogErrorCodes_UnloadModuleFailed, "Windows error code: " + ::formatWindowsError(::GetLastError()));
		return false;
	}
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	if(::dlclose(m_Handle) != 0)
	{
		this->setError(LogErrorCodes_UnloadModuleFailed, "Linux error: " + std::string(::dlerror()));
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

void CDynamicModule::setDynamicModuleErrorMode(unsigned int errorMode)
{
	m_ErrorMode = errorMode;
}

void CDynamicModule::setShouldFreeModule(bool shouldFreeModule)
{
	m_ShouldFreeModule = shouldFreeModule;
}

CDynamicModule::symbol_t CDynamicModule::getSymbolGeneric(const char* symbolName) const
{
	CDynamicModule::symbol_t l_Result = NULL;

	if (!m_Handle)
	{
		m_ErrorCode = LogErrorCodes_NoModuleLoaded;
		return l_Result;
	}
	
	if (m_Handle)
	{
#if defined TARGET_OS_Windows
		l_Result = (CDynamicModule::symbol_t)::GetProcAddress(reinterpret_cast<HMODULE>(m_Handle), symbolName);

		if (!l_Result)
		{
			m_ErrorCode = LogErrorCodes_InvalidSymbol;
			return l_Result;
		}

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
		l_Result = (CDynamicModule::symbol_t)::dlsym(m_Handle, symbolName);

		if (!l_Result)
		{
			m_ErrorCode = LogErrorCodes_InvalidSymbol;
			return l_Result;
		}
#else
#endif
	}

	return l_Result;
}

#ifdef TARGET_OS_Windows
bool CDynamicModule::getImageFileHeaders(const char* fileName, IMAGE_NT_HEADERS& headers)
{
	HANDLE l_FileHandle = CreateFile(
		fileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0
		);

	if (l_FileHandle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	HANDLE l_ImageHandle = CreateFileMapping(
		l_FileHandle,
		NULL,
		PAGE_READONLY,
		0,
		0,
		NULL
		);

	if (l_ImageHandle == 0)
	{
		CloseHandle(l_FileHandle);
		return false;
	}

	void* l_ImagePtr = MapViewOfFile(
		l_ImageHandle,
		FILE_MAP_READ,
		0,
		0,
		0
		);

	if (l_ImagePtr == NULL)
	{
		CloseHandle(l_ImageHandle);
		CloseHandle(l_FileHandle);
		return false;
	}

	PIMAGE_NT_HEADERS l_HeadersPtr = ImageNtHeader(l_ImagePtr);

	if (l_HeadersPtr == NULL)
	{
		UnmapViewOfFile(l_ImagePtr);
		CloseHandle(l_ImageHandle);
		CloseHandle(l_FileHandle);
		return false;
	}

	headers = *l_HeadersPtr;

	UnmapViewOfFile(l_ImagePtr);
	CloseHandle(l_ImageHandle);
	CloseHandle(l_FileHandle);

	return true;
};
#endif

void CDynamicModule::setError(CDynamicModule::ELogErrorCodes errorCode, const std::string& details)
{
	m_ErrorCode = errorCode;
	::strcpy(m_ErrorDetails, details.c_str());
}
