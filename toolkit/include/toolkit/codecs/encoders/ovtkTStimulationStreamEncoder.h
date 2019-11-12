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

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> m_iStimulationSet;

		using T::m_codec;
		using T::m_boxAlgorithm;
		using T::m_oBuffer;

		bool initializeImpl()
		{
			m_codec = &m_boxAlgorithm->getAlgorithmManager().getAlgorithm(m_boxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
			m_codec->initialize();
			m_iStimulationSet.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
			m_oBuffer.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_boxAlgorithm == nullptr || m_codec == nullptr) { return false; }

			m_iStimulationSet.uninitialize();
			m_oBuffer.uninitialize();
			m_codec->uninitialize();
			m_boxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_codec);
			m_boxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*>& getInputStimulationSet() { return m_iStimulationSet; }

	protected:
		bool encodeHeaderImpl() { return m_codec->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader); }
		bool encodeBufferImpl() { return m_codec->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer); }
		bool encodeEndImpl() { return m_codec->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeEnd); }
	};

	template <class T>
	class TStimulationEncoder : public TStimulationEncoderLocal<TEncoder<T>>
	{
		using TStimulationEncoderLocal<TEncoder<T>>::m_boxAlgorithm;
	public:
		using TStimulationEncoderLocal<TEncoder<T>>::uninitialize;

		TStimulationEncoder() { }

		TStimulationEncoder(T& boxAlgorithm, size_t index)
		{
			m_boxAlgorithm = NULL;
			this->initialize(boxAlgorithm, index);
		}

		virtual ~TStimulationEncoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
