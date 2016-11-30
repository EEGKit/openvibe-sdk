#pragma once

#include "defines.h"

#if defined TARGET_OS_Windows
#include <shlobj.h>
#include <Dbghelp.h>
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
#include <dlfcn.h>
#endif

#include <string>

namespace System
{
	class CDynamicModuleSymbolLoader; // forward declare to make function declaration possible

	class System_API CDynamicModule
	{
	public:
		enum ELogErrorCodes :unsigned int
		{
			LogErrorCodes_NoError = 0,
			LogErrorCodes_ModuleAlreadyLoaded = 1,
			LogErrorCodes_NoModuleLoaded = 2,
			LogErrorCodes_FilenameEmpty = 3,
			LogErrorCodes_FolderPathInvalid = 4,
			LogErrorCodes_RegistryQueryFailed = 5,
			LogErrorCodes_UnloadModuleFailed = 6,
			LogErrorCodes_FailToLoadModule = 7,
			LogErrorCodes_InvalidSymbol = 8,
			LogErrorCodes_EnvironmentVariableInvalid = 9,
			LogErrorCodes_ModuleNotFound = 10
		};

		CDynamicModule(void);
		virtual ~CDynamicModule(void);

#if defined TARGET_OS_Windows
		/**
		 * \brief Load existing module that was already loaded by the process.
		 *
		 * \param modulePath The path to the module.
		 * \param symbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromExisting(const char* modulePath, const char* symbolNameCheck = nullptr);
#endif

		/**
		 * \brief Load module from a path.
		 *
		 * \param modulePath
		 * \param symbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromPath(const char* modulePath, const char* symbolNameCheck = nullptr);

#if defined TARGET_OS_Windows
		/**
		 * \brief Load module from known path. Windows only.
		 *
		 * \param standardPath A CSIDL value that identifies the folder whose path is to be retrieved. Only real folders are valid. If a virtual folder is specified, this function fails. You can force creation of a folder by combining the folder's CSIDL with CSIDL_FLAG_CREATE.
		 * \param modulePath Path of the module to load.
		 * \param symbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromKnownPath(int standardPath, const char* modulePath, const char* symbolNameCheck = nullptr);
#endif

#if defined TARGET_OS_Windows
		/**
		 * \brief Load module from Windows environment. Windows only.
		 *
		 * \param sEnvironmentPath Environment path.
		 * \param sModulePath Module file path.
		 * \param sSymbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromEnvironment(const char* environmentPath, const char* modulePath, const char* symbolNameCheck = nullptr);
#endif

#if defined TARGET_OS_Windows
		/**
		 * \brief Load module from the registry. Windows only.
		 *
		 * \param ui32Key Registry key. Check https://msdn.microsoft.com/en-us/library/windows/desktop/ms724836
		 * \param sRegistryPath Registry path.
		 * \param sModulePath sModulePath Module path.
		 * \param sSymbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromRegistry(HKEY key, const char* registryPath, const char* modulePath, const char* symbolNameCheck = nullptr);
#endif

#if defined TARGET_OS_Windows
		/**
		 * \brief Check the module architecture. Windows only.
		 * The architecture type of the computer. An image file can only be run on the specified computer or a system that emulates the specified computer.
		 * This member can be one of the following values.
		 * - x86: 0x014c
		 * - x64: 0x8664
		 * - ia64: 0x0200
		 *
		 * \param filePath Module file path
		 * \param architecture Architecture code
		 *
		 * \retval true If the module architecture is equal to the architecture parameter. 
		 * \retval false If the module is unequal to the architecture parameter.
		 */
		static bool isModuleCompatible(const std::string& filePath, int architecture);
#endif

		// --------------------------------------

		/**
		 * \brief Unload the module. If setShouldFreeModule(false) is called, the unload() has no effect.
		 *
		 * \retval true In case of success.
		 * \retval false In case of failure.
		 *
		 * \sa setShouldFreeModule
		 * \sa isLoaded
		 */
		bool unload(void);

		/**
		 * \brief Check if the module is loaded.
		 *
		 * \retval true If the module is loaded.
		 * \retval false If no module are loaded.
		 *
		 * \sa unload
		 * \sa setShouldFreeModule
		 */
		bool isLoaded(void) const;

		/**
		 * \brief Get the filename of the module.
		 *
		 * \return the file name of the module.
		 */
		const char* getFilename(void) const;

		/**
		 * \brief
		 */
		void setDynamicModuleErrorMode(unsigned int errorMode);

		/**
		 * \brief Set if the module should, or not, be free. By default the module will be free.
		 *
		 * \param bShouldFreeModule Set to true to free the module when unload is called. False otherwise.
		 *
		 * \sa unload
		 */
		void setShouldFreeModule(bool shouldFreeModule);

		unsigned getLastError(void) const;
		const char* getErrorString(unsigned int errorCode) const;
		const char* getErrorDetails(void) const;

	private:
		void* m_Handle;
		char m_Filename[1024];
		unsigned int m_ErrorMode;
		bool m_ShouldFreeModule;
		typedef void(*pSymbol_t)(void);

		char m_ErrorDetails[1024];
		mutable ELogErrorCodes m_ErrorCode;
		

		static const unsigned int m_ErrorModeNull = 0xffffffff;

	private:
		friend class CDynamicModuleSymbolLoader;

		void setError(ELogErrorCodes errorCode, const std::string& details = std::string());

		/**
		* \brief Get a symbol from the module.
		*
		* \param sSymbolName Symbol name.
		*
		* \return The symbol
		*/
		pSymbol_t getSymbolGeneric(const char* symbolName) const;

#ifdef TARGET_OS_Windows
		/**
		 * \brief Get the image file headers. Windows only.
		 * 
		 * \param sFileName The file path.
		 * \param headers [out] The header
		 *
		 * \retval true In case of success.
		 * \retval false In case of failure.
		 */
		static bool getImageFileHeaders(const std::string& filePath, IMAGE_NT_HEADERS& headers);
#endif

	};

	class CDynamicModuleSymbolLoader
	{
	public:
		/**
		 * \brief Get a symbol from the module.
		 *
		 * \param sSymbolName The symbol name.
		 * \param pSymbol [out] The symbol.
		 *
		 * \retval true If the symbol exists.
		 * \retval false If the symbol does not exist.
		 */
		template <typename T>
		static bool getSymbol(CDynamicModule& dynamicModule, const char* symbolName, T* symbol)
		{
			*symbol = reinterpret_cast<T>(dynamicModule.getSymbolGeneric(symbolName));
			return *symbol != NULL;
		}
	};
};
