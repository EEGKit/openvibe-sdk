#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TFeatureVectorEncoderLocal : public T
	{
	protected:
		// the feature vector stream is just a streamed matrix with some constraint (dimension = 2).
		// no specific parameter.

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;
		using T::m_pInputMatrix;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_FeatureVectorStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputParameterId_Matrix));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_FeatureVectorStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	protected:

		bool encodeHeaderImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeHeader);
		}

		bool encodeBufferImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		bool encodeEndImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_FeatureVectorStreamEncoder_InputTriggerId_EncodeEnd);
		}

	public:
		using T::initialize;
		using T::uninitialize;
	};

	template <class T>
	class TFeatureVectorEncoder : public TFeatureVectorEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>
	{
	private:
		using TFeatureVectorEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::m_pBoxAlgorithm;
	public:
		using TFeatureVectorEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::uninitialize;

		TFeatureVectorEncoder() { }

		TFeatureVectorEncoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TFeatureVectorEncoder()
		{
			this->uninitialize();
		}
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
