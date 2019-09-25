#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CExperimentInformationEncoder final : public CEBMLBaseEncoder
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

		class CExperimentInformationEncoderDesc final : public CEBMLBaseEncoderDesc
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

				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier, "Experiment identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate, "Experiment date",
					OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier, "Subject identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName, "Subject name", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge, "Subject age", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender, "Subject gender",
					OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier, "Laboratory identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName, "Laboratory name",
					OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier, "Technician identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName, "Technician name",
					OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
