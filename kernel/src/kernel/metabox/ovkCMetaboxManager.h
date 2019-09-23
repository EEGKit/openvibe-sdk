#pragma once

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>


namespace OpenViBE
{
	namespace Kernel
	{
		class CMetaboxManager final : public TKernelObject<IMetaboxManager>
		{
		public:
			explicit CMetaboxManager(const IKernelContext& ctx);
			~CMetaboxManager() override;
			bool addMetaboxesFromFiles(const CString& fileNameWildCard) override;
			CIdentifier getNextMetaboxObjectDescIdentifier(const CIdentifier& previousID) const override;
			const Plugins::IPluginObjectDesc* getMetaboxObjectDesc(const CIdentifier& metaboxIdentifier) const override;
			void setMetaboxObjectDesc(const CIdentifier& metaboxIdentifier, Plugins::IPluginObjectDesc* metaboxDescriptor) override;
			CString getMetaboxFilePath(const CIdentifier& metaboxIdentifier) const override;
			void setMetaboxFilePath(const CIdentifier& metaboxIdentifier, const CString& filePath) override;
			CIdentifier getMetaboxHash(const CIdentifier& metaboxIdentifier) const override;
			void setMetaboxHash(const CIdentifier& metaboxIdentifier, const CIdentifier& hash) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IMetaboxManager, OVK_ClassId_Kernel_Metaboxes_MetaboxManager)

		protected:
			std::map<CIdentifier, const Plugins::IPluginObjectDesc*> m_MetaboxObjectDesc;
			std::map<CIdentifier, CString> m_MetaboxFilePath;
			std::map<CIdentifier, CIdentifier> m_MetaboxHash;
		};
	} // namespace Kernel
} // namespace OpenViBE
