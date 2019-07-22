#pragma once

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder                                             OpenViBE::CIdentifier(0xC4AA738A, 0x2368C0EA)
#define OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoderDesc                                         OpenViBE::CIdentifier(0x3F7B49A3, 0x2B8F861A)
#define OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic                            OpenViBE::CIdentifier(0xCF5DD4F8, 0xC2FF2878)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelLocalisationEncoder : public CStreamedMatrixEncoder
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processHeader();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<bool> ip_bDynamic;
		};


		class CChannelLocalisationEncoderDesc : public CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Channel localisation stream encoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CChannelLocalisationEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoderDesc)
		};
	};
};


