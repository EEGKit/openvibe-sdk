#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TChannelUnitsDecoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<bool> m_bOutputDynamic;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;
		using T::m_pOutputMatrix;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ChannelUnitsStreamDecoder));
			m_pCodec->initialize();
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ChannelUnitsStreamDecoder_InputParameterId_MemoryBufferToDecode));
			m_pOutputMatrix.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ChannelUnitsStreamDecoder_OutputParameterId_Matrix));
			m_bOutputDynamic.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ChannelUnitsStreamDecoder_OutputParameterId_Dynamic));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_bOutputDynamic.uninitialize();
			m_pOutputMatrix.uninitialize();

			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<bool>& getOutputDynamic()
		{
			return m_bOutputDynamic;
		}

		virtual bool isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ChannelUnitsStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual bool isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ChannelUnitsStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual bool isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ChannelUnitsStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TChannelUnitsDecoder : public TChannelUnitsDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>
	{
	private:
		using TChannelUnitsDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::m_pBoxAlgorithm;
	public:
		using TChannelUnitsDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::uninitialize;

		TChannelUnitsDecoder() { }

		TChannelUnitsDecoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TChannelUnitsDecoder()
		{
			this->uninitialize();
		}
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
