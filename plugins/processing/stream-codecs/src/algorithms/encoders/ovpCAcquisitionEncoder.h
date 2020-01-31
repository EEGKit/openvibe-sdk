#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBE
{
	namespace Plugins
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

				_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_AcquisitionEncoder)

			protected:

				Kernel::TParameterHandler<uint64_t> ip_bufferDuration;
				Kernel::TParameterHandler<IMemoryBuffer*> ip_experimentInfoStream;
				Kernel::TParameterHandler<IMemoryBuffer*> ip_signalStream;
				Kernel::TParameterHandler<IMemoryBuffer*> ip_stimulationStream;
				Kernel::TParameterHandler<IMemoryBuffer*> ip_channelLocalisationStream;
				Kernel::TParameterHandler<IMemoryBuffer*> ip_channelUnitsStream;
			};

			class CAcquisitionEncoderDesc final : public CEBMLBaseEncoderDesc
			{
			public:
				void release() override { }
				CString getName() const override { return CString("Acquisition stream encoder"); }
				CString getAuthorName() const override { return CString("Yann Renard"); }
				CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
				CString getShortDescription() const override { return CString(""); }
				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Stream codecs/Encoders"); }
				CString getVersion() const override { return CString("1.1"); }
				CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
				CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
				CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
				CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_AcquisitionEncoder; }
				IPluginObject* create() override { return new CAcquisitionEncoder(); }

				bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
				{
					CEBMLBaseEncoderDesc::getAlgorithmPrototype(prototype);

					prototype.addInputParameter(
						OVP_Algorithm_AcquisitionEncoder_InputParameterId_BufferDuration, "Buffer duration", Kernel::ParameterType_UInteger);
					prototype.addInputParameter(
						OVP_Algorithm_AcquisitionEncoder_InputParameterId_ExperimentInfoStream, "Experiment information stream",
						Kernel::ParameterType_MemoryBuffer);
					prototype.addInputParameter(
						OVP_Algorithm_AcquisitionEncoder_InputParameterId_SignalStream, "Signal stream", Kernel::ParameterType_MemoryBuffer);
					prototype.addInputParameter(
						OVP_Algorithm_AcquisitionEncoder_InputParameterId_StimulationStream, "Stimulation stream",
						Kernel::ParameterType_MemoryBuffer);
					prototype.addInputParameter(
						OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelLocalisationStream, "Channel localisation stream",
						Kernel::ParameterType_MemoryBuffer);
					prototype.addInputParameter(
						OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelUnitsStream, "Channel units stream",
						Kernel::ParameterType_MemoryBuffer);

					return true;
				}

				_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_AcquisitionEncoderDesc)
			};
		} // namespace StreamCodecs
	}  // namespace Plugins
}  // namespace OpenViBE
