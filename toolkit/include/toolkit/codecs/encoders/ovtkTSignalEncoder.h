#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TSignalEncoderLocal : public T
	{
	protected:
		//The signal stream is a streamed matrix plus a sampling rate
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_iSampling;

		using T::m_codec;
		using T::m_boxAlgorithm;
		using T::m_oBuffer;
		using T::m_iMatrix;

		bool initializeImpl()
		{
			m_codec = &m_boxAlgorithm->getAlgorithmManager().getAlgorithm(m_boxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_SignalStreamEncoder));
			m_codec->initialize();
			m_iMatrix.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Matrix));
			m_iSampling.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_SignalStreamEncoder_InputParameterId_Sampling));
			m_oBuffer.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		//again... we propagate initialize from upperclass.
		using T::initialize;

		bool uninitialize()
		{
			if (m_boxAlgorithm == nullptr || m_codec == nullptr) { return false; }

			m_iMatrix.uninitialize();
			m_iSampling.uninitialize();
			m_oBuffer.uninitialize();
			m_codec->uninitialize();
			m_boxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_codec);
			m_boxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSamplingRate() { return m_iSampling; }

	protected:

		/*
		The methods specific to the Signal encoder (overriding the TStreamedMatrixEncoderLocal implementations):
		*/
		bool encodeHeaderImpl() { return m_codec->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeHeader); }
		bool encodeBufferImpl() { return m_codec->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeBuffer); }
		bool encodeEndImpl() { return m_codec->process(OVP_GD_Algorithm_SignalStreamEncoder_InputTriggerId_EncodeEnd); }
	};

	/*
	The Signal encoder can be instanciated easily through this class.
	You just need one template class : the box (T).
	*/
	template <class T>
	class TSignalEncoder : public TSignalEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>
	{
		using TSignalEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::m_boxAlgorithm;
	public:
		using TSignalEncoderLocal<TStreamedMatrixEncoderLocal<TEncoder<T>>>::uninitialize;

		TSignalEncoder() { }

		TSignalEncoder(T& boxAlgorithm, size_t index)
		{
			m_boxAlgorithm = NULL;
			this->initialize(boxAlgorithm, index);
		}

		virtual ~TSignalEncoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
