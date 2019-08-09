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
			~CPluginModule() override;
			bool load(const CString& sName, CString* pError) override;
			bool unload(CString* pError) override;
			bool getFileName(CString& rFileName) const override;
			bool initialize() override;
			bool getPluginObjectDescription(uint32_t ui32Index, Plugins::IPluginObjectDesc*& rpPluginObjectDescription) override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IPluginModule, OVK_ClassId_Kernel_Plugins_PluginModule)

		protected:

			IPluginModule* m_pImplementation;
		};
	}  // namespace Kernel
}  // namespace OpenViBE


