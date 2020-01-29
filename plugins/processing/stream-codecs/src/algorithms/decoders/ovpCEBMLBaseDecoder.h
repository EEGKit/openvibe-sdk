#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/TReaderCallbackProxy.h>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEBMLBaseDecoder : public OpenViBE::Toolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:

			CEBMLBaseDecoder();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_EBMLBaseDecoder)

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& identifier);
			virtual void openChild(const EBML::CIdentifier& identifier);
			virtual void processChildData(const void* /*buffer*/, const size_t /*size*/) { }
			virtual void closeChild() { }

		protected:

			EBML::IReaderHelper* m_readerHelper = nullptr;
			EBML::IReader* m_reader             = nullptr;
			EBML::TReaderCallbackProxy1<CEBMLBaseDecoder> m_callbackProxy;

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_bufferToDecode;
		};

		class CEBMLBaseDecoderDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputParameter(
					OVP_Algorithm_EBMLDecoder_InputParameterId_MemoryBufferToDecode, "Memory buffer to decode",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);

				prototype.addOutputTrigger(OVP_Algorithm_EBMLDecoder_OutputTriggerId_ReceivedHeader, "Received header");
				prototype.addOutputTrigger(OVP_Algorithm_EBMLDecoder_OutputTriggerId_ReceivedBuffer, "Received buffer");
				prototype.addOutputTrigger(OVP_Algorithm_EBMLDecoder_OutputTriggerId_ReceivedEnd, "Received end");

				return true;
			}

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_EBMLBaseDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
