#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TSignalDecoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_pOutputSamplingRate;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;
		using T::m_pOutputMatrix;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamDecoder));
			m_pCodec->initialize();
			m_pOutputSamplingRate.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));
			m_pOutputMatrix.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_SignalStreamDecoder_OutputParameterId_Matrix));
			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_SignalStreamDecoder_InputParameterId_MemoryBufferToDecode));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_pOutputSamplingRate.uninitialize();
			m_pOutputMatrix.uninitialize();
			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSamplingRate() { return m_pOutputSamplingRate; }

		virtual bool isHeaderReceived() { return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedHeader); }

		virtual bool isBufferReceived() { return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedBuffer); }

		virtual bool isEndReceived() { return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_SignalStreamDecoder_OutputTriggerId_ReceivedEnd); }
	};

	template <class T>
	class TSignalDecoder : public TSignalDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>
	{
		using TSignalDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::m_pBoxAlgorithm;
	public:
		using TSignalDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::uninitialize;

		TSignalDecoder() { }

		TSignalDecoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TSignalDecoder() { this->uninitialize(); }
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
