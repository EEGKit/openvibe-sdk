#pragma once

#include "../ovkTKernelObject.h"

#include <map>


namespace OpenViBE {
namespace Kernel {
class CMetaboxManager final : public TKernelObject<IMetaboxManager>
{
public:
	explicit CMetaboxManager(const IKernelContext& ctx);
	~CMetaboxManager() override;
	bool addMetaboxesFromFiles(const CString& fileNameWildCard) override;
	CIdentifier getNextMetaboxObjectDescIdentifier(const CIdentifier& previousID) const override;
	const Plugins::IPluginObjectDesc* getMetaboxObjectDesc(const CIdentifier& metaboxID) const override;
	void setMetaboxObjectDesc(const CIdentifier& metaboxID, Plugins::IPluginObjectDesc* metaboxDesc) override;
	CString getMetaboxFilePath(const CIdentifier& metaboxID) const override;
	void setMetaboxFilePath(const CIdentifier& metaboxID, const CString& filePath) override;
	CIdentifier getMetaboxHash(const CIdentifier& metaboxID) const override;
	void setMetaboxHash(const CIdentifier& metaboxID, const CIdentifier& hash) override;

	_IsDerivedFromClass_Final_(TKernelObject<IMetaboxManager>, OVK_ClassId_Kernel_Metaboxes_MetaboxManager)

protected:
	std::map<CIdentifier, const Plugins::IPluginObjectDesc*> m_objectDesc;
	std::map<CIdentifier, CString> m_filepath;
	std::map<CIdentifier, CIdentifier> m_hash;
};
}  // namespace Kernel
}  // namespace OpenViBE
