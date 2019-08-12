#pragma once

#include "ovpCStreamedMatrixDecoder.h"

#define OVP_ClassId_Algorithm_ChannelUnitsStreamDecoder                                             OpenViBE::CIdentifier(0x5F973DDF, 0x4A582DAF)
#define OVP_ClassId_Algorithm_ChannelUnitsStreamDecoderDesc                                         OpenViBE::CIdentifier(0x2D59257D, 0x3B1915DA)
#define OVP_Algorithm_ChannelUnitsStreamDecoder_OutputParameterId_Dynamic                           OpenViBE::CIdentifier(0x31CF1C7A, 0x17475323)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelUnitsDecoder : public CStreamedMatrixDecoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_ChannelUnitsStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& rIdentifier) override;
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
			void closeChild() override;

		protected:

			OpenViBE::Kernel::TParameterHandler<bool> op_bDynamic;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
			//			uint32_t m_ui32UnitIndex = 0;
		};

		class CChannelUnitsDecoderDesc : public CStreamedMatrixDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Channel units stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ChannelUnitsStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CChannelUnitsDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ChannelUnitsStreamDecoder_OutputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_ChannelUnitsStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
