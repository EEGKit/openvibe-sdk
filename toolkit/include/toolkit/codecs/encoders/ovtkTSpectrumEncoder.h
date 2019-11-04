#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"
#include "ovtkTStreamedMatrixEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TSpectrumEncoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> m_pInputFrequencyAbscissa;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_pInputSamplingRate;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;
		using T::m_pInputMatrix;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(
				m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Matrix));
			m_pInputFrequencyAbscissa.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa));
			m_pInputSamplingRate.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_InputParameterId_Sampling));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamEncoder_OutputParameterId_EncodedMemoryBuffer));


			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_pInputMatrix.uninitialize();
			m_pInputFrequencyAbscissa.uninitialize();
			m_pInputSamplingRate.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSamplingRate() { return m_pInputSamplingRate; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*>& getInputFrequencyAbscissa() { return m_pInputFrequencyAbscissa; }

		size_t getInputFrequencyAbscissaCount() { return m_pInputFrequencyAbscissa->getDimensionSize(0); }


	protected:
		bool encodeHeaderImpl() { return m_pCodec->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeHeader); }

		bool encodeBufferImpl() { return m_pCodec->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeBuffer); }

		bool encodeEndImpl() { return m_pCodec->process(OVP_GD_Algorithm_SpectrumStreamEncoder_InputTriggerId_EncodeEnd); }
	};

	template <class T>
	class TSpectrumEncoder : public TSpectrumEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>
	{
		using TSpectrumEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::m_pBoxAlgorithm;
	public:
		using TSpectrumEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::uninitialize;

		TSpectrumEncoder() { }

		TSpectrumEncoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TSpectrumEncoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
