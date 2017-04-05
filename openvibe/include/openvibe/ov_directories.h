#ifndef __OpenViBE_Directories_H__
#define __OpenViBE_Directories_H__

#include <cstdlib>	 // For getenv()

#include "ovCString.h"

namespace OpenViBE
{
	class Directories
	{
	// The functions in this file should be only used in bootstrapping circumstances where Configuration Manager (Kernel Context) is not available.
	// With access to Configuration Manager, the paths should be fetched using tokens such as ${Path_UserData}. Note that in such a case changing the environment variable later may no longer be affected in the token value.

	// @NOTE These functions may not be thread-safe
	public:
		static OpenViBE::CString getDistRootDir(void)
		{
			return pathFromEnv("OV_PATH_ROOT", OV_CMAKE_PATH_ROOT);
		}
		static OpenViBE::CString getBinDir(void)
		{
			return pathFromEnvOrExtendedRoot("OV_PATH_BIN", "/bin", OV_CMAKE_PATH_BIN);
		}
		static OpenViBE::CString getDataDir(void)
		{
			return pathFromEnvOrExtendedRoot("OV_PATH_DATA", "/share/openvibe", OV_CMAKE_PATH_DATA);
		}
		static OpenViBE::CString getLibDir(void)
		{
#if defined TARGET_OS_Windows
			return pathFromEnvOrExtendedRoot("OV_PATH_LIB", "/bin", OV_CMAKE_PATH_BIN);
#else
			return pathFromEnvOrExtendedRoot("OV_PATH_LIB", "/lib", OV_CMAKE_PATH_LIB);
#endif
		}
		static OpenViBE::CString getUserHomeDir(void)
		{
#if defined TARGET_OS_Windows
			return pathFromEnv("USERPROFILE", "openvibe-user");
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return pathFromEnv("HOME", "openvibe-user");
#endif
		}
		static OpenViBE::CString getUserDataDir(void)
		{
#if defined TARGET_OS_Windows
			return  pathFromEnv("APPDATA", "openvibe-user") + "/" + OV_CONFIG_SUBDIR;
#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS
			return getUserHomeDir() + "/.config/" + OV_CONFIG_SUBDIR;
#endif
		}
		static OpenViBE::CString getLogDir(void)
		{
			return getUserDataDir() + "/log";
		}

		// Used to convert \ in paths to /, we need this because \ is a special character for .conf token parsing
		static OpenViBE::CString convertPath(const OpenViBE::CString &strIn)
		{
			OpenViBE::CString l_sOut(strIn);
			unsigned int l_sLen = strIn.length();
			for(unsigned int i=0; i<l_sLen; i++)
			{
				if(strIn[i]=='\\')
				{
					l_sOut[i]='/';
				}
			}
			return l_sOut;
		}

	private:
		// Static class, don't allow instances
		Directories() { }

		// Returns ENV variable value or sDefaultPath if the variable doesn't exist. The path is converted with each \ to /.
		static OpenViBE::CString pathFromEnv(const char *sEnvVar, const char *sDefaultPath)
		{
			const char *l_sPathPtr = std::getenv(sEnvVar);
			OpenViBE::CString l_sPath = (l_sPathPtr ? l_sPathPtr : sDefaultPath);
			return convertPath(l_sPath);
		}
		// Returns ENV variable if it is defined, otherwise it extends the ROOT variable if it exists, finally returns a default path
		static OpenViBE::CString pathFromEnvOrExtendedRoot(const char* envVar, const char* rootPostfix, const char* defaultPath)
		{
			if (const char* envPath = std::getenv(envVar))
			{
				return convertPath(envPath);
			}
			if (const char* ovPathRoot = std::getenv("OV_PATH_ROOT"))
			{
				return convertPath(CString(ovPathRoot) + rootPostfix);
			}
			return convertPath(defaultPath);
		}
	};
}

#endif // __OpenViBE_Directories_H__
