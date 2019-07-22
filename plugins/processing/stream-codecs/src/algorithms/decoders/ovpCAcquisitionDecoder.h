#pragma once

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
		class CAcquisitionDecoder : public CEBMLBaseDecoder
		{
		public:

			CAcquisitionDecoder();

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_AcquisitionStreamDecoder)

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual void closeChild();

		protected:

			void appendMemoryBuffer(OpenViBE::IMemoryBuffer* pMemoryBuffer, const void* pBuffer, uint64_t ui64BufferSize);

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pSignalStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelLocalisationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelUnitsStream;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CAcquisitionDecoderDesc : public CEBMLBaseDecoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Acquisition stream decoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_AcquisitionStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAcquisitionDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream, "Experiment information stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream, "Signal stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream, "Stimulation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream, "Channel localisation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelUnitsStream, "Channel units stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamDecoderDesc)
		};
	};
};
