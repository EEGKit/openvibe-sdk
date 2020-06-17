#pragma once

#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>

class CPluginObjectDescEnum
{
public:

	explicit CPluginObjectDescEnum(const OpenViBE::Kernel::IKernelContext& ctx) : m_kernelCtx(ctx) { }
	virtual ~CPluginObjectDescEnum() { }

	virtual bool enumeratePluginObjectDesc(std::vector<const OpenViBE::Plugins::IPluginObjectDesc*>& pod);
	virtual bool enumeratePluginObjectDesc(const OpenViBE::CIdentifier& parentClassID);

	virtual bool callback(const OpenViBE::Plugins::IPluginObjectDesc& pod) = 0;

	static std::string transform(const std::string& in, bool removeSlash = false);

protected:

	const OpenViBE::Kernel::IKernelContext& m_kernelCtx;
};
