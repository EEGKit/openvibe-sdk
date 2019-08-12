#pragma once

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_FeatureVectorStreamEncoder                                                   OpenViBE::CIdentifier(0x7EBE049D, 0xF777A602)
#define OVP_ClassId_Algorithm_FeatureVectorStreamEncoderDesc                                               OpenViBE::CIdentifier(0xC249527B, 0x89EE1996)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CFeatureVectorEncoder : public CStreamedMatrixEncoder
		{
		public:
			void release() override { delete this; }
			bool processHeader() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_FeatureVectorStreamEncoder)
		};

		class CFeatureVectorEncoderDesc : public CStreamedMatrixEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Feature vector stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_FeatureVectorStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CFeatureVectorEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_FeatureVectorStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
