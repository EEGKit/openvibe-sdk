#pragma once

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModule : public TKernelObject<IPluginModule>
		{
		public:

			explicit CPluginModule(const IKernelContext& rKernelContext);
			virtual ~CPluginModule();

			virtual bool load(const CString& sName, CString* pError);
			virtual bool unload(CString* pError);
			virtual bool getFileName(CString& rFileName) const;

			virtual bool initialize();
			virtual bool getPluginObjectDescription(uint32_t ui32Index, Plugins::IPluginObjectDesc*& rpPluginObjectDescription);
			virtual bool uninitialize();

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IPluginModule, OVK_ClassId_Kernel_Plugins_PluginModule)

		protected:

			IPluginModule* m_pImplementation;
		};
	}  // namespace Kernel
}  // namespace OpenViBE


