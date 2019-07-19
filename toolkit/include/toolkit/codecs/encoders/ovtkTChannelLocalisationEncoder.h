#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "ovtkTStreamedMatrixEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TChannelLocalisationEncoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<bool> m_bInputDynamic;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;
		using T::m_pInputMatrix;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ChannelLocalisationStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Matrix));
			m_bInputDynamic.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == NULL || m_pCodec == NULL) { return false; }

			m_pInputMatrix.uninitialize();
			m_bInputDynamic.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<bool>& getInputDynamic()
		{
			return m_bInputDynamic;
		}

	protected:
		bool encodeHeaderImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeHeader);
		}

		bool encodeBufferImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		bool encodeEndImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_ChannelLocalisationStreamEncoder_InputTriggerId_EncodeEnd);
		}
	};

	template <class T>
	class TChannelLocalisationEncoder : public TChannelLocalisationEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>
	{
	private:
		using TChannelLocalisationEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::m_pBoxAlgorithm;
	public:
		using TChannelLocalisationEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::uninitialize;

		TChannelLocalisationEncoder() { }

		TChannelLocalisationEncoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TChannelLocalisationEncoder()
		{
			this->uninitialize();
		}
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines


