#pragma once

#include "../../ovp_defines.h"
#include "ovpCStreamedMatrixDecoder.h"
#include <iomanip>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSpectrumDecoder final : public CStreamedMatrixDecoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_SpectrumStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const uint64_t size) override;
			void closeChild() override;

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pFrequencyAbscissa;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_pSamplingRate;


		private:

			std::stack<EBML::CIdentifier> m_vNodes;

			uint32_t m_ui32FrequencyBandIdx = 0;

			// Value of the current lower frequency of the band. Only used to read old spectrum format.
			double m_lowerFreq = 0;
		};

		class CSpectrumDecoderDesc final : public CStreamedMatrixDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Spectrum stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Decodes the Spectrum type streams."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_SpectrumStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CSpectrumDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(prototype);

				prototype.addOutputParameter(
					OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa, "Frequency abscissa", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addOutputParameter(
					OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_Sampling, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_SpectrumStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
