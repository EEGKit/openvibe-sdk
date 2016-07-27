#ifndef __OpenViBEKernel_Kernel_Scenario_CBoxListenerContext_H__
#define __OpenViBEKernel_Kernel_Scenario_CBoxListenerContext_H__

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CBoxListenerContext : public OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IBoxListenerContext >
		{
		public:

			CBoxListenerContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::IBox& rBox, OpenViBE::uint32 ui32Index) :
					TKernelObject<IBoxListenerContext>(rKernelContext),
					m_rBox(rBox),
					m_ui32Index(ui32Index)
			{
			}

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const { return this->getKernelContext().getAlgorithmManager(); }
			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const { return this->getKernelContext().getPlayerManager(); }
			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const { return this->getKernelContext().getPluginManager(); }
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const { return this->getKernelContext().getScenarioManager(); }
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const { return this->getKernelContext().getTypeManager(); }
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const { return this->getKernelContext().getLogManager(); }
			virtual OpenViBE::Kernel::IErrorManager& getErrorManager(void) const { return this->getKernelContext().getErrorManager(); }
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const { return this->getKernelContext().getConfigurationManager(); }

			virtual OpenViBE::Kernel::IBox& getBox(void) const { return m_rBox; }
			virtual OpenViBE::Kernel::IScenario& getScenario(void) const
			{
				OV_FATAL(
					"Getting scenario from box listener context is not yet implemented",
					ErrorType::NotImplemented,
					this->getKernelContext().getLogManager()
				);

				//return *((IScenario*)NULL);
			}
			virtual OpenViBE::uint32 getIndex(void) const { return m_ui32Index; }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelObject, OVK_ClassId_Kernel_Scenario_BoxListenerContext)

		private:

			OpenViBE::Kernel::IBox& m_rBox;
			OpenViBE::uint32 m_ui32Index;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CBoxListenerContext_H__
