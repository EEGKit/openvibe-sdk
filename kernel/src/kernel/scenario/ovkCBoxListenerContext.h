#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CBoxListenerContext : public TKernelObject<IBoxListenerContext>
		{
		public:

			CBoxListenerContext(const IKernelContext& rKernelContext, IBox& rBox, uint32_t ui32Index) : TKernelObject<IBoxListenerContext>(rKernelContext), m_rBox(rBox), m_ui32Index(ui32Index) { }
			IAlgorithmManager& getAlgorithmManager() const override { return this->getKernelContext().getAlgorithmManager(); }
			IPlayerManager& getPlayerManager() const override { return this->getKernelContext().getPlayerManager(); }
			IPluginManager& getPluginManager() const override { return this->getKernelContext().getPluginManager(); }
			IMetaboxManager& getMetaboxManager() const override { return this->getKernelContext().getMetaboxManager(); }
			IScenarioManager& getScenarioManager() const override { return this->getKernelContext().getScenarioManager(); }
			ITypeManager& getTypeManager() const override { return this->getKernelContext().getTypeManager(); }
			ILogManager& getLogManager() const override { return this->getKernelContext().getLogManager(); }
			IErrorManager& getErrorManager() const override { return this->getKernelContext().getErrorManager(); }
			IConfigurationManager& getConfigurationManager() const override { return this->getKernelContext().getConfigurationManager(); }
			IBox& getBox() const override { return m_rBox; }

			IScenario& getScenario() const override
			{
				OV_FATAL("Getting scenario from box listener context is not yet implemented", ErrorType::NotImplemented, this->getKernelContext().getLogManager());
			}

			uint32_t getIndex() const override { return m_ui32Index; }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelObject, OVK_ClassId_Kernel_Scenario_BoxListenerContext)

		private:

			IBox& m_rBox;
			uint32_t m_ui32Index;
		};
	}  // namespace Kernel
}  // namespace OpenViBE


