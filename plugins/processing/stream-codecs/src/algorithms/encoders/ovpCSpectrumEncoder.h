#ifndef __SamplePlugin_Algorithms_CSpectrumEncoder_H__
#define __SamplePlugin_Algorithms_CSpectrumEncoder_H__

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_SpectrumStreamEncoder                                                        OpenViBE::CIdentifier(0xB3E252DB, 0xC3214498)
#define OVP_ClassId_Algorithm_SpectrumStreamEncoderDesc                                                    OpenViBE::CIdentifier(0xD6182973, 0x122CE114)
#define OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa                          OpenViBE::CIdentifier(0x05C91BD6, 0x2D8C4083)
#define OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate                                  OpenViBE::CIdentifier(0x02D25E1B, 0x76A1019B)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSpectrumEncoder : public CStreamedMatrixEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processHeader(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_SpectrumStreamEncoder);

		protected:
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pFrequencyAbscissa;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_pSamplingRate;
		};

		class CSpectrumEncoderDesc : public CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Spectrum stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Encodes a Spectrum stream."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_SpectrumStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CSpectrumEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa, "Frequency abscissa", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_SpectrumStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CSpectrumEncoder_H__
