#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "../ovtkTCodec.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TEncoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> m_pOutputMemoryBuffer;

		using T::initialize;
		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_ui32ConnectorIndex;

		virtual void setOutputChunk(OpenViBE::IMemoryBuffer* pOutputChunkMemoryBuffer)
		{
			m_pOutputMemoryBuffer = pOutputChunkMemoryBuffer;
		}

		virtual OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*>& getOutputMemoryBuffer()
		{
			return m_pOutputMemoryBuffer;
		}

		virtual bool isOutputTriggerActive(OpenViBE::CIdentifier oTrigger)
		{
			return m_pCodec->isOutputTriggerActive(oTrigger);
		}

		virtual bool process(const OpenViBE::CIdentifier& oTrigger)
		{
			return m_pCodec->process(oTrigger);
		}

		virtual bool process()
		{
			return m_pCodec->process();
		}

		// The functions that need to be specified by the encoders (specific Trigger ID)
		virtual bool encodeHeaderImpl() = 0;
		virtual bool encodeBufferImpl() = 0;
		virtual bool encodeEndImpl() = 0;

	public:

		/*
		These public methods handle every aspect of the encoding process :
		- prepare the output memory buffer with a chunk
		- encode (specific for each encoder)
		- mark output as ready to be sent has to be done manually, for accurate timing.
		*/

		bool encodeHeader()
		{
			this->setOutputChunk(m_pBoxAlgorithm->getDynamicBoxContext().getOutputChunk(m_ui32ConnectorIndex));
			if (!this->encodeHeaderImpl()) return false;
			return true;
		}

		bool encodeBuffer()
		{
			this->setOutputChunk(m_pBoxAlgorithm->getDynamicBoxContext().getOutputChunk(m_ui32ConnectorIndex));
			if (!this->encodeBufferImpl()) return false;
			return true;
		}

		bool encodeEnd()
		{
			this->setOutputChunk(m_pBoxAlgorithm->getDynamicBoxContext().getOutputChunk(m_ui32ConnectorIndex));
			if (!this->encodeEndImpl()) return false;
			return true;
		}
	};

	/*
	This class provides an access to the superclass TEncoder.
	Use case : iterating over a vector of TEncoder, calling encodeHeader() each time.
	You don't need to know which type of encoder is in the vector.
	*/
	template <class T>
	class TEncoder : public TEncoderLocal<TCodec<T>>
	{
	public:
		virtual ~TEncoder() { }
	protected:
		// constructor is protected, ensuring we can't instanciate a TEncoder
		TEncoder() { }
	};
};

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
