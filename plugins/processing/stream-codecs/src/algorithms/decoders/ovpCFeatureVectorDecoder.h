#ifndef __SamplePlugin_Algorithms_CFeatureVectorDecoder_H__
#define __SamplePlugin_Algorithms_CFeatureVectorDecoder_H__

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

			virtual void release(void) { delete this; }

			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_FeatureVectorStreamDecoder);

		private:

			EBML::CIdentifier m_oTop;
		};

		class CFeatureVectorDecoderDesc : public CStreamedMatrixDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Feature vector stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_FeatureVectorStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CFeatureVectorDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_FeatureVectorStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CFeatureVectorDecoder_H__
