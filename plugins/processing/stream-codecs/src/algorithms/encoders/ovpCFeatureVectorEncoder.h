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

			virtual void release() { delete this; }

			virtual bool processHeader();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_FeatureVectorStreamEncoder)
		};

		class CFeatureVectorEncoderDesc : public CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Feature vector stream encoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_FeatureVectorStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CFeatureVectorEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_FeatureVectorStreamEncoderDesc)
		};
	};
};


