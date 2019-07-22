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

			virtual IAlgorithmManager& getAlgorithmManager() const { return this->getKernelContext().getAlgorithmManager(); }
			virtual IPlayerManager& getPlayerManager() const { return this->getKernelContext().getPlayerManager(); }
			virtual IPluginManager& getPluginManager() const { return this->getKernelContext().getPluginManager(); }
			virtual IMetaboxManager& getMetaboxManager() const { return this->getKernelContext().getMetaboxManager(); }
			virtual IScenarioManager& getScenarioManager() const { return this->getKernelContext().getScenarioManager(); }
			virtual ITypeManager& getTypeManager() const { return this->getKernelContext().getTypeManager(); }
			virtual ILogManager& getLogManager() const { return this->getKernelContext().getLogManager(); }
			virtual IErrorManager& getErrorManager() const { return this->getKernelContext().getErrorManager(); }
			virtual IConfigurationManager& getConfigurationManager() const { return this->getKernelContext().getConfigurationManager(); }

			virtual IBox& getBox() const { return m_rBox; }

			virtual IScenario& getScenario() const
			{
				OV_FATAL("Getting scenario from box listener context is not yet implemented", ErrorType::NotImplemented, this->getKernelContext().getLogManager());
			}

			virtual uint32_t getIndex() const { return m_ui32Index; }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelObject, OVK_ClassId_Kernel_Scenario_BoxListenerContext)

		private:

			IBox& m_rBox;
			uint32_t m_ui32Index;
		};
	};
};


