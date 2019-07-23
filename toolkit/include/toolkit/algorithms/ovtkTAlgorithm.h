#pragma once

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	template <class CAlgorithmParentClass>
	class TAlgorithm : public CAlgorithmParentClass
	{
	public:

		TAlgorithm() : m_pAlgorithmContext(NULL) { }

		virtual bool initialize(OpenViBE::Kernel::IAlgorithmContext& rAlgorithmContext)
		{
			CScopedAlgorithm l_oScopedAlgorithm(m_pAlgorithmContext, &rAlgorithmContext);
			return initialize();
		}

		virtual bool uninitialize(OpenViBE::Kernel::IAlgorithmContext& rAlgorithmContext)
		{
			CScopedAlgorithm l_oScopedAlgorithm(m_pAlgorithmContext, &rAlgorithmContext);
			return uninitialize();
		}

		virtual bool process(OpenViBE::Kernel::IAlgorithmContext& rAlgorithmContext)
		{
			CScopedAlgorithm l_oScopedAlgorithm(m_pAlgorithmContext, &rAlgorithmContext);
			return process();
		}

		// ====================================================================================================================================

	public:

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

		virtual OpenViBE::Kernel::IParameter* getInputParameter(const OpenViBE::CIdentifier& rInputParameterIdentifier)
		{
			return m_pAlgorithmContext->getInputParameter(rInputParameterIdentifier);
		}

		virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const
		{
			return m_pAlgorithmContext->getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
		}

		virtual OpenViBE::Kernel::IParameter* getOutputParameter(const OpenViBE::CIdentifier& rOutputParameterIdentifier)
		{
			return m_pAlgorithmContext->getOutputParameter(rOutputParameterIdentifier);
		}

		virtual bool isInputTriggerActive(const OpenViBE::CIdentifier& rInputTriggerIdentifier) const
		{
			return m_pAlgorithmContext->isInputTriggerActive(rInputTriggerIdentifier);
		}

		virtual bool activateOutputTrigger(const OpenViBE::CIdentifier& rOutputTriggerIdentifier, const bool bTriggerState)
		{
			return m_pAlgorithmContext->activateOutputTrigger(rOutputTriggerIdentifier, bTriggerState);
		}

		// ====================================================================================================================================

		_IsDerivedFromClass_(CAlgorithmParentClass, OVTK_ClassId_)

	private:

		class CScopedAlgorithm
		{
		public:

			CScopedAlgorithm(OpenViBE::Kernel::IAlgorithmContext*& rpAlgorithmContext, OpenViBE::Kernel::IAlgorithmContext* pAlgorithmContext)
				: m_rpAlgorithmContext(rpAlgorithmContext)
			{
				m_rpAlgorithmContext = pAlgorithmContext;
			}

			virtual ~CScopedAlgorithm() { m_rpAlgorithmContext = NULL; }

		protected:

			OpenViBE::Kernel::IAlgorithmContext*& m_rpAlgorithmContext;
		};

		OpenViBE::Kernel::IAlgorithmContext* m_pAlgorithmContext;
	};
}  // namespace OpenViBEToolkit
