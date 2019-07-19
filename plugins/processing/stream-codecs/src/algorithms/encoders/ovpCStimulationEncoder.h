#pragma once

#include "ovpCEBMLBaseEncoder.h"

#define OVP_ClassId_Algorithm_StimulationStreamEncoder                                                     OpenViBE::CIdentifier(0x6E86F7D5, 0xA4668108)
#define OVP_ClassId_Algorithm_StimulationStreamEncoderDesc                                                 OpenViBE::CIdentifier(0x9B994B50, 0x52C3F06A)
#define OVP_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet                             OpenViBE::CIdentifier(0x8565254C, 0x3A49268E)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStimulationEncoder : public CEBMLBaseEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processBuffer(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_StimulationStreamEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> ip_pStimulationSet;
		};

		class CStimulationEncoderDesc : public CEBMLBaseEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Stimulation stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_StimulationStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CStimulationEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet, "Stimulation set", OpenViBE::Kernel::ParameterType_StimulationSet);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_StimulationStreamEncoderDesc);
		};
	};
};


