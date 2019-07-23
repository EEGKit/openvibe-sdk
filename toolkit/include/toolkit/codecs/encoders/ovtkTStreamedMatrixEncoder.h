#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TStreamedMatrixEncoderLocal : public T
	{
	protected:
		//specific attribute : a matrix handler
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> m_pInputMatrix;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder));
			m_pCodec->initialize();
			m_pInputMatrix.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
			m_pOutputMemoryBuffer.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		// we propagate the visiblity of TCodec::initialize
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == NULL || m_pCodec == NULL) { return false; }

			m_pInputMatrix.uninitialize();
			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*>& getInputMatrix()
		{
			return m_pInputMatrix;
		}

	protected:

		/*
		The methods specific to the Streamed Matrix encoder :
		*/
		bool encodeHeaderImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
		}

		bool encodeBufferImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeBuffer);
		}

		bool encodeEndImpl()
		{
			return m_pCodec->process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeEnd);
		}
	};

	/*
	This class provides an access to the Local class.
	It makes a lot easier the declaration of a Streamed Matrix encoder, as you don't have to specify any more template than the Box class (T).
	*/
	template <class T>
	class TStreamedMatrixEncoder : public TStreamedMatrixEncoderLocal<TEncoder<T>>
	{
	private:
		using TStreamedMatrixEncoderLocal<TEncoder<T>>::m_pBoxAlgorithm;
	public:
		using TStreamedMatrixEncoderLocal<TEncoder<T>>::uninitialize;

		TStreamedMatrixEncoder() { }

		TStreamedMatrixEncoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TStreamedMatrixEncoder()
		{
			this->uninitialize();
		}
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
