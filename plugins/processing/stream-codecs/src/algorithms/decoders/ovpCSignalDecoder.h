#pragma once

#include "ovpCStreamedMatrixDecoder.h"

#define OVP_ClassId_Algorithm_SignalStreamDecoder                                                          OpenViBE::CIdentifier(0x7237C149, 0x0CA66DA7)
#define OVP_ClassId_Algorithm_SignalStreamDecoderDesc                                                      OpenViBE::CIdentifier(0xF1547D89, 0x49FFD0C2)
#define OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate                                   OpenViBE::CIdentifier(0x363D8D79, 0xEEFB912C)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSignalDecoder : public CStreamedMatrixDecoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_SignalStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& rIdentifier) override;
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
			void closeChild() override;

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SamplingRate;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CSignalDecoderDesc : public CStreamedMatrixDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Signal stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_SignalStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CSignalDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_SignalStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
