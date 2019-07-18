#ifndef __OpenViBEKernel_Kernel_CAlgorithmProxy_H__
#define __OpenViBEKernel_Kernel_CAlgorithmProxy_H__

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CAlgorithmProxy : public TKernelObject<IAlgorithmProxy>
		{
		public:

			CAlgorithmProxy(const IKernelContext& rKernelContext, Plugins::IAlgorithm& rAlgorithm, const Plugins::IAlgorithmDesc& rAlgorithmDesc);
			virtual ~CAlgorithmProxy(void);

			virtual Plugins::IAlgorithm& getAlgorithm(void);
			virtual const Plugins::IAlgorithm& getAlgorithm(void) const;
			virtual const Plugins::IAlgorithmDesc& getAlgorithmDesc(void) const;

			virtual bool addInputParameter(
				const CIdentifier& rInputParameterIdentifier,
				const CString& sInputName,
				EParameterType eParameterType,
				const CIdentifier& rSubTypeIdentifier);
			virtual CIdentifier getNextInputParameterIdentifier(
				const CIdentifier& rPreviousInputParameterIdentifier) const;
			virtual IParameter* getInputParameter(
				const CIdentifier& rInputParameterIdentifier);
			virtual EParameterType getInputParameterType(
				const CIdentifier& rInputParameterIdentifier) const;
			virtual CString getInputParameterName(
				const CIdentifier& rInputParameterIdentifier) const;
			virtual bool removeInputParameter(
				const CIdentifier& rInputParameterIdentifier);

			virtual bool addOutputParameter(
				const CIdentifier& rOutputParameterIdentifier,
				const CString& sOutputName,
				EParameterType eParameterType,
				const CIdentifier& rSubTypeIdentifier);
			virtual CIdentifier getNextOutputParameterIdentifier(
				const CIdentifier& rPreviousOutputParameterIdentifier) const;
			virtual IParameter* getOutputParameter(
				const CIdentifier& rOutputParameterIdentifier);
			virtual EParameterType getOutputParameterType(
				const CIdentifier& rOutputParameterIdentifier) const;
			virtual CString getOutputParameterName(
				const CIdentifier& rOutputParameterIdentifier) const;
			virtual bool removeOutputParameter(
				const CIdentifier& rOutputParameterIdentifier);

			virtual bool addInputTrigger(
				const CIdentifier& rInputTriggerIdentifier,
				const CString& rInputTriggerName);
			virtual CIdentifier getNextInputTriggerIdentifier(
				const CIdentifier& rPreviousInputTriggerIdentifier) const;
			virtual CString getInputTriggerName(
				const CIdentifier& rInputTriggerIdentifier) const;
			virtual bool isInputTriggerActive(
				const CIdentifier& rInputTriggerIdentifier) const;
			virtual bool activateInputTrigger(
				const CIdentifier& rInputTriggerIdentifier,
				bool bTriggerState);
			virtual bool removeInputTrigger(
				const CIdentifier& rInputTriggerIdentifier);

			virtual bool addOutputTrigger(
				const CIdentifier& rOutputTriggerIdentifier,
				const CString& rOutputTriggerName);
			virtual CIdentifier getNextOutputTriggerIdentifier(
				const CIdentifier& rPreviousOutputTriggerIdentifier) const;
			virtual CString getOutputTriggerName(
				const CIdentifier& rOutputTriggerIdentifier) const;
			virtual bool isOutputTriggerActive(
				const CIdentifier& rOutputTriggerIdentifier) const;
			virtual bool activateOutputTrigger(
				const CIdentifier& rOutputTriggerIdentifier,
				bool bTriggerState);
			virtual bool removeOutputTrigger(
				const CIdentifier& rOutputTriggerIdentifier);

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool process(void);
			virtual bool process(
				const CIdentifier& rTriggerIdentifier);

			virtual bool isAlgorithmDerivedFrom(
				const CIdentifier& rClassIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IAlgorithmProxy >, OVK_ClassId_Kernel_Algorithm_AlgorithmProxy)

		protected:

			IConfigurable* m_pInputConfigurable;
			IConfigurable* m_pOutputConfigurable;
			std::map<CIdentifier, CString> m_vInputParameterName;
			std::map<CIdentifier, CString> m_vOutputParameterName;
			std::map<CIdentifier, std::pair<CString, bool>> m_vInputTrigger;
			std::map<CIdentifier, std::pair<CString, bool>> m_vOutputTrigger;

		protected:

			void setAllInputTriggers(bool bTriggerStatus);
			void setAllOutputTriggers(bool bTriggerStatus);

		private:

			void handleException(const char* errorHint, const std::exception& exception);

			const Plugins::IAlgorithmDesc& m_rAlgorithmDesc;
			Plugins::IAlgorithm& m_rAlgorithm;
			bool m_bIsInitialized;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_CAlgorithm_H__
