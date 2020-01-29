#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CMasterAcquisitionEncoder final : public OpenViBE::Toolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_MasterAcquisitionEncoder)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_acquisitionStreamEncoder         = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_experimentInfoStreamEncoder      = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_signalStreamEncoder              = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_stimulationStreamEncoder         = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_channelLocalisationStreamEncoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_channelUnitsStreamEncoder        = nullptr;
		};

		class CMasterAcquisitionEncoderDesc final : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Master acquisition stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_MasterAcquisitionEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CMasterAcquisitionEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputTrigger(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader, "Encode header");
				prototype.addInputTrigger(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer, "Encode buffer");
				prototype.addInputTrigger(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd, "Encode end");

				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectID, "Subject identifier", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectAge, "Subject age", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectGender, "Subject gender", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SignalMatrix, "Signal matrix", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SignalSampling, "Signal sampling rate", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_StimulationSet, "Stimulation set", OpenViBE::Kernel::ParameterType_StimulationSet);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_ChannelLocalisation, "Channel localisation", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_ChannelUnits, "Channel units", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_EncodeChannelLocalisationData, "Encode channel localisation data", OpenViBE::Kernel::ParameterType_Boolean);
				prototype.addInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_EncodeChannelUnitData, "Encode channel unit data", OpenViBE::Kernel::ParameterType_Boolean);

				prototype.addOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer, "Encoded memory buffer", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_MasterAcquisitionEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
