#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpectralAnalysis final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_SpectralAnalysis)

		protected:
			OpenViBE::Toolkit::TSignalDecoder<CBoxAlgorithmSpectralAnalysis> m_decoder;

			std::vector<OpenViBE::Toolkit::TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis> *> m_spectrumEncoders;
			std::vector<bool> m_isSpectrumEncoderActive;

			size_t m_nChannel = 0;
			size_t m_nSample  = 0;
			size_t m_sampling = 0;

			size_t m_sizeFFT = 0;

			OpenViBE::IMatrix* m_frequencyAbscissa = nullptr;
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

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);

				prototype.addOutput("Amplitude", OV_TypeId_Spectrum);
				prototype.addOutput("Phase", OV_TypeId_Spectrum);
				prototype.addOutput("Real Part", OV_TypeId_Spectrum);
				prototype.addOutput("Imaginary Part", OV_TypeId_Spectrum);

				prototype.addSetting("Amplitude", OV_TypeId_Boolean, "true");
				prototype.addSetting("Phase", OV_TypeId_Boolean, "false");
				prototype.addSetting("Real Part", OV_TypeId_Boolean, "false");
				prototype.addSetting("Imaginary Part", OV_TypeId_Boolean, "false");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SpectralAnalysisDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
