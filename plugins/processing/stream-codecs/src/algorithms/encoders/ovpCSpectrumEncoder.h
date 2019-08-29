#pragma once

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_SpectrumStreamEncoder                                                        OpenViBE::CIdentifier(0xB3E252DB, 0xC3214498)
#define OVP_ClassId_Algorithm_SpectrumStreamEncoderDesc                                                    OpenViBE::CIdentifier(0xD6182973, 0x122CE114)
#define OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa                          OpenViBE::CIdentifier(0x05C91BD6, 0x2D8C4083)
#define OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate                                  OpenViBE::CIdentifier(0x02D25E1B, 0x76A1019B)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSpectrumEncoder final : public CStreamedMatrixEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_SpectrumStreamEncoder)

		protected:
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pFrequencyAbscissa;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_pSamplingRate;
		};

		class CSpectrumEncoderDesc final : public CStreamedMatrixEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Spectrum stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Encodes a Spectrum stream."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_SpectrumStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CSpectrumEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_FrequencyAbscissa, "Frequency abscissa", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_SpectrumStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
