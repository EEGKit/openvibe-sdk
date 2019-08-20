#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmProxy;

		class CAlgorithmContext : public TKernelObject<IAlgorithmContext>
		{
		public:

			CAlgorithmContext(const IKernelContext& rKernelContext, CAlgorithmProxy& rAlgorithmProxy, const Plugins::IPluginObjectDesc& rPluginObjectDesc);
			~CAlgorithmContext() override;
			IConfigurationManager& getConfigurationManager() const override;
			IAlgorithmManager& getAlgorithmManager() const override;
			ILogManager& getLogManager() const override;
			IErrorManager& getErrorManager() const override;
			ITypeManager& getTypeManager() const override;
			CIdentifier getNextInputParameterIdentifier(const CIdentifier& rPreviousInputParameterIdentifier) const override;
			IParameter* getInputParameter(const CIdentifier& rInputParameterIdentifier) override;
			CIdentifier getNextOutputParameterIdentifier(const CIdentifier& rPreviousOutputParameterIdentifier) const override;
			IParameter* getOutputParameter(const CIdentifier& rOutputParameterIdentifier) override;
			bool isInputTriggerActive(const CIdentifier& rInputTriggerIdentifier) const override;
			bool activateOutputTrigger(const CIdentifier& rOutputTriggerIdentifier, bool bTriggerState) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmContext >, OVK_ClassId_Kernel_Algorithm_AlgorithmContext)

		protected:

			ILogManager& m_rLogManager;
			CAlgorithmProxy& m_rAlgorithmProxy;
		};
	}  // namespace Kernel
}  // namespace OpenViBE
