#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TSpectrumDecoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> m_pOutputFrequencyAbscissa;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_pOutputSamplingRate;


		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;
		using T::m_pOutputMatrix;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(
				m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SpectrumStreamDecoder));
			m_pCodec->initialize();
			m_pOutputMatrix.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Matrix));
			m_pOutputFrequencyAbscissa.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));
			m_pOutputSamplingRate.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputParameterId_Sampling));
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SpectrumStreamDecoder_InputParameterId_MemoryBufferToDecode));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_pOutputMatrix.uninitialize();
			m_pOutputFrequencyAbscissa.uninitialize();
			m_pOutputSamplingRate.uninitialize();
			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSamplingRate() { return m_pOutputSamplingRate; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*>& getOutputFrequencyAbscissa() { return m_pOutputFrequencyAbscissa; }

		virtual bool isHeaderReceived() { return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedHeader); }

		virtual bool isBufferReceived() { return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedBuffer); }

		virtual bool isEndReceived() { return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SpectrumStreamDecoder_OutputTriggerId_ReceivedEnd); }
	};

	template <class T>
	class TSpectrumDecoder : public TSpectrumDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>
	{
		using TSpectrumDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::m_pBoxAlgorithm;
	public:
		using TSpectrumDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::uninitialize;

		TSpectrumDecoder() { }

		TSpectrumDecoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TSpectrumDecoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
