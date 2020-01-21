#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CExperimentInfoEncoder final : public CEBMLBaseEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_ExperimentInfoEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ExperimentID;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_experimentDate;

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_subjectID;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_subjectName;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_subjectAge;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_subjectGender;

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_LaboratoryID;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_pLaboratoryName;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_TechnicianID;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_pTechnicianName;
		};

		class CExperimentInfoEncoderDesc final : public CEBMLBaseEncoderDesc
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
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ExperimentInfoEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CExperimentInfoEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(prototype);

				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentID, "Experiment identifier", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentDate, "Experiment date", OpenViBE::Kernel::ParameterType_String);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectID, "Subject identifier", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectName, "Subject name", OpenViBE::Kernel::ParameterType_String);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectAge, "Subject age", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectGender, "Subject gender", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryID, "Laboratory identifier", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryName, "Laboratory name", OpenViBE::Kernel::ParameterType_String);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianID, "Technician identifier", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianName, "Technician name", OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_ExperimentInfoEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
