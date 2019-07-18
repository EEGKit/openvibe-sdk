#ifndef __OpenViBEKernel_Kernel_Scenario_CScenarioImporterContext_H__
#define __OpenViBEKernel_Kernel_Scenario_CScenarioImporterContext_H__

#include "../ovkTKernelObject.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenarioImporterContext : public TKernelObject<Kernel::IScenarioImporterContext>
		{
		public:

			CScenarioImporterContext(
				const IKernelContext& rKernelContext,
				const CString& sFileName,
				IScenario& rScenario);

			virtual CString getFileName(void) const;
			virtual IScenario& getScenario(void) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioImporterContext>, OVK_ClassId_Kernel_Scenario_ScenarioImporterContext);

		protected:

			const CString m_sFileName;
			IScenario& m_rScenario;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CScenarioImporterContext_H__
