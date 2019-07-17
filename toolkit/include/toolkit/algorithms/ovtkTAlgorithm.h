#ifndef __OpenViBEToolkit_TAlgorithm_H__
#define __OpenViBEToolkit_TAlgorithm_H__

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	template <class CAlgorithmParentClass>
	class TAlgorithm : public CAlgorithmParentClass
	{
	public:

		TAlgorithm(void)
			: m_pAlgorithmContext(NULL) { }

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

		virtual bool initialize(void) { return true; }
		virtual bool uninitialize(void) { return true; }
		virtual bool process(void) =0;

		// ====================================================================================================================================

		virtual OpenViBE::Kernel::IAlgorithmContext& getAlgorithmContext(void)
		{
			return *m_pAlgorithmContext; // should never be null
		}

	protected:

		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void)
		{
			return m_pAlgorithmContext->getConfigurationManager(); // should never be null
		}

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void)
		{
			return m_pAlgorithmContext->getAlgorithmManager(); // should never be null
		}

		virtual OpenViBE::Kernel::ILogManager& getLogManager(void)
		{
			return m_pAlgorithmContext->getLogManager(); // should never be null
		}

		virtual OpenViBE::Kernel::IErrorManager& getErrorManager(void)
		{
			return m_pAlgorithmContext->getErrorManager(); // should never be null
		}

		virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void)
		{
			return m_pAlgorithmContext->getTypeManager(); // should never be null
		}

		virtual OpenViBE::CIdentifier getNextInputParameterIdentifier(
			const OpenViBE::CIdentifier& rPreviousInputParameterIdentifier) const
		{
			return m_pAlgorithmContext->getNextInputParameterIdentifier(rPreviousInputParameterIdentifier);
		}

		virtual OpenViBE::Kernel::IParameter* getInputParameter(
			const OpenViBE::CIdentifier& rInputParameterIdentifier)
		{
			return m_pAlgorithmContext->getInputParameter(rInputParameterIdentifier);
		}

		virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(
			const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const
		{
			return m_pAlgorithmContext->getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
		}

		virtual OpenViBE::Kernel::IParameter* getOutputParameter(
			const OpenViBE::CIdentifier& rOutputParameterIdentifier)
		{
			return m_pAlgorithmContext->getOutputParameter(rOutputParameterIdentifier);
		}

		virtual bool isInputTriggerActive(
			const OpenViBE::CIdentifier& rInputTriggerIdentifier) const
		{
			return m_pAlgorithmContext->isInputTriggerActive(rInputTriggerIdentifier);
		}

		virtual bool activateOutputTrigger(
			const OpenViBE::CIdentifier& rOutputTriggerIdentifier,
			const bool bTriggerState)
		{
			return m_pAlgorithmContext->activateOutputTrigger(rOutputTriggerIdentifier, bTriggerState);
		}

		// ====================================================================================================================================

		_IsDerivedFromClass_(CAlgorithmParentClass, OVTK_ClassId_);

	private:

		class CScopedAlgorithm
		{
		public:

			CScopedAlgorithm(OpenViBE::Kernel::IAlgorithmContext*& rpAlgorithmContext, OpenViBE::Kernel::IAlgorithmContext* pAlgorithmContext)
				: m_rpAlgorithmContext(rpAlgorithmContext)
			{
				m_rpAlgorithmContext = pAlgorithmContext;
			}

			virtual ~CScopedAlgorithm(void)
			{
				m_rpAlgorithmContext = NULL;
			}

		protected:

			OpenViBE::Kernel::IAlgorithmContext*& m_rpAlgorithmContext;
		};

		OpenViBE::Kernel::IAlgorithmContext* m_pAlgorithmContext;
	};
};

#endif // __OpenViBEToolkit_TAlgorithm_H__
