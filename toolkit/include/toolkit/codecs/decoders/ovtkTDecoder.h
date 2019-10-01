#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "../ovtkTCodec.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TDecoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> m_pInputMemoryBuffer;


		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_ui32ConnectorIndex;

		virtual void setInputChunk(const OpenViBE::IMemoryBuffer* pInputChunkMemoryBuffer) { m_pInputMemoryBuffer = pInputChunkMemoryBuffer; }

		virtual OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*>& getInputMemoryBuffer() { return m_pInputMemoryBuffer; }

		virtual bool isOutputTriggerActive(const OpenViBE::CIdentifier oTrigger) { return m_pCodec->isOutputTriggerActive(oTrigger); }

		virtual bool process(const OpenViBE::CIdentifier& oTrigger) { return m_pCodec->process(oTrigger); }

		virtual bool process() { return m_pCodec->process(); }

	public:
		// We make visible the initialize methods of the superclass (should be TCodec), in the same scope (public)
		using T::initialize;

		/*
		This public function handles every aspects of the decoding process:
		- fill the input memory buffer with a chunk
		- decode it (specific for each decoder)
		- mark input as deprecated
		*/
		virtual bool decode(uint32_t chunkIdx, const bool markInputAsDeprecated = true)
		{
			this->setInputChunk(m_pBoxAlgorithm->getDynamicBoxContext().getInputChunk(m_ui32ConnectorIndex, chunkIdx));
			if (! m_pCodec->process()) return false;
			if (markInputAsDeprecated) m_pBoxAlgorithm->getDynamicBoxContext().markInputAsDeprecated(m_ui32ConnectorIndex, chunkIdx);
			return true;
		}

		// We explicitly delete the decode function taking two integers as parameters
		// in order to raise errors in plugins using the older API
#ifndef TARGET_OS_MacOS // Current clang has a bug which fails to link these
		virtual bool decode(int, int)                   = delete;
		virtual bool decode(uint32_t, uint32_t) = delete;
#endif

		// The functions that need to be specified by the decoders (specific Trigger ID)
		virtual bool isHeaderReceived() = 0;
		virtual bool isBufferReceived() = 0;
		virtual bool isEndReceived() = 0;
	};

	/*
	This class provides an access to the superclass TDecoder.
	Use case : iterating over a vector of TDecoder, calling decode() each time.
	You don't need to know which type of decoder is in the vector.
	*/
	template <class T>
	class TDecoder : public TDecoderLocal<TCodec<T>>
	{
	public:
		virtual ~TDecoder() { }
	protected:
		// constructor is protected, ensuring we can't instanciate a TDecoder
		TDecoder() { }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
