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
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_AcquisitionStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& rIdentifier) override;
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
			void closeChild() override;

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
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Acquisition stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_AcquisitionStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAcquisitionDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
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
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
