#pragma once

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	template <class CAlgorithmParentClass>
	class TAlgorithm : public CAlgorithmParentClass
	{
	public:

		TAlgorithm() { }

		bool initialize(OpenViBE::Kernel::IAlgorithmContext& algorithmCtx) override
		{
			CScopedAlgorithm scopedAlgorithm(m_pAlgorithmContext, &algorithmCtx);
			return initialize();
		}

		bool uninitialize(OpenViBE::Kernel::IAlgorithmContext& algorithmCtx) override
		{
			CScopedAlgorithm scopedAlgorithm(m_pAlgorithmContext, &algorithmCtx);
			return uninitialize();
		}

		bool process(OpenViBE::Kernel::IAlgorithmContext& algorithmCtx) override
		{
			CScopedAlgorithm scopedAlgorithm(m_pAlgorithmContext, &algorithmCtx);
			return process();
		}

		// ====================================================================================================================================

		virtual bool initialize() { return true; }
		virtual bool uninitialize() { return true; }
		virtual bool process() = 0;

		// ====================================================================================================================================

		virtual OpenViBE::Kernel::IAlgorithmContext& getAlgorithmContext()
		{
			return *m_pAlgorithmContext; // should never be null
		}

	protected:

		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager()
		{
			return m_pAlgorithmContext->getConfigurationManager(); // should never be null
		}

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager()
		{
			return m_pAlgorithmContext->getAlgorithmManager(); // should never be null
		}

		virtual OpenViBE::Kernel::ILogManager& getLogManager()
		{
			return m_pAlgorithmContext->getLogManager(); // should never be null
		}

		virtual OpenViBE::Kernel::IErrorManager& getErrorManager()
		{
			return m_pAlgorithmContext->getErrorManager(); // should never be null
		}

		virtual OpenViBE::Kernel::ITypeManager& getTypeManager()
		{
			return m_pAlgorithmContext->getTypeManager(); // should never be null
		}

		virtual OpenViBE::CIdentifier getNextInputParameterIdentifier(const OpenViBE::CIdentifier& rPreviousInputParameterIdentifier) const
		{
			return m_pAlgorithmContext->getNextInputParameterIdentifier(rPreviousInputParameterIdentifier);
		}

		virtual OpenViBE::Kernel::IParameter* getInputParameter(const OpenViBE::CIdentifier& InputParameterID)
		{
			return m_pAlgorithmContext->getInputParameter(InputParameterID);
		}

		virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const
		{
			return m_pAlgorithmContext->getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
		}

		virtual OpenViBE::Kernel::IParameter* getOutputParameter(const OpenViBE::CIdentifier& outputParameterID)
		{
			return m_pAlgorithmContext->getOutputParameter(outputParameterID);
		}

		virtual bool isInputTriggerActive(const OpenViBE::CIdentifier& inputTriggerID) const
		{
			return m_pAlgorithmContext->isInputTriggerActive(inputTriggerID);
		}

		virtual bool activateOutputTrigger(const OpenViBE::CIdentifier& outputTriggerID, const bool bTriggerState)
		{
			return m_pAlgorithmContext->activateOutputTrigger(outputTriggerID, bTriggerState);
		}

		// ====================================================================================================================================

		_IsDerivedFromClass_(CAlgorithmParentClass, OVTK_ClassId_)

	private:

		class CScopedAlgorithm final
		{
		public:

			CScopedAlgorithm(OpenViBE::Kernel::IAlgorithmContext*& rpAlgorithmContext, OpenViBE::Kernel::IAlgorithmContext* pAlgorithmContext)
				: m_rpAlgorithmContext(rpAlgorithmContext) { m_rpAlgorithmContext = pAlgorithmContext; }

			~CScopedAlgorithm() { m_rpAlgorithmContext = nullptr; }

		protected:

			OpenViBE::Kernel::IAlgorithmContext*& m_rpAlgorithmContext;
		};

		OpenViBE::Kernel::IAlgorithmContext* m_pAlgorithmContext = nullptr;
	};
} // namespace OpenViBEToolkit
