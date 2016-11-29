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
	class CSymbolHelper; // forward declare to make function declaration possible

	class System_API CDynamicModule
	{
	public:
		CDynamicModule(void);
		virtual ~CDynamicModule(void);

		/**
		 * \brief Load existing module that was already loaded by the process.
		 *
		 * \param sModulePath The path to the module.
		 * \param sSymbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromExisting(const char* sModulePath, const char* sSymbolNameCheck = nullptr);

		/**
		 * \brief Load module from a path.
		 *
		 * \param sModulePath
		 * \param sSymbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromPath(const char* sModulePath, const char* sSymbolNameCheck = nullptr);

#if defined TARGET_OS_Windows
		/**
		 * \brief Load module from known path. Windows only.
		 *
		 * \param iStandardPath A CSIDL value that identifies the folder whose path is to be retrieved. Only real folders are valid. If a virtual folder is specified, this function fails. You can force creation of a folder by combining the folder's CSIDL with CSIDL_FLAG_CREATE.
		 * \param sModulePath
		 * \param sSymbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromKnownPath(int iStandardPath, const char* sModulePath, const char* sSymbolNameCheck = nullptr);
#endif

#if defined TARGET_OS_Windows
		/**
		 * \brief Load module from Windows environment. Windows only.
		 *
		 * \param sEnvironmentPath Environment path
		 * \param sModulePath Module file path
		 * \param sSymbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromEnvironment(const char* sEnvironmentPath, const char* sModulePath, const char* sSymbolNameCheck = nullptr);
#endif

#if defined TARGET_OS_Windows
		/**
		 * \brief Load module from the registry. Windows only.
		 *
		 * \param ui32Key Registry key. Check https://msdn.microsoft.com/en-us/library/windows/desktop/ms724836
		 * \param sRegistryPath Registry path
		 * \param sModulePath sModulePath Module path.
		 * \param sSymbolNameCheck Symbol to check if it is present in the module. It is optionnal and is nullptr by default.
		 *
		 * \retval true If the module loaded successfully.
		 * \retval false If module loading failed.
		 */
		bool loadFromRegistry(HKEY key, const char* sRegistryPath, const char* sModulePath, const char* sSymbolNameCheck = nullptr);
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
		 * \param sFilePath Module file path
		 * \param architecture Architecture code
		 *
		 * \retval true If the module architecture is equal to the architecture parameter. 
		 * \retval false If the module is unequal to the architecture parameter.
		 */
		static bool isModuleCompatible(const std::string& sFilePath, int architecture);
#endif

		// --------------------------------------

		/**
		 * \brief Unload the module. If setShouldFreeModule(false) is called, the unload() has no effect.
		 *
		 * \retval true In case of success
		 * \retval false In case of failure
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
		 * \brief Get the filename of the module
		 *
		 * \return the file name of the module
		 */
		const char* getFilename(void) const;

		/**
		 * Should be used to avoid the warning "Missing dll" when loading acquisition server
		 * This can happen when the loaded library needs a second library that is not detected
		 */
		void setDynamicModuleErrorMode(uint32 ui32ErrorMode);

		/**
		 * \brief Set if the module should, or not, be free. By default the module will be free.
		 *
		 * \param bShouldFreeModule Set to true to free the module when unload is called. False otherwise.
		 *
		 * \sa unload
		 */
		void setShouldFreeModule(bool bShouldFreeModule);

	protected:
		void* m_pHandle;
		char m_sFilename[1024];
		uint32 m_ui32ErrorMode;
		bool m_bShouldFreeModule;
		typedef void(*pSymbol_t)(void);

		static const unsigned int m_ErrorModeNull = 0xffffffff;

	private:
		friend class CSymbolHelper;

		/**
		* \brief Get a symbol from the module.
		*
		* \param sSymbolName Symbol name.
		*
		* \return The symbol
		*/
		pSymbol_t getSymbolGeneric(const char* sSymbolName) const;

#ifdef TARGET_OS_Windows
		/**
		 * \brief Get the image file headers. Windows only.
		 * 
		 * \param sFileName The file path.
		 * \param headers [out] The header
		 *
		 * \retval true
		 * \retval false
		 */
		static bool getImageFileHeaders(const std::string& sFilePath, IMAGE_NT_HEADERS& headers);
#endif

	};

	class CSymbolHelper
	{
	public:
		/**
		 * \brief Get a symbol from the module
		 *
		 * \param sSymbolName The symbol name
		 * \param pSymbol [out] The symbol
		 *
		 * \retval true If the symbol exists.
		 * \retval false If the symbol does not exist.
		 */
		template <typename T>
		static bool getSymbol(CDynamicModule& dynamicModule, const char* sSymbolName, T* pSymbol)
		{
			*pSymbol = (T)dynamicModule.getSymbolGeneric(sSymbolName);
			return *pSymbol != NULL;
		}
	};
};
