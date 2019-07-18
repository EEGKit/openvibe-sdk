#ifndef __OpenViBEKernel_Kernel_Plugins_CPluginModule_H__
#define __OpenViBEKernel_Kernel_Plugins_CPluginModule_H__

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
			virtual ~CPluginModule(void);

			virtual bool load(
				const CString& sName,
				CString* pError);
			virtual bool unload(
				CString* pError);
			virtual bool getFileName(
				CString& rFileName) const;

			virtual bool initialize(void);
			virtual bool getPluginObjectDescription(
				uint32_t ui32Index,
				Plugins::IPluginObjectDesc*& rpPluginObjectDescription);
			virtual bool uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IPluginModule, OVK_ClassId_Kernel_Plugins_PluginModule)

		protected:

			IPluginModule* m_pImplementation;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Plugins_CPluginModule_H__
