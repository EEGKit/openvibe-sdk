#ifndef __SamplePlugin_Algorithms_CAcquisitionEncoder_H__
#define __SamplePlugin_Algorithms_CAcquisitionEncoder_H__

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
		class CAcquisitionEncoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processHeader(void);
			virtual OpenViBE::boolean processBuffer(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_AcquisitionStreamEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 > ip_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pSignalStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pChannelLocalisationStream;
			OpenViBE::Kernel::TParameterHandler < OpenViBE::IMemoryBuffer* > ip_pChannelUnitsStream;

		private:

			OpenViBE::uint64 m_ui64MatrixBufferSize;
		};

		class CAcquisitionEncoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Acquisition stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.1"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_AcquisitionStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CAcquisitionEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_BufferDuration,              "Buffer duration",               OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ExperimentInformationStream, "Experiment information stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_SignalStream,                "Signal stream",                 OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_StimulationStream,           "Stimulation stream",            OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelLocalisationStream,   "Channel localisation stream",   OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_AcquisitionStreamEncoder_InputParameterId_ChannelUnitsStream,          "Channel units stream",          OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CAcquisitionEncoder_H__
