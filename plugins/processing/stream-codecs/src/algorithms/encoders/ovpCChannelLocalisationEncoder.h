#pragma once

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder                                             OpenViBE::CIdentifier(0xC4AA738A, 0x2368C0EA)
#define OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoderDesc                                         OpenViBE::CIdentifier(0x3F7B49A3, 0x2B8F861A)
#define OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic                            OpenViBE::CIdentifier(0xCF5DD4F8, 0xC2FF2878)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelLocalisationEncoder final : public CStreamedMatrixEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<bool> ip_bDynamic;
		};


		class CChannelLocalisationEncoderDesc final : public CStreamedMatrixEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Channel localisation stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CChannelLocalisationEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
