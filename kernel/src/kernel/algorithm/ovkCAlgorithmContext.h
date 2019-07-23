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
			virtual ~CAlgorithmContext();

			virtual IConfigurationManager& getConfigurationManager() const;
			virtual IAlgorithmManager& getAlgorithmManager() const;
			virtual ILogManager& getLogManager() const;
			virtual IErrorManager& getErrorManager() const;
			virtual ITypeManager& getTypeManager() const;

			virtual CIdentifier getNextInputParameterIdentifier(const CIdentifier& rPreviousInputParameterIdentifier) const;
			virtual IParameter* getInputParameter(const CIdentifier& rInputParameterIdentifier);

			virtual CIdentifier getNextOutputParameterIdentifier(const CIdentifier& rPreviousOutputParameterIdentifier) const;
			virtual IParameter* getOutputParameter(const CIdentifier& rOutputParameterIdentifier);

			virtual bool isInputTriggerActive(const CIdentifier& rInputTriggerIdentifier) const;

			virtual bool activateOutputTrigger(const CIdentifier& rOutputTriggerIdentifier, bool bTriggerState);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmContext >, OVK_ClassId_Kernel_Algorithm_AlgorithmContext)

		protected:

			ILogManager& m_rLogManager;
			CAlgorithmProxy& m_rAlgorithmProxy;
		};
	}  // namespace Kernel
}  // namespace OpenViBE


