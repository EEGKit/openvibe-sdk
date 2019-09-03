#pragma once

#include <map>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_SpectralAnalysis         OpenViBE::CIdentifier(0x84218FF8, 0xA87E7995)
#define OVP_ClassId_SpectralAnalysisDesc     OpenViBE::CIdentifier(0x0051E63C, 0x68E83AD1)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpectralAnalysis final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SpectralAnalysis)

		protected:
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSpectralAnalysis> m_Decoder;

			std::vector<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis> *> m_SpectrumEncoders;
			std::vector<bool> m_IsSpectrumEncoderActive;

			unsigned int m_ChannelCount = 0;
			unsigned int m_SampleCount  = 0;
			unsigned int m_SamplingRate = 0;

			unsigned int m_FFTSize = 0;

			OpenViBE::IMatrix* m_FrequencyAbscissa = nullptr;
		};

		class CBoxAlgorithmSpectralAnalysisDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Spectral Analysis"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Laurent Bonnet / Quentin Barthelemy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Performs a Spectral Analysis using FFT."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Performs a Spectral Analysis using FFT."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.2"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-execute"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_SpectralAnalysis; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSpectralAnalysis(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);

				rPrototype.addOutput("Amplitude", OV_TypeId_Spectrum);
				rPrototype.addOutput("Phase", OV_TypeId_Spectrum);
				rPrototype.addOutput("Real Part", OV_TypeId_Spectrum);
				rPrototype.addOutput("Imaginary Part", OV_TypeId_Spectrum);

				rPrototype.addSetting("Amplitude", OV_TypeId_Boolean, "true");
				rPrototype.addSetting("Phase", OV_TypeId_Boolean, "false");
				rPrototype.addSetting("Real Part", OV_TypeId_Boolean, "false");
				rPrototype.addSetting("Imaginary Part", OV_TypeId_Boolean, "false");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SpectralAnalysisDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
