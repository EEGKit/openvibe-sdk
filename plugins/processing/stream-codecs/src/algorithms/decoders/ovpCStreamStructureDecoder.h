#pragma once

#include "ovpCEBMLBaseDecoder.h"

#define OVP_ClassId_Algorithm_StreamStructureStreamDecoder     OpenViBE::CIdentifier(0xA7EF3E8B, 0x4CF70B74)
#define OVP_ClassId_Algorithm_StreamStructureStreamDecoderDesc OpenViBE::CIdentifier(0x2E361099, 0xCBE828A7)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStreamStructureDecoder : public CEBMLBaseDecoder
		{
		public:

			CStreamStructureDecoder();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StreamStructureStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& rIdentifier) override;
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
			void closeChild() override;
		};

		class CStreamStructureDecoderDesc : public CEBMLBaseDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Stream Structure Decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jozef Legeny"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("2.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_StreamStructureStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CStreamStructureDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StreamStructureStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
