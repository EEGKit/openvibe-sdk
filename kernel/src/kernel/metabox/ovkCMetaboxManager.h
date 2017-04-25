#ifndef __OpenViBEKernel_Kernel_Metabox_CMetaboxManager_H__
#define __OpenViBEKernel_Kernel_Metabox_CMetaboxManager_H__

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>

#define OVP_ScenarioImportContext_OnLoadMetaboxImport OpenViBE::CIdentifier(0xED48480A, 0x9F6DBAA7)

namespace OpenViBE
{
	namespace Kernel
	{
		class CMetaboxManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMetaboxManager>
		{
		public:
			explicit CMetaboxManager(const OpenViBE::Kernel::IKernelContext& kernelContext);
			virtual ~CMetaboxManager(void);

			virtual bool addMetaboxesFromFiles(const OpenViBE::CString& fileNameWildCard);
			virtual OpenViBE::CIdentifier getNextMetaboxObjectDescIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;

			virtual const OpenViBE::Plugins::IPluginObjectDesc* getMetaboxObjectDesc(const OpenViBE::CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxObjectDesc(const OpenViBE::CIdentifier& metaboxIdentifier, OpenViBE::Plugins::IPluginObjectDesc* metaboxDescriptor);

			virtual OpenViBE::CString getMetaboxFilePath(const OpenViBE::CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxFilePath(const OpenViBE::CIdentifier& metaboxIdentifier, const OpenViBE::CString& filePath);

			virtual OpenViBE::CIdentifier getMetaboxHash(const OpenViBE::CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxHash(const OpenViBE::CIdentifier& metaboxIdentifier, const OpenViBE::CIdentifier& hash);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IMetaboxManager, OVK_ClassId_Kernel_Metaboxes_MetaboxManager)

		protected:
			std::map<OpenViBE::CIdentifier, const OpenViBE::Plugins::IPluginObjectDesc*> m_MetaboxObjectDesc;
			std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_MetaboxFilePath;
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_MetaboxHash;
		};
	};
};



#endif // __OpenViBEKernel_Kernel_Metabox_CMetaboxManager_H__
