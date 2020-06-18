///-------------------------------------------------------------------------------------------------
/// 
/// \file directories.hpp
/// \brief Some functions to get some Root directories of OpenViBE.
/// 
/// The functions in this file should be only used in bootstrapping circumstances where Configuration Manager (Kernel Context) is not available.
/// With access to Configuration Manager, the paths should be fetched using tokens such as ${Path_UserData}. Note that in such a case changing the environment variable later may no longer be affected in the token value.
/// 
/// \version 1.0.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// \remarks These functions may not be thread-safe
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "ovCString.h"

#include <cstdlib>	 // For getenv()
#include <string>

#if defined TARGET_OS_Windows
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#endif

#if defined TARGET_OS_Windows
#include <Windows.h>
#include "m_ConverterUtf8.h"
#include <memory>
#elif defined TARGET_OS_Linux
#include <unistd.h>
#include <cstring>
#elif defined TARGET_OS_MacOS
#include <mach-o/dyld.h>
#endif

namespace OpenViBE {
/// <summary> Class to find root folder. </summary>
/// <remarks> We used class to have some hidden functions and keep all definition in header (for link). </remarks>
class Directories
{
public:
	/// <summary> Force this class to be used as Namespace . </summary>
	Directories() = delete;

#ifdef OV_USE_CMAKE_DEFAULT_PATHS
	/// <summary> Gets the dist root dir. </summary>
	/// <returns></returns>
	static CString getDistRootDir() { return pathFromEnv("OV_PATH_ROOT", OV_CMAKE_PATH_ROOT).c_str(); }
#else
	/// <summary> Gets the dist root dir. </summary>
	/// <returns></returns>
	static CString getDistRootDir() { return pathFromEnv("OV_PATH_ROOT", guessRootDir().c_str()).c_str(); }
#endif

	/// <summary> Gets the bin dir. </summary>
	/// <returns></returns>
	static CString getBinDir() { return getExistingPath("OV_PATH_BIN", "/bin", OV_CMAKE_PATH_BIN).c_str(); }

	/// <summary> Gets the data dir. </summary>
	/// <returns></returns>
	static CString getDataDir() { return getExistingPath("OV_PATH_DATA", "/share/openvibe", OV_CMAKE_PATH_DATA).c_str(); }

#if defined TARGET_OS_Windows
	/// <summary> Gets the library dir. </summary>
	/// <returns></returns>
	static CString getLibDir() { return getExistingPath("OV_PATH_LIB", "/bin", OV_CMAKE_PATH_BIN).c_str(); }

	/// <summary> Gets the user home dir. </summary>
	/// <returns></returns>
	static CString getUserHomeDir() { return pathFromEnv("USERPROFILE", "openvibe-user").c_str(); }

	/// <summary> Gets the user data dir. </summary>
	/// <returns></returns>
	static CString getUserDataDir() { return (pathFromEnv("APPDATA", "openvibe-user") + "/" + OV_CONFIG_SUBDIR).c_str(); }

	/// <summary> Gets the all user data dir. </summary>
	/// <returns></returns>
	static std::string getAllUsersDataDir()
	{
		std::string path = pathFromEnv("PROGRAMDATA", "");					// first chance: Win7 and higher
		if (path.empty()) { path = pathFromEnv("ALLUSERSPROFILE", ""); }	// second chance: WinXP
		if (path.empty()) { path = "openvibe-user"; }						// fallback
		return path + "/" + OV_CONFIG_SUBDIR;
	}
#else
	/// <summary> Gets the library dir. </summary>
	/// <returns></returns>
	static CString getLibDir() { return getExistingPath("OV_PATH_LIB", "/lib", OV_CMAKE_PATH_LIB).c_str(); }
	
	/// <summary> Gets the user home dir. </summary>
	/// <returns></returns>
	static CString getUserHomeDir() { return pathFromEnv("HOME", "openvibe-user").c_str(); }
	
	/// <summary> Gets the user data dir. </summary>
	/// <returns></returns>
	static CString getUserDataDir() { return (getUserHomeDir() + "/.config/" + OV_CONFIG_SUBDIR); }
	
	/// <summary> Gets the all user data dir. </summary>
	/// <returns></returns>
	static CString getAllUsersDataDir() { return (getUserHomeDir() + "/.config/" + OV_CONFIG_SUBDIR); }
#endif

	/// <summary> Gets the log dir. </summary>
	/// <returns></returns>
	static CString getLogDir() { return getUserDataDir() + "/log"; }

private:

	/// <summary> Used to convert \ in paths to /, we need this because \ is a special character for .conf token parsing. </summary>
	/// <param name="in"> The string to convert. </param>
	/// <returns></returns>
	static std::string convertPath(const std::string& in)
	{
		std::string out(in);
		const size_t len = in.length();
		for (size_t i = 0; i < len; ++i) { if (in[i] == '\\') { out[i] = '/'; } }
		return out;
	}

	/// <summary> Try to guess the root directory by assuming that any program that uses the kernel is in the bin subdirectory of the dist folder. </summary>
	/// <returns></returns>
	static std::string guessRootDir()
	{
		static std::string rootDir;					// Static to guess only one time
		if (!rootDir.empty()) { return rootDir; }	// if already guess

		std::string fullpath;
#if defined TARGET_OS_Windows
		// Unlike GetEnvironmentVariableW, this function can not return the length of the actual path
		const std::unique_ptr<wchar_t> utf16Value(new wchar_t[1024]);
		GetModuleFileNameW(nullptr, utf16Value.get(), 1024);
		const int multiByteSize = WideCharToMultiByte(CP_UTF8, 0, utf16Value.get(), -1, nullptr, 0, nullptr, nullptr);
		if (multiByteSize == 0)
		{
			// There are no sensible values to return if the above call fails and the program will not be
			// able to run in any case.
			std::abort();
		}
		const std::unique_ptr<char> utf8Value(new char[size_t(multiByteSize)]);
		if (WideCharToMultiByte(CP_UTF8, 0, utf16Value.get(), -1, utf8Value.get(), multiByteSize, nullptr, nullptr) == 0) { std::abort(); }

		fullpath = convertPath(utf8Value.get());
#elif defined TARGET_OS_Linux
			char path[2048];
			memset(path, 0, sizeof(path)); // readlink does not produce 0 terminated strings
			readlink("/proc/self/exe", path, sizeof(path));
			fullpath = std::string(path);
#elif defined TARGET_OS_MacOS
			size_t size = 0;
			_NSGetExecutablePath(nullptr, &size);
			std::unique_ptr<char> path(new char[size + 1]);

			if (_NSGetExecutablePath(path.get(), &size) != 0) { std::abort(); }

			fullpath = std::string(path.get());
#endif
		const auto slashBeforeLast = fullpath.find_last_of('/', fullpath.find_last_of('/') - 1);
		rootDir                    = fullpath.substr(0, slashBeforeLast);
		return rootDir;
	}

	/// <summary> Returns ENV variable value or defaultPath if the variable doesn't exist. The path is converted with each \ to /. </summary>
	/// <param name="envVar"> The env variable. </param>
	/// <param name="defaultPath"> The default path. </param>
	/// <returns></returns>
	static std::string pathFromEnv(const char* envVar, const char* defaultPath)
	{
#if defined TARGET_OS_Windows
		// Using std::getenv on Windows yields UTF7 strings which do not work with the Utf8ToUtf16 function
		// as this seems to be the only place where we actually get UTF7, let's get it as UTF16 by default
		const DWORD wideBufferSize = GetEnvironmentVariableW(Common::Converter::Utf8ToUtf16(envVar).c_str(), nullptr, 0);
		if (wideBufferSize == 0) { return convertPath(defaultPath); }
		const std::unique_ptr<wchar_t> utf16Value(new wchar_t[wideBufferSize]);
		GetEnvironmentVariableW(Common::Converter::Utf8ToUtf16(envVar).c_str(), utf16Value.get(), wideBufferSize);

		const int multiByteSize = WideCharToMultiByte(CP_UTF8, 0, utf16Value.get(), -1, nullptr, 0, nullptr, nullptr);
		if (multiByteSize == 0) { return convertPath(defaultPath); }
		const std::unique_ptr<char> utf8Value(new char[size_t(multiByteSize)]);
		if (WideCharToMultiByte(CP_UTF8, 0, utf16Value.get(), -1, utf8Value.get(), multiByteSize, nullptr, nullptr) == 0) { return convertPath(defaultPath); }

		const char* pathPtr = utf8Value.get();
#else
		const char* pathPtr = std::getenv(envVar);
#endif
		const std::string path = (pathPtr ? pathPtr : defaultPath);
		return convertPath(path);
	}

	/// <summary> Returns ENV variable if it is defined, otherwise it extends the ROOT variable if it exists, finally returns a default path. </summary>
	/// <param name="envVar"> The env variable. </param>
	/// <param name="rootPostfix"> The root postfix. </param>
	/// <param name="defaultPath"> The default path. </param>
	/// <returns></returns>
	static std::string getExistingPath(const char* envVar, const char* rootPostfix, const char* defaultPath)
	{
		if (std::getenv(envVar)) { return pathFromEnv(envVar, defaultPath); }
		if (std::getenv("OV_PATH_ROOT"))
		{
			// the default case for this one is wrong but it should never happen
			return pathFromEnv("OV_PATH_ROOT", "") + rootPostfix;
		}
#ifdef OV_USE_CMAKE_DEFAULT_PATHS
		return convertPath(defaultPath);
#else
		return guessRootDir() + rootPostfix;
#endif
	}
};  // Class/Namespace Directories
}  // namespace OpenViBE
