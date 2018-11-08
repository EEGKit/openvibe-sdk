#pragma once

#include <string>

#include "ov_directories_string.h"
#include "ovCString.h"


namespace OpenViBE
{
	class Directories
	{
	// The functions in this file should be only used in bootstrapping circumstances where Configuration Manager (Kernel Context) is not available.
	// With access to Configuration Manager, the paths should be fetched using tokens such as ${Path_UserData}. Note that in such a case changing the environment variable later may no longer be affected in the token value.

	// @NOTE These functions may not be thread-safe
	public:
		Directories() = delete;

		static OpenViBE::CString getDistRootDir(void)
		{
			return OpenViBE::StringDirectories::getDistRootDir().c_str();
		}
		static OpenViBE::CString getBinDir(void)
		{
			return OpenViBE::StringDirectories::getBinDir().c_str();
		}
		static OpenViBE::CString getDataDir(void)
		{
			return OpenViBE::StringDirectories::getDataDir().c_str();
		}
		static OpenViBE::CString getLibDir(void)
		{
			return OpenViBE::StringDirectories::getLibDir().c_str();
		}
		static OpenViBE::CString getUserHomeDir(void)
		{
			return OpenViBE::StringDirectories::getUserHomeDir().c_str();
		}
		static OpenViBE::CString getUserDataDir(void)
		{
			return OpenViBE::StringDirectories::getUserDataDir().c_str();
		}
		static OpenViBE::CString getLogDir(void)
		{
			return OpenViBE::StringDirectories::getLogDir().c_str();
		}

	};
}

