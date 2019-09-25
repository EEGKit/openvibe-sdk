#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CAcquisitionEncoder final : public CEBMLBaseEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;
			bool processBuffer() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_AcquisitionStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pSignalStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pChannelLocalisationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pChannelUnitsStream;
		};

		class CAcquisitionEncoderDesc final : public CEBMLBaseEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Acquisition stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_AcquisitionStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAcquisitionEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream, "Experiment information stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream, "Signal stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream, "Stimulation stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream, "Channel localisation stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelUnitsStream, "Channel units stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
