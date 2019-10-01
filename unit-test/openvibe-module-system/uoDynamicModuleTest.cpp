/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * OpenViBE SDK Test Software
 * Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
 * Copyright (C) Inria, 2015-2017,V1.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "gtest/gtest.h"

#include "system/ovCDynamicModule.h"

#if defined TARGET_OS_Windows
#include <winreg.h>
#endif

#include <algorithm>

using namespace System;

namespace
{
#if defined TARGET_OS_Windows
	//const std::string LIB_PATH = OV_CMAKE_PATH_LIB;

	// Microsoft specific
	const std::string EXISTING_MODULE_NAME = "NTDLL.dll";

	std::string existingModulePath;

#if defined _WIN64
#define CSIDL_SYSTEM_PLATFORM CSIDL_SYSTEM
#else
	#define CSIDL_SYSTEM_PLATFORM CSIDL_SYSTEMX86
#endif

	std::string existingModulePathName;
	const std::string NON_EXISTING_MODULE_NAME = "randomRandomRandom.dll";

	const std::string NON_EXISTING_SYMBOL = "nonExistingSymbol";

	const std::string EXISTING_ENVIRONMENT_PATH = "PATH";

	const std::string EXISTING_REGISTRY_MODULE_NAME = EXISTING_MODULE_NAME;
	const std::string SYMBOL_NAME_NTDLL             = "toupper";

	const HKEY EXISTING_REGISTRY_KEY     = HKEY_LOCAL_MACHINE; // 0x80000002
	const HKEY NON_EXISTING_REGISTRY_KEY = (HKEY)(ULONG_PTR)((LONG)0x800000FF);

	const std::string EXISTING_REGISTRY_PATH     = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Installer"; // Present on all Windows versions
	const std::string NON_EXISTING_REGISTRY_PATH = "SOFTWARE\\Random\\Random\\Random";

	int (*toupperSymbol)(int c);
	bool (*randomRandomRandomSymbol)(int number);

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	static const std::string EXISTING_SYMBOL = "createCSVLib";
	static const std::string NON_EXISTING_SYMBOL = "nonExistingSymbol";

	static const std::string LIB_PATH = OV_CMAKE_PATH_LIB;
	static const std::string EXISTING_MODULE_NAME = "libopenvibe-module-csv.so";
	static const std::string existingModulePath = LIB_PATH + "/";
	static const std::string existingModulePathName = existingModulePath + EXISTING_MODULE_NAME;
	static const std::string NON_EXISTING_MODULE_NAME = "randomRandomRandom.so";
#endif
}  // namespace

#if defined TARGET_OS_Windows
TEST(DynamicModule_Test_Case, loadFromExistingSuccessNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromExisting(EXISTING_MODULE_NAME.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = EXISTING_MODULE_NAME;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());

	ASSERT_TRUE(dynamicModule.unload());
}
#endif

#if defined TARGET_OS_Windows
TEST(DynamicModule_Test_Case, loadFromExistingFailNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(dynamicModule.loadFromExisting(NON_EXISTING_MODULE_NAME.c_str()));
	ASSERT_EQ(CDynamicModule::LogErrorCodes_FailToLoadModule, dynamicModule.getLastError());
	ASSERT_FALSE(dynamicModule.isLoaded());
}
#endif

TEST(DynamicModule_Test_Case, loadFromPathSuccessNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromPath(existingModulePathName.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = existingModulePathName;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());

	ASSERT_TRUE(dynamicModule.unload());
}

TEST(DynamicModule_Test_Case, loadFromPathFailNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(dynamicModule.loadFromPath(NON_EXISTING_MODULE_NAME.c_str()));
	ASSERT_EQ(CDynamicModule::LogErrorCodes_FailToLoadModule, dynamicModule.getLastError());
	ASSERT_FALSE(dynamicModule.isLoaded());
}

#if defined TARGET_OS_Windows // Must be tested on Linux
TEST(DynamicModule_Test_Case, loadFromPathSuccessSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromPath(existingModulePathName.c_str(), SYMBOL_NAME_NTDLL.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = existingModulePathName;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());

	ASSERT_TRUE(dynamicModule.unload());
}
#endif

TEST(DynamicModule_Test_Case, loadFromPathSymbolCheckFail)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(dynamicModule.loadFromPath(existingModulePathName.c_str(), NON_EXISTING_SYMBOL.c_str()));
	ASSERT_EQ(CDynamicModule::LogErrorCodes_InvalidSymbol, dynamicModule.getLastError());
	ASSERT_FALSE(dynamicModule.isLoaded());
}

#if defined TARGET_OS_Windows
TEST(DynamicModule_Test_Case, loadFromKnownPathSuccessNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromKnownPath(CSIDL_SYSTEM_PLATFORM, EXISTING_MODULE_NAME.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = existingModulePathName;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());

	ASSERT_TRUE(CDynamicModuleSymbolLoader::getSymbol<>(dynamicModule, SYMBOL_NAME_NTDLL.c_str(), &toupperSymbol));

	ASSERT_TRUE(dynamicModule.unload());
}

TEST(DynamicModule_Test_Case, loadFromKnownPathFailNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(dynamicModule.loadFromKnownPath(CSIDL_SYSTEM_PLATFORM, NON_EXISTING_MODULE_NAME.c_str()));
	ASSERT_EQ(CDynamicModule::LogErrorCodes_FailToLoadModule, dynamicModule.getLastError());
	ASSERT_FALSE(dynamicModule.isLoaded());
}
#endif

#if defined TARGET_OS_Windows
TEST(DynamicModule_Test_Case, loadFromEnvironmentSuccessNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromEnvironment(EXISTING_ENVIRONMENT_PATH.c_str(), EXISTING_MODULE_NAME.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = "C:\\WINDOWS\\system32\\ntdll.dll";
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());
	ASSERT_TRUE(CDynamicModuleSymbolLoader::getSymbol<>(dynamicModule, SYMBOL_NAME_NTDLL.c_str(), &toupperSymbol));

	ASSERT_TRUE(dynamicModule.unload());
}

TEST(DynamicModule_Test_Case, loadFromEnvironmentFailNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(dynamicModule.loadFromEnvironment(EXISTING_ENVIRONMENT_PATH.c_str(), NON_EXISTING_MODULE_NAME.c_str()));
	ASSERT_EQ(CDynamicModule::LogErrorCodes_ModuleNotFound, dynamicModule.getLastError());
	ASSERT_FALSE(dynamicModule.isLoaded());
}
#endif

#if defined TARGET_OS_Windows
TEST(DynamicModule_Test_Case, loadFromRegistrySuccessNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(
		dynamicModule.loadFromRegistry(EXISTING_REGISTRY_KEY, EXISTING_REGISTRY_PATH.c_str(), nullptr, KEY_READ | KEY_WOW64_64KEY, EXISTING_REGISTRY_MODULE_NAME
			.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = EXISTING_MODULE_NAME;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());

	ASSERT_TRUE(CDynamicModuleSymbolLoader::getSymbol<>(dynamicModule, SYMBOL_NAME_NTDLL.c_str(), &toupperSymbol));

	ASSERT_TRUE(dynamicModule.unload());
}

TEST(DynamicModule_Test_Case, loadFromRegistrySuccessSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(
		dynamicModule.loadFromRegistry(EXISTING_REGISTRY_KEY, EXISTING_REGISTRY_PATH.c_str(), nullptr, KEY_READ | KEY_WOW64_64KEY, EXISTING_REGISTRY_MODULE_NAME
			.c_str(), SYMBOL_NAME_NTDLL.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = EXISTING_MODULE_NAME;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());

	ASSERT_TRUE(CDynamicModuleSymbolLoader::getSymbol<>(dynamicModule, SYMBOL_NAME_NTDLL.c_str(), &toupperSymbol));

	ASSERT_TRUE(dynamicModule.unload());
}

TEST(DynamicModule_Test_Case, loadFromRegistryFailNoSymbolCheck)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(
		dynamicModule.loadFromRegistry(NON_EXISTING_REGISTRY_KEY, EXISTING_REGISTRY_PATH.c_str(), nullptr, KEY_READ | KEY_WOW64_32KEY,
			EXISTING_REGISTRY_MODULE_NAME.c_str()));
	ASSERT_FALSE(dynamicModule.isLoaded());

	ASSERT_FALSE(
		dynamicModule.loadFromRegistry(EXISTING_REGISTRY_KEY, NON_EXISTING_REGISTRY_PATH.c_str(), nullptr, KEY_READ | KEY_WOW64_32KEY,
			EXISTING_REGISTRY_MODULE_NAME.c_str()));
	ASSERT_FALSE(dynamicModule.isLoaded());

	ASSERT_FALSE(
		dynamicModule.loadFromRegistry(NON_EXISTING_REGISTRY_KEY, EXISTING_REGISTRY_PATH.c_str(), nullptr, KEY_READ | KEY_WOW64_32KEY,
			EXISTING_REGISTRY_MODULE_NAME.c_str()));
	ASSERT_FALSE(dynamicModule.isLoaded());
}
#endif

#if defined TARGET_OS_Windows // Must be tested on Linux
TEST(DynamicModule_Test_Case, getSymbolSuccess)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromPath(existingModulePathName.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = existingModulePathName;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());
	ASSERT_TRUE(CDynamicModuleSymbolLoader::getSymbol<>(dynamicModule, SYMBOL_NAME_NTDLL.c_str(), &toupperSymbol));

	const char lowerCase = 'r';
	char upperCase = char(toupperSymbol(lowerCase));

	ASSERT_EQ(upperCase, 'R');

	ASSERT_TRUE(dynamicModule.unload());
}

TEST(DynamicModule_Test_Case, getSymbolFail)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromPath(existingModulePathName.c_str()));
	ASSERT_TRUE(dynamicModule.isLoaded());
	std::string moduleNameExpected = existingModulePathName;
	std::string moduleName         = dynamicModule.getFilename();
	std::transform(moduleNameExpected.begin(), moduleNameExpected.end(), moduleNameExpected.begin(), tolower);
	std::transform(moduleName.begin(), moduleName.end(), moduleName.begin(), tolower);
	ASSERT_STREQ(moduleNameExpected.c_str(), moduleName.c_str());
	ASSERT_FALSE(CDynamicModuleSymbolLoader::getSymbol<>(dynamicModule, NON_EXISTING_SYMBOL.c_str(), &randomRandomRandomSymbol));
}
#endif

#if defined TARGET_OS_Windows
TEST(DynamicModule_Test_Case, isModulecompatibleSuccess)
{
#if defined _WIN64
	ASSERT_FALSE(CDynamicModule::isModuleCompatible(existingModulePathName.c_str(), 0x014c)); // x86
	ASSERT_TRUE(CDynamicModule::isModuleCompatible(existingModulePathName.c_str(), 0x8664)); // x64
	ASSERT_FALSE(CDynamicModule::isModuleCompatible(existingModulePathName.c_str(), 0x0200)); // ia64
#else
	ASSERT_TRUE(CDynamicModule::isModuleCompatible(existingModulePathName.c_str(), 0x014c)); // x86
	ASSERT_FALSE(CDynamicModule::isModuleCompatible(existingModulePathName.c_str(), 0x8664)); // x64
	ASSERT_FALSE(CDynamicModule::isModuleCompatible(existingModulePathName.c_str(), 0x0200)); // ia64
#endif
}
#endif

#if defined TARGET_OS_Windows
TEST(DynamicModule_Test_Case, isModulecompatibleFail)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(dynamicModule.loadFromPath(NON_EXISTING_MODULE_NAME.c_str()));
	ASSERT_FALSE(dynamicModule.isLoaded());
}
#endif

TEST(DynamicModule_Test_Case, unloadSuccess)
{
	CDynamicModule dynamicModule;

	ASSERT_TRUE(dynamicModule.loadFromPath(existingModulePathName.c_str()));
	ASSERT_TRUE(dynamicModule.unload());
}

TEST(DynamicModule_Test_Case, unloadFail)
{
	CDynamicModule dynamicModule;

	ASSERT_FALSE(dynamicModule.loadFromPath(NON_EXISTING_MODULE_NAME.c_str()));
	ASSERT_FALSE(dynamicModule.isLoaded());
	ASSERT_FALSE(dynamicModule.unload());
}

int uoDynamicModuleTest(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);

#if defined TARGET_OS_Windows
	BOOL bIsWow64 = FALSE;

	if (!IsWow64Process(GetCurrentProcess(), &bIsWow64)) { return false; }

	existingModulePath     = bIsWow64 ? "C:\\WINDOWS\\SysWOW64\\" : "C:\\WINDOWS\\system32\\";
	existingModulePathName = existingModulePath + EXISTING_MODULE_NAME;
#endif

	return RUN_ALL_TESTS();
}
