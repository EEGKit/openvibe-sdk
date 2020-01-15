#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseDecoder.h"
#include <stack>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CAcquisitionDecoder final : public CEBMLBaseDecoder
		{
		public:

			CAcquisitionDecoder();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_AcquisitionDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const size_t size) override;
			void closeChild() override;

		protected:

			static void appendMemoryBuffer(OpenViBE::IMemoryBuffer* memoryBuffer, const void* buffer, size_t size);

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_bufferDuration;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_experimentInfoStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_signalStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_stimulationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_channelLocalisationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_channelUnitsStream;

		private:

			std::stack<EBML::CIdentifier> m_nodes;
		};

		class CAcquisitionDecoderDesc final : public CEBMLBaseDecoderDesc
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
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_AcquisitionDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAcquisitionDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(prototype);

				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ExperimentInfoStream, "Experiment information stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_SignalStream, "Signal stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_StimulationStream, "Stimulation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ChannelLocalisationStream, "Channel localisation stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ChannelUnitsStream, "Channel units stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_AcquisitionDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
