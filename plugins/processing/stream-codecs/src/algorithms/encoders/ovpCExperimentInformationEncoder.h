#ifndef __SamplePlugin_Algorithms_CExperimentInformationEncoder_H__
#define __SamplePlugin_Algorithms_CExperimentInformationEncoder_H__

#include "ovpCEBMLBaseEncoder.h"

#define OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder                                           OpenViBE::CIdentifier(0x56B354FE, 0xBF175468)
#define OVP_ClassId_Algorithm_ExperimentInformationStreamEncoderDesc                                       OpenViBE::CIdentifier(0x8CC2C754, 0x61665FDA)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier             OpenViBE::CIdentifier(0x40259641, 0x478C73DE)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate                   OpenViBE::CIdentifier(0xBC0266A2, 0x9C2935F1)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier                OpenViBE::CIdentifier(0x97C5D20D, 0x203E65B3)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName                      OpenViBE::CIdentifier(0x3D3826EA, 0xE8883815)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge                       OpenViBE::CIdentifier(0xC36C6B08, 0x5227380A)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender                    OpenViBE::CIdentifier(0x7D5059E8, 0xE4D8B38D)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier             OpenViBE::CIdentifier(0xE761D3D4, 0x44BA1EBF)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName                   OpenViBE::CIdentifier(0x5CA80FA5, 0x774F01CB)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier             OpenViBE::CIdentifier(0xC8ECFBBC, 0x0DCDA310)
#define OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName                   OpenViBE::CIdentifier(0xB8A94B68, 0x389393D9)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CExperimentInformationEncoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processHeader(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64ExperimentIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pExperimentDate;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SubjectIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pSubjectName;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SubjectAge;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64SubjectGender;

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64LaboratoryIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pLaboratoryName;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64TechnicianIdentifier;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::CString* > ip_pTechnicianName;
		};

		class CExperimentInformationEncoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Experiment information stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const   { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CExperimentInformationEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier, "Experiment identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate,       "Experiment date",       OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier,    "Subject identifier",    OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName,          "Subject name",          OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge,           "Subject age",           OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender,        "Subject gender",        OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier, "Laboratory identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName,       "Laboratory name",       OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier, "Technician identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName,       "Technician name",       OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CExperimentInformationEncoder_H__
