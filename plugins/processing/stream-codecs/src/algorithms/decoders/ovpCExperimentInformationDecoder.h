#ifndef __SamplePlugin_Algorithms_CExperimentInformationDecoder_H__
#define __SamplePlugin_Algorithms_CExperimentInformationDecoder_H__

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
		class CExperimentInformationDecoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder
		{
		public:

			CExperimentInformationDecoder(void);

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder);

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const uint64_t ui64BufferSize);
			virtual void closeChild(void);

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

		class CExperimentInformationDecoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Experiment information stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_ExperimentInformationStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::StreamCodecs::CExperimentInformationDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

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

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_ExperimentInformationStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CExperimentInformationDecoder_H__
