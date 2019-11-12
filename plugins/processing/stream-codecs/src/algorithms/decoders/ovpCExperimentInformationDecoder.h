#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseDecoder.h"
#include <stack>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CExperimentInformationDecoder final : public CEBMLBaseDecoder
		{
		public:

			CExperimentInformationDecoder();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const uint64_t size) override;
			void closeChild() override;

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64ExperimentIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> op_pExperimentDate;

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SubjectIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> op_pSubjectName;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SubjectAge;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SubjectGender;

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64LaboratoryIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> op_pLaboratoryName;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64TechnicianIdentifier;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> op_pTechnicianName;

		private:

			std::stack<EBML::CIdentifier> m_nodes;
		};

		class CExperimentInformationDecoderDesc final : public CEBMLBaseDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Experiment information stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CExperimentInformationDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(prototype);

				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier, "Experiment identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate, "Experiment date",
					OpenViBE::Kernel::ParameterType_String);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier, "Subject identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName, "Subject name", OpenViBE::Kernel::ParameterType_String);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge, "Subject age", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender, "Subject gender",
					OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier, "Laboratory identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName, "Laboratory name",
					OpenViBE::Kernel::ParameterType_String);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier, "Technician identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(
					OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName, "Technician name",
					OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
