#pragma once

#include "../ovkTKernelObject.h"
#include "ovkCAlgorithmProxy.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmContext final : public TKernelObject<IAlgorithmContext>
		{
		public:

			CAlgorithmContext(const IKernelContext& ctx, CAlgorithmProxy& algorithmProxy, const Plugins::IPluginObjectDesc& /*pluginObjectDesc*/)
				: TKernelObject<IAlgorithmContext>(ctx), m_rLogManager(ctx.getLogManager()), m_rAlgorithmProxy(algorithmProxy) {}

			~CAlgorithmContext() override { }
			IConfigurationManager& getConfigurationManager() const override { return getKernelContext().getConfigurationManager(); }
			IAlgorithmManager& getAlgorithmManager() const override { return getKernelContext().getAlgorithmManager(); }
			ILogManager& getLogManager() const override { return m_rLogManager; }
			IErrorManager& getErrorManager() const override { return getKernelContext().getErrorManager(); }
			ITypeManager& getTypeManager() const override { return getKernelContext().getTypeManager(); }

			CIdentifier getNextInputParameterIdentifier(const CIdentifier& previous) const override
			{
				return m_rAlgorithmProxy.getNextInputParameterIdentifier(previous);
			}

			IParameter* getInputParameter(const CIdentifier& identifier) override { return m_rAlgorithmProxy.getInputParameter(identifier); }

			CIdentifier getNextOutputParameterIdentifier(const CIdentifier& previous) const override
			{
				return m_rAlgorithmProxy.getNextOutputParameterIdentifier(previous);
			}

			IParameter* getOutputParameter(const CIdentifier& identifier) override { return m_rAlgorithmProxy.getOutputParameter(identifier); }
			bool isInputTriggerActive(const CIdentifier& identifier) const override { return m_rAlgorithmProxy.isInputTriggerActive(identifier); }

			bool activateOutputTrigger(const CIdentifier& identifier, bool state) override
			{
				return m_rAlgorithmProxy.activateOutputTrigger(identifier, state);
			}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmContext >, OVK_ClassId_Kernel_Algorithm_AlgorithmContext)

		protected:

			ILogManager& m_rLogManager;
			CAlgorithmProxy& m_rAlgorithmProxy;
		};
	} // namespace Kernel
} // namespace OpenViBE
