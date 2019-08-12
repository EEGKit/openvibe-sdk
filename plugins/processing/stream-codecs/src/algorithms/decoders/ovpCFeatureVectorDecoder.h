#pragma once

#include "ovpCStreamedMatrixDecoder.h"

#define OVP_ClassId_Algorithm_FeatureVectorStreamDecoder                                                   OpenViBE::CIdentifier(0xC2689ECC, 0x43B335C1)
#define OVP_ClassId_Algorithm_FeatureVectorStreamDecoderDesc                                               OpenViBE::CIdentifier(0xAB0AE561, 0xF181E34F)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CFeatureVectorDecoder : public CStreamedMatrixDecoder
		{
		public:
			void release() override { delete this; }
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* pBuffer, uint64_t ui64BufferSize) override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_FeatureVectorStreamDecoder)

		private:

			EBML::CIdentifier m_oTop;
		};

		class CFeatureVectorDecoderDesc : public CStreamedMatrixDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Feature vector stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_FeatureVectorStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CFeatureVectorDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_FeatureVectorStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
