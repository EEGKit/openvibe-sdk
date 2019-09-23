#pragma once

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginModule final : public TKernelObject<IPluginModule>
		{
		public:

			explicit CPluginModule(const IKernelContext& ctx);
			~CPluginModule() override;
			bool load(const CString& filename, CString* pError) override;
			bool unload(CString* pError) override;
			bool getFileName(CString& rFileName) const override;
			bool initialize() override;
			bool getPluginObjectDescription(uint32_t index, Plugins::IPluginObjectDesc*& rpPluginObjectDescription) override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(TKernelObject<IPluginModule>, OVK_ClassId_Kernel_Plugins_PluginModule)

		protected:

			IPluginModule* m_pImplementation = nullptr;
		};
	} // namespace Kernel
} // namespace OpenViBE
