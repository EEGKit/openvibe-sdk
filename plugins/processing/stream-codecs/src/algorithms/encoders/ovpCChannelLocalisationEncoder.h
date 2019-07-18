#ifndef __SamplePlugin_Algorithms_CChannelLocalisationEncoder_H__
#define __SamplePlugin_Algorithms_CChannelLocalisationEncoder_H__

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

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processHeader(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler<bool> ip_bDynamic;
		};


		class CChannelLocalisationEncoderDesc : public CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Channel localisation stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CChannelLocalisationEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_ChannelLocalisationStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CChannelLocalisationEncoder_H__
