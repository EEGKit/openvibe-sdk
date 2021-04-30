#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>

namespace OpenViBE {
namespace PluginInspector {
class CPluginObjectDescEnum
{
public:

	explicit CPluginObjectDescEnum(const Kernel::IKernelContext& ctx) : m_kernelCtx(ctx) { }
	virtual ~CPluginObjectDescEnum() { }

	virtual bool enumeratePluginObjectDesc(std::vector<const Plugins::IPluginObjectDesc*>& pod);
	virtual bool enumeratePluginObjectDesc(const CIdentifier& parentClassID);

	virtual bool callback(const Plugins::IPluginObjectDesc& pod) = 0;

	static std::string transform(const std::string& in, bool removeSlash = false);

protected:

	const Kernel::IKernelContext& m_kernelCtx;
};
}  // namespace PluginInspector
}  // namespace OpenViBE
