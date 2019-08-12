#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TStreamStructureDecoderLocal : public T
	{
	protected:

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamStructureDecoder));
			m_pCodec->initialize();
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_StreamStructureDecoder_InputParameterId_MemoryBufferToDecode));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		virtual bool isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_StreamStructureDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual bool isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_StreamStructureDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual bool isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_StreamStructureDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TStreamStructureDecoder : public TStreamStructureDecoderLocal<TDecoder<T>>
	{
		using TStreamStructureDecoderLocal<TDecoder<T>>::m_pBoxAlgorithm;
	public:
		using TStreamStructureDecoderLocal<TDecoder<T>>::uninitialize;

		TStreamStructureDecoder() { }

		TStreamStructureDecoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TStreamStructureDecoder() { this->uninitialize(); }
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
