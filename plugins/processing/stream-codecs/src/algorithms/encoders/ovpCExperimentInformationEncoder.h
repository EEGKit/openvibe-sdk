#pragma once

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
		class CExperimentInformationEncoder : public CEBMLBaseEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64ExperimentIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_pExperimentDate;

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SubjectIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_pSubjectName;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SubjectAge;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SubjectGender;

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64LaboratoryIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_pLaboratoryName;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64TechnicianIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_pTechnicianName;
		};

		class CExperimentInformationEncoderDesc : public CEBMLBaseEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Experiment information stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ExperimentInformationStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CExperimentInformationEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier, "Experiment identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate, "Experiment date", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier, "Subject identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName, "Subject name", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge, "Subject age", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender, "Subject gender", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier, "Laboratory identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName, "Laboratory name", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier, "Technician identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName, "Technician name", OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
