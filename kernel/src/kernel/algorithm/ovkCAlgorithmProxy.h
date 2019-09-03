#pragma once

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmProxy : public TKernelObject<IAlgorithmProxy>
		{
		public:

			CAlgorithmProxy(const IKernelContext& ctx, Plugins::IAlgorithm& rAlgorithm, const Plugins::IAlgorithmDesc& rAlgorithmDesc);
			~CAlgorithmProxy() override;

			virtual Plugins::IAlgorithm& getAlgorithm();
			virtual const Plugins::IAlgorithm& getAlgorithm() const;
			virtual const Plugins::IAlgorithmDesc& getAlgorithmDesc() const;

			virtual bool addInputParameter(const CIdentifier& InputParameterID, const CString& sInputName, EParameterType eParameterType,
										   const CIdentifier& subTypeID);
			CIdentifier getNextInputParameterIdentifier(const CIdentifier& rPreviousInputParameterIdentifier) const override;
			IParameter* getInputParameter(const CIdentifier& InputParameterID) override;
			virtual EParameterType getInputParameterType(const CIdentifier& InputParameterID) const;
			CString getInputParameterName(const CIdentifier& InputParameterID) const override;
			virtual bool removeInputParameter(const CIdentifier& InputParameterID);

			virtual bool addOutputParameter(const CIdentifier& outputParameterID, const CString& sOutputName, EParameterType eParameterType,
											const CIdentifier& subTypeID);
			CIdentifier getNextOutputParameterIdentifier(const CIdentifier& rPreviousOutputParameterIdentifier) const override;
			IParameter* getOutputParameter(const CIdentifier& outputParameterID) override;
			virtual EParameterType getOutputParameterType(const CIdentifier& outputParameterID) const;
			CString getOutputParameterName(const CIdentifier& outputParameterID) const override;
			virtual bool removeOutputParameter(const CIdentifier& outputParameterID);

			virtual bool addInputTrigger(const CIdentifier& inputTriggerID, const CString& rInputTriggerName);
			CIdentifier getNextInputTriggerIdentifier(const CIdentifier& rPreviousInputTriggerIdentifier) const override;
			CString getInputTriggerName(const CIdentifier& inputTriggerID) const override;
			virtual bool isInputTriggerActive(const CIdentifier& inputTriggerID) const;
			bool activateInputTrigger(const CIdentifier& inputTriggerID, bool bTriggerState) override;
			virtual bool removeInputTrigger(const CIdentifier& inputTriggerID);

			virtual bool addOutputTrigger(const CIdentifier& outputTriggerID, const CString& rOutputTriggerName);
			CIdentifier getNextOutputTriggerIdentifier(const CIdentifier& rPreviousOutputTriggerIdentifier) const override;
			CString getOutputTriggerName(const CIdentifier& outputTriggerID) const override;
			bool isOutputTriggerActive(const CIdentifier& outputTriggerID) const override;
			virtual bool activateOutputTrigger(const CIdentifier& outputTriggerID, bool bTriggerState);
			virtual bool removeOutputTrigger(const CIdentifier& outputTriggerID);
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;
			bool process(const CIdentifier& rTriggerIdentifier) override;
			bool isAlgorithmDerivedFrom(const CIdentifier& rClassIdentifier) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmProxy >, OVK_ClassId_Kernel_Algorithm_AlgorithmProxy)

		protected:

			IConfigurable* m_pInputConfigurable  = nullptr;
			IConfigurable* m_pOutputConfigurable = nullptr;
			std::map<CIdentifier, CString> m_vInputParameterName;
			std::map<CIdentifier, CString> m_vOutputParameterName;
			std::map<CIdentifier, std::pair<CString, bool>> m_vInputTrigger;
			std::map<CIdentifier, std::pair<CString, bool>> m_vOutputTrigger;

			void setAllInputTriggers(bool bTriggerStatus);
			void setAllOutputTriggers(bool bTriggerStatus);

		private:

			void handleException(const char* errorHint, const std::exception& exception);

			const Plugins::IAlgorithmDesc& m_rAlgorithmDesc;
			Plugins::IAlgorithm& m_rAlgorithm;
			bool m_bIsInitialized = false;
		};
	} // namespace Kernel
} // namespace OpenViBE
