#pragma once

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/TReaderCallbackProxy.h>

#include <stack>

#define OVP_ClassId_Algorithm_EBMLBaseStreamDecoder                                                        OpenViBE::CIdentifier(0xFD30C96D, 0x8245A8F8)
#define OVP_ClassId_Algorithm_EBMLBaseStreamDecoderDesc                                                    OpenViBE::CIdentifier(0x4F701AC9, 0xDFBE912E)
#define OVP_Algorithm_EBMLStreamDecoder_InputParameterId_MemoryBufferToDecode                              OpenViBE::CIdentifier(0x2F98EA3C, 0xFB0BE096)
#define OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedHeader                                     OpenViBE::CIdentifier(0x815234BF, 0xAABAE5F2)
#define OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedBuffer                                     OpenViBE::CIdentifier(0xAA2738BF, 0xF7FE9FC3)
#define OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedEnd                                        OpenViBE::CIdentifier(0xC4AA114C, 0x628C2D77)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEBMLBaseDecoder : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:

			CEBMLBaseDecoder();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_EBMLBaseStreamDecoder)

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& identifier);
			virtual void openChild(const EBML::CIdentifier& identifier);
			virtual void processChildData(const void* /*buffer*/, const uint64_t /*size*/) { }
			virtual void closeChild() { }

		protected:

			EBML::IReaderHelper* m_pEBMLReaderHelper = nullptr;
			EBML::IReader* m_pEBMLReader             = nullptr;
			EBML::TReaderCallbackProxy1<CEBMLBaseDecoder> m_oEBMLReaderCallbackProxy;

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pMemoryBufferToDecode;
		};

		class CEBMLBaseDecoderDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_EBMLStreamDecoder_InputParameterId_MemoryBufferToDecode, "Memory buffer to decode", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				rAlgorithmPrototype.addOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedHeader, "Received header");
				rAlgorithmPrototype.addOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedBuffer, "Received buffer");
				rAlgorithmPrototype.addOutputTrigger(OVP_Algorithm_EBMLStreamDecoder_OutputTriggerId_ReceivedEnd, "Received end");

				return true;
			}

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_EBMLBaseStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
