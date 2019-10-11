#pragma once

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmProxy final : public TKernelObject<IAlgorithmProxy>
		{
		public:

			CAlgorithmProxy(const IKernelContext& ctx, Plugins::IAlgorithm& rAlgorithm, const Plugins::IAlgorithmDesc& rAlgorithmDesc);
			~CAlgorithmProxy() override;

			Plugins::IAlgorithm& getAlgorithm();
			const Plugins::IAlgorithm& getAlgorithm() const;
			const Plugins::IAlgorithmDesc& getAlgorithmDesc() const;

			bool addInputParameter(const CIdentifier& parameterID, const CString& name, EParameterType parameterType, const CIdentifier& subTypeID);
			CIdentifier getNextInputParameterIdentifier(const CIdentifier& parameterID) const override;
			IParameter* getInputParameter(const CIdentifier& parameterID) override;
			EParameterType getInputParameterType(const CIdentifier& parameterID) const;
			CString getInputParameterName(const CIdentifier& parameterID) const override;
			bool removeInputParameter(const CIdentifier& parameterID);

			bool addOutputParameter(const CIdentifier& parameterID, const CString& name, EParameterType parameterType, const CIdentifier& subTypeID);
			CIdentifier getNextOutputParameterIdentifier(const CIdentifier& parameterID) const override;
			IParameter* getOutputParameter(const CIdentifier& parameterID) override;
			EParameterType getOutputParameterType(const CIdentifier& parameterID) const;
			CString getOutputParameterName(const CIdentifier& parameterID) const override;
			bool removeOutputParameter(const CIdentifier& parameterID);

			bool addInputTrigger(const CIdentifier& triggerID, const CString& name);
			CIdentifier getNextInputTriggerIdentifier(const CIdentifier& triggerID) const override;
			CString getInputTriggerName(const CIdentifier& triggerID) const override;
			bool isInputTriggerActive(const CIdentifier& triggerID) const;
			bool activateInputTrigger(const CIdentifier& triggerID, bool triggerState) override;
			bool removeInputTrigger(const CIdentifier& triggerID);

			bool addOutputTrigger(const CIdentifier& triggerID, const CString& name);
			CIdentifier getNextOutputTriggerIdentifier(const CIdentifier& triggerID) const override;
			CString getOutputTriggerName(const CIdentifier& triggerID) const override;
			bool isOutputTriggerActive(const CIdentifier& triggerID) const override;
			bool activateOutputTrigger(const CIdentifier& triggerID, const bool triggerState);
			bool removeOutputTrigger(const CIdentifier& triggerID);
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;
			bool process(const CIdentifier& triggerID) override;
			bool isAlgorithmDerivedFrom(const CIdentifier& classID) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmProxy >, OVK_ClassId_Kernel_Algorithm_AlgorithmProxy)

		protected:

			IConfigurable* m_pInputConfigurable  = nullptr;
			IConfigurable* m_pOutputConfigurable = nullptr;
			std::map<CIdentifier, CString> m_vInputParameterName;
			std::map<CIdentifier, CString> m_vOutputParameterName;
			std::map<CIdentifier, std::pair<CString, bool>> m_vInputTrigger;
			std::map<CIdentifier, std::pair<CString, bool>> m_vOutputTrigger;

			void setAllInputTriggers(bool status);
			void setAllOutputTriggers(bool status);

		private:

			void handleException(const char* errorHint, const std::exception& exception);

			const Plugins::IAlgorithmDesc& m_rAlgorithmDesc;
			Plugins::IAlgorithm& m_rAlgorithm;
			bool m_isInitialized = false;
		};
	} // namespace Kernel
} // namespace OpenViBE
