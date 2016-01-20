#include "ovpiCPluginObjectDescEnumManifestGenerator.h"

#include <system/Time.h>
#include <fs/Files.h>

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <pango/pango.h>

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

CPluginObjectDescEnumManifestGenerator::CPluginObjectDescEnumManifestGenerator(const IKernelContext& rKernelContext, const CString& sManifestFilename)
	:CPluginObjectDescEnum(rKernelContext)
	,m_sManifestFilename(sManifestFilename)
{
	FILE* l_pFile=FS::Files::open(m_sManifestFilename.c_str(), "wb");
	if(l_pFile)
	{
		::fclose(l_pFile);
	}
}

CPluginObjectDescEnumManifestGenerator::~CPluginObjectDescEnumManifestGenerator(void)
{
}

boolean CPluginObjectDescEnumManifestGenerator::callback(const IPluginObjectDesc& rPluginObjectDesc)
{
	FILE* l_pFile=FS::Files::open(m_sManifestFilename.c_str(), "ab");
	if(!l_pFile)
	{
		m_rKernelContext.getLogManager() << LogLevel_Error << "Could not open manifest file [" << CString(m_sManifestFilename.c_str()) << "] for writing\n";
		return false;
	}

	uint64 l_ui64ClassId = rPluginObjectDesc.getClassIdentifier().toUInteger();
	::fprintf(l_pFile, "# %s\n", rPluginObjectDesc.getName().toASCIIString());
	::fprintf(l_pFile, "0x%016llx\n", l_ui64ClassId);
	::fclose(l_pFile);
	return true;
}
