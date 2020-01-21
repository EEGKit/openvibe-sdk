#pragma once

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	template <class TAlgorithmParentClass>
	class TAlgorithm : public TAlgorithmParentClass
	{
	public:

		TAlgorithm() { }

		bool initialize(OpenViBE::Kernel::IAlgorithmContext& algorithmCtx) override
		{
			CScopedAlgorithm scopedAlgorithm(m_algorithmCtx, &algorithmCtx);
			return initialize();
		}

		bool uninitialize(OpenViBE::Kernel::IAlgorithmContext& algorithmCtx) override
		{
			CScopedAlgorithm scopedAlgorithm(m_algorithmCtx, &algorithmCtx);
			return uninitialize();
		}

		bool process(OpenViBE::Kernel::IAlgorithmContext& algorithmCtx) override
		{
			CScopedAlgorithm scopedAlgorithm(m_algorithmCtx, &algorithmCtx);
			return process();
		}

		// ====================================================================================================================================

		virtual bool initialize() { return true; }
		virtual bool uninitialize() { return true; }
		virtual bool process() = 0;

		// ====================================================================================================================================

		virtual OpenViBE::Kernel::IAlgorithmContext& getAlgorithmContext()
		{
			return *m_algorithmCtx; // should never be null
		}

	protected:

		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager()
		{
			return m_algorithmCtx->getConfigurationManager(); // should never be null
		}

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager()
		{
			return m_algorithmCtx->getAlgorithmManager(); // should never be null
		}

		virtual OpenViBE::Kernel::ILogManager& getLogManager()
		{
			return m_algorithmCtx->getLogManager(); // should never be null
		}

		virtual OpenViBE::Kernel::IErrorManager& getErrorManager()
		{
			return m_algorithmCtx->getErrorManager(); // should never be null
		}

		virtual OpenViBE::Kernel::ITypeManager& getTypeManager()
		{
			return m_algorithmCtx->getTypeManager(); // should never be null
		}

		virtual OpenViBE::CIdentifier getNextInputParameterIdentifier(const OpenViBE::CIdentifier& rPreviousInputParameterIdentifier) const
		{
			return m_algorithmCtx->getNextInputParameterIdentifier(rPreviousInputParameterIdentifier);
		}

		virtual OpenViBE::Kernel::IParameter* getInputParameter(const OpenViBE::CIdentifier& InputParameterID)
		{
			return m_algorithmCtx->getInputParameter(InputParameterID);
		}

		virtual OpenViBE::CIdentifier getNextOutputParameterIdentifier(const OpenViBE::CIdentifier& rPreviousOutputParameterIdentifier) const
		{
			return m_algorithmCtx->getNextOutputParameterIdentifier(rPreviousOutputParameterIdentifier);
		}

		virtual OpenViBE::Kernel::IParameter* getOutputParameter(const OpenViBE::CIdentifier& outputParameterID)
		{
			return m_algorithmCtx->getOutputParameter(outputParameterID);
		}

		virtual bool isInputTriggerActive(const OpenViBE::CIdentifier& inputTriggerID) const { return m_algorithmCtx->isInputTriggerActive(inputTriggerID); }

		virtual bool activateOutputTrigger(const OpenViBE::CIdentifier& outputTriggerID, const bool bTriggerState)
		{
			return m_algorithmCtx->activateOutputTrigger(outputTriggerID, bTriggerState);
		}

		// ====================================================================================================================================

		_IsDerivedFromClass_(TAlgorithmParentClass, OVTK_ClassId_)

	private:

		class CScopedAlgorithm final
		{
		public:

			CScopedAlgorithm(OpenViBE::Kernel::IAlgorithmContext*& algorithmCtxRef, OpenViBE::Kernel::IAlgorithmContext* algorithmCtx)
				: m_algorithmCtx(algorithmCtxRef) { m_algorithmCtx = algorithmCtx; }

			~CScopedAlgorithm() { m_algorithmCtx = nullptr; }

		protected:

			OpenViBE::Kernel::IAlgorithmContext*& m_algorithmCtx;
		};

		OpenViBE::Kernel::IAlgorithmContext* m_algorithmCtx = nullptr;
	};
} // namespace OpenViBEToolkit
