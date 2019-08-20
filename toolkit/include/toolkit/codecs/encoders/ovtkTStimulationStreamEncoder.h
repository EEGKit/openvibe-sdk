#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TStimulationEncoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> m_pInputStimulationSet;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
			m_pCodec->initialize();
			m_pInputStimulationSet.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_pInputStimulationSet.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*>& getInputStimulationSet() { return m_pInputStimulationSet; }

	protected:
		bool encodeHeaderImpl() { return m_pCodec->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader); }

		bool encodeBufferImpl() { return m_pCodec->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer); }

		bool encodeEndImpl() { return m_pCodec->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd); }
	};

	template <class T>
	class TStimulationEncoder : public TStimulationEncoderLocal<TEncoder<T>>
	{
		using TStimulationEncoderLocal<TEncoder<T>>::m_pBoxAlgorithm;
	public:
		using TStimulationEncoderLocal<TEncoder<T>>::uninitialize;

		TStimulationEncoder() { }

		TStimulationEncoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TStimulationEncoder() { this->uninitialize(); }
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
