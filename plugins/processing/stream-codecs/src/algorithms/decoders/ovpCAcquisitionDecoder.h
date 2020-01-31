#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseDecoder.h"
#include <stack>

namespace OpenViBE
{
	namespace Plugins
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

			_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_AcquisitionDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const size_t size) override;
			void closeChild() override;

		protected:

			static void appendMemoryBuffer(IMemoryBuffer* memoryBuffer, const void* buffer, size_t size);

			Kernel::TParameterHandler<uint64_t> op_bufferDuration;
			Kernel::TParameterHandler<IMemoryBuffer*> op_experimentInfoStream;
			Kernel::TParameterHandler<IMemoryBuffer*> op_signalStream;
			Kernel::TParameterHandler<IMemoryBuffer*> op_stimulationStream;
			Kernel::TParameterHandler<IMemoryBuffer*> op_channelLocalisationStream;
			Kernel::TParameterHandler<IMemoryBuffer*> op_channelUnitsStream;

		private:

			std::stack<EBML::CIdentifier> m_nodes;
		};

		class CAcquisitionDecoderDesc final : public CEBMLBaseDecoderDesc
		{
		public:
			void release() override { }
			CString getName() const override { return CString("Acquisition stream decoder"); }
			CString getAuthorName() const override { return CString("Yann Renard"); }
			CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
			CString getShortDescription() const override { return CString(""); }
			CString getDetailedDescription() const override { return CString(""); }
			CString getCategory() const override { return CString("Stream codecs/Decoders"); }
			CString getVersion() const override { return CString("1.1"); }
			CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
			CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
			CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
			CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_AcquisitionDecoder; }
			IPluginObject* create() override { return new CAcquisitionDecoder(); }

			bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(prototype);

				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_BufferDuration, "Buffer duration", Kernel::ParameterType_UInteger);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ExperimentInfoStream, "Experiment information stream", Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_SignalStream, "Signal stream", Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_StimulationStream, "Stimulation stream", Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ChannelLocalisationStream, "Channel localisation stream", Kernel::ParameterType_MemoryBuffer);
				prototype.addOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ChannelUnitsStream, "Channel units stream", Kernel::ParameterType_MemoryBuffer);

				return true;
			}

			_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_AcquisitionDecoderDesc)
		};
	} // namespace StreamCodecs
	}  // namespace Plugins
}  // namespace OpenViBE
