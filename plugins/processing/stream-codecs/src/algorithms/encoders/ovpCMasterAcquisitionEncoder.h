#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CMasterAcquisitionEncoder final : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoder)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pAcquisitionStreamEncoder           = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pExperimentInformationStreamEncoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pSignalStreamEncoder                = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStimulationStreamEncoder           = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pChannelLocalisationStreamEncoder   = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pChannelUnitsStreamEncoder          = nullptr;
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
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CMasterAcquisitionEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader, "Encode header");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer, "Encode buffer");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeEnd, "Encode end");

				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectIdentifier, "Subject identifier",
					OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectAge, "Subject age", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SubjectGender, "Subject gender", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalMatrix, "Signal matrix", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_SignalSamplingRate, "Signal sampling rate",
					OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_StimulationSet, "Stimulation set",
					OpenViBE::Kernel::ParameterType_StimulationSet);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_ChannelLocalisation, "Channel localisation",
					OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_ChannelUnits, "Channel units", OpenViBE::Kernel::ParameterType_Matrix);

				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_EncodeChannelLocalisationData, "Encode channel localisation data",
					OpenViBE::Kernel::ParameterType_Boolean);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_MasterAcquisitionStreamEncoder_InputParameterId_EncodeChannelUnitData, "Encode channel unit data",
					OpenViBE::Kernel::ParameterType_Boolean);

				rAlgorithmPrototype.addOutputParameter(
					OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer, "Encoded memory buffer",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_MasterAcquisitionStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
