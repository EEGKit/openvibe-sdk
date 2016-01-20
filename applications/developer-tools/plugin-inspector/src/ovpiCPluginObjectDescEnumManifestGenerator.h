#ifndef __OpenViBEPluginInspector_CPluginObjectDescEnumManifestGenerator_H__
#define __OpenViBEPluginInspector_CPluginObjectDescEnumManifestGenerator_H__

#include "ovpiCPluginObjectDescEnum.h"

#include <map>
#include <vector>
#include <string>

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

class CPluginObjectDescEnumManifestGenerator : public CPluginObjectDescEnum
{
public:

	CPluginObjectDescEnumManifestGenerator(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBE::CString& sManifestFilename);
	virtual ~CPluginObjectDescEnumManifestGenerator(void);
	virtual OpenViBE::boolean callback(const OpenViBE::Plugins::IPluginObjectDesc& rPluginObjectDesc);

protected:

	std::string m_sManifestFilename;
};

#endif // __OpenViBEPluginInspector_CPluginObjectDescEnumManifestGenerator_H__
