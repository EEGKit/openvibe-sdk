#ifndef __SamplePlugin_Algorithms_CAcquisitionDecoder_H__
#define __SamplePlugin_Algorithms_CAcquisitionDecoder_H__

#include "ovpCEBMLBaseDecoder.h"

#define OVP_ClassId_Algorithm_AcquisitionStreamDecoder                                                     OpenViBE::CIdentifier(0x1E0812B7, 0x3F686DD4)
#define OVP_ClassId_Algorithm_AcquisitionStreamDecoderDesc                                                 OpenViBE::CIdentifier(0xA01599B0, 0x7F51631A)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration                            OpenViBE::CIdentifier(0x7527D6E5, 0xB7A70339)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream               OpenViBE::CIdentifier(0xA7F1D539, 0xEC708539)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream                              OpenViBE::CIdentifier(0x42C0D7BD, 0xBBCEA3F3)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream                         OpenViBE::CIdentifier(0x08FC3C12, 0x86A07BF7)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream                 OpenViBE::CIdentifier(0x4EB92F81, 0x6ECDA6B9)
#define OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelUnitsStream                        OpenViBE::CIdentifier(0x11B93981, 0x6E5DA9B0)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CAcquisitionDecoder : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder
		{
		public:

			CAcquisitionDecoder(void);

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_AcquisitionStreamDecoder);

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			void appendMemoryBuffer(OpenViBE::IMemoryBuffer* pMemoryBuffer, const void* pBuffer, const EBML::uint64 ui64BufferSize);

			OpenViBE::Kernel::TParameterHandler<OpenViBE::uint64> op_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pSignalStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelLocalisationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelUnitsStream;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CAcquisitionDecoderDesc : public OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Acquisition stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.1"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_AcquisitionStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::StreamCodecs::CAcquisitionDecoder(); }

			virtual bool getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream, "Experiment information stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream, "Signal stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream, "Stimulation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream, "Channel localisation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelUnitsStream, "Channel units stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CAcquisitionDecoder_H__
