#pragma once

#include "ovpCEBMLBaseDecoder.h"

#define OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder                                           OpenViBE::CIdentifier(0x6FA7D52B, 0x80E2ABD6)
#define OVP_ClassId_Algorithm_ExperimentInformationStreamDecoderDesc                                       OpenViBE::CIdentifier(0x0F37CA61, 0x8A77F44E)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier            OpenViBE::CIdentifier(0x40259641, 0x478C73DE)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate                  OpenViBE::CIdentifier(0xBC0266A2, 0x9C2935F1)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier               OpenViBE::CIdentifier(0x97C5D20D, 0x203E65B3)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName                     OpenViBE::CIdentifier(0x3D3826EA, 0xE8883815)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge                      OpenViBE::CIdentifier(0xC36C6B08, 0x5227380A)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender                   OpenViBE::CIdentifier(0x7D5059E8, 0xE4D8B38D)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier            OpenViBE::CIdentifier(0xE761D3D4, 0x44BA1EBF)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName                  OpenViBE::CIdentifier(0x5CA80FA5, 0x774F01CB)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier            OpenViBE::CIdentifier(0xC8ECFBBC, 0x0DCDA310)
#define OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName                  OpenViBE::CIdentifier(0xB8A94B68, 0x389393D9)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CExperimentInformationDecoder : public CEBMLBaseDecoder
		{
		public:

			CExperimentInformationDecoder();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& rIdentifier) override;
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
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

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CExperimentInformationDecoderDesc : public CEBMLBaseDecoderDesc
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

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier, "Experiment identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate, "Experiment date", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier, "Subject identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName, "Subject name", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge, "Subject age", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender, "Subject gender", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier, "Laboratory identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName, "Laboratory name", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier, "Technician identifier", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName, "Technician name", OpenViBE::Kernel::ParameterType_String);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
