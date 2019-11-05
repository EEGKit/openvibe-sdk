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

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_AcquisitionStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const uint64_t size) override;
			void closeChild() override;

		protected:

			void appendMemoryBuffer(OpenViBE::IMemoryBuffer* pMemoryBuffer, const void* buffer, uint64_t size);

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64BufferDuration;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pExperimentInformationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pSignalStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pStimulationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelLocalisationStream;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelUnitsStream;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
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
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_AcquisitionStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAcquisitionDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(prototype);

				prototype.addOutputParameter(
					OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration, "Buffer duration", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(
					OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream, "Experiment information stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(
					OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream, "Signal stream", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(
					OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream, "Stimulation stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(
					OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream, "Channel localisation stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(
					OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelUnitsStream, "Channel units stream",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_AcquisitionStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
