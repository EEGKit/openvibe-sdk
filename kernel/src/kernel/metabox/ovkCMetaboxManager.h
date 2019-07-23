#pragma once

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>


namespace OpenViBE
{
	namespace Kernel
	{
		class CMetaboxManager : public TKernelObject<IMetaboxManager>
		{
		public:
			explicit CMetaboxManager(const IKernelContext& kernelContext);
			virtual ~CMetaboxManager();

			virtual bool addMetaboxesFromFiles(const CString& fileNameWildCard);
			virtual CIdentifier getNextMetaboxObjectDescIdentifier(const CIdentifier& previousIdentifier) const;

			virtual const Plugins::IPluginObjectDesc* getMetaboxObjectDesc(const CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxObjectDesc(const CIdentifier& metaboxIdentifier, Plugins::IPluginObjectDesc* metaboxDescriptor);

			virtual CString getMetaboxFilePath(const CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxFilePath(const CIdentifier& metaboxIdentifier, const CString& filePath);

			virtual CIdentifier getMetaboxHash(const CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxHash(const CIdentifier& metaboxIdentifier, const CIdentifier& hash);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IMetaboxManager, OVK_ClassId_Kernel_Metaboxes_MetaboxManager)

		protected:
			std::map<CIdentifier, const Plugins::IPluginObjectDesc*> m_MetaboxObjectDesc;
			std::map<CIdentifier, CString> m_MetaboxFilePath;
			std::map<CIdentifier, CIdentifier> m_MetaboxHash;
		};
	}  // namespace Kernel
}  // namespace OpenViBE



