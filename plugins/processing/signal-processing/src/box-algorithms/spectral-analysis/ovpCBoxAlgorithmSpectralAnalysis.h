#pragma once

#include <map>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_SpectralAnalysis         OpenViBE::CIdentifier(0x84218FF8, 0xA87E7995)
#define OVP_ClassId_SpectralAnalysisDesc     OpenViBE::CIdentifier(0x0051E63C, 0x68E83AD1)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpectralAnalysis : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SpectralAnalysis)

		protected:
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSpectralAnalysis> m_Decoder;

			std::vector<OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis> *> m_SpectrumEncoders;
			std::vector<bool> m_IsSpectrumEncoderActive;

			unsigned int m_ChannelCount;
			unsigned int m_SampleCount;
			unsigned int m_SamplingRate;

			unsigned int m_FFTSize;

			OpenViBE::IMatrix* m_FrequencyAbscissa;
		};

		class CBoxAlgorithmSpectralAnalysisDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Spectral Analysis"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Laurent Bonnet / Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Performs a Spectral Analysis using FFT."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Performs a Spectral Analysis using FFT."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.2"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.1.0"); }
			virtual OpenViBE::CString getStockItemName() const { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_SpectralAnalysis; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmSpectralAnalysis(); }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins
