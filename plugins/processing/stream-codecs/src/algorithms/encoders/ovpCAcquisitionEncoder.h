#pragma once

#include "ovpCEBMLBaseEncoder.h"

#define OVP_ClassId_Algorithm_AcquisitionStreamEncoder                                                     OpenViBE::CIdentifier(0xF9FD2FB5, 0xDF0B3B2C)
#define OVP_ClassId_Algorithm_AcquisitionStreamEncoderDesc                                                 OpenViBE::CIdentifier(0xE3E0D9EB, 0x4D4EBA00)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration                             OpenViBE::CIdentifier(0xAFA07097, 0x1145B59B)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream                OpenViBE::CIdentifier(0x38755128, 0xCB0C908A)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream                               OpenViBE::CIdentifier(0x4ED9D929, 0x6DF5B2B6)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream                          OpenViBE::CIdentifier(0xCDE202AD, 0xF4864EC9)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream                  OpenViBE::CIdentifier(0x2CF786E5, 0x520714A1)
#define OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelUnitsStream                         OpenViBE::CIdentifier(0x25DD84B4, 0x528524CA)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CAcquisitionEncoder : public CEBMLBaseEncoder
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processHeader();
			virtual bool processBuffer();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_AcquisitionStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pSignalStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pChannelLocalisationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> ip_pChannelUnitsStream;

		private:
		};

		class CAcquisitionEncoderDesc : public CEBMLBaseEncoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Acquisition stream encoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_AcquisitionStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAcquisitionEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream, "Experiment information stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream, "Signal stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream, "Stimulation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream, "Channel localisation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelUnitsStream, "Channel units stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamEncoderDesc)
		};
	};
};


