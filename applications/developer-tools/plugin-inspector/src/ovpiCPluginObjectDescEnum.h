#ifndef __OpenViBEPluginInspector_CPluginObjectDescEnum_H__
#define __OpenViBEPluginInspector_CPluginObjectDescEnum_H__

#include "ovpi_base.h"

#include <string>
#include <vector>

class CPluginObjectDescEnum
{
public:

	CPluginObjectDescEnum(const OpenViBE::Kernel::IKernelContext& rKernelContext);
	virtual ~CPluginObjectDescEnum(void);

	virtual OpenViBE::boolean enumeratePluginObjectDesc(std::vector<const OpenViBE::Plugins::IPluginObjectDesc*>& vPluginDescriptors);
	virtual OpenViBE::boolean enumeratePluginObjectDesc(std::vector<OpenViBE::uint64> * pAuthorizedPluginsIdentifiers = NULL);
	virtual OpenViBE::boolean enumeratePluginObjectDesc(const OpenViBE::CIdentifier& rParentClassIdentifier, std::vector<OpenViBE::uint64> * pAuthorizedPluginsIdentifiers = NULL);

	virtual OpenViBE::boolean callback(
		const OpenViBE::Plugins::IPluginObjectDesc& rPluginObjectDesc)=0;

	static std::string transform(const std::string& sInput, const OpenViBE::boolean bRemoveSlash=false);

protected:

	const OpenViBE::Kernel::IKernelContext& m_rKernelContext;
};

#endif // __OpenViBEPluginInspector_CPluginObjectDescEnum_H__
