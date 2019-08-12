#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TFeatureVectorDecoderLocal : public T
	{
	protected:

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;
		using T::m_pOutputMatrix;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamDecoder));
			m_pCodec->initialize();
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_FeatureVectorStreamDecoder_InputParameterId_MemoryBufferToDecode));
			m_pOutputMatrix.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputParameterId_Matrix));

			return true;
		}

	public:
		using T::initialize;
		using T::uninitialize;

		virtual bool isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual bool isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual bool isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_FeatureVectorStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TFeatureVectorDecoder : public TFeatureVectorDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>
	{
		using TFeatureVectorDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::m_pBoxAlgorithm;
	public:
		using TFeatureVectorDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::uninitialize;

		TFeatureVectorDecoder() { }

		TFeatureVectorDecoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TFeatureVectorDecoder() { this->uninitialize(); }
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
