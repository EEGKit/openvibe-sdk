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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder)

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual void closeChild();

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

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Experiment information stream decoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CExperimentInformationDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
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
	};
};


