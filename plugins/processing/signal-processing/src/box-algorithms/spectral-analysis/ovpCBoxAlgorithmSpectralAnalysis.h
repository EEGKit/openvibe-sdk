#pragma once

#include <map>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_SpectralAnalysis         OpenViBE::CIdentifier(0x84218FF8, 0xA87E7995)
#define OVP_ClassId_SpectralAnalysisDesc     OpenViBE::CIdentifier(0x0051E63C, 0x68E83AD1)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpectralAnalysis : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SpectralAnalysis)

		protected:
			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmSpectralAnalysis > m_Decoder;

			std::vector < OpenViBEToolkit::TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis > * > m_SpectrumEncoders;
			std::vector < OpenViBE::boolean > m_IsSpectrumEncoderActive;

			unsigned int m_ChannelCount;
			unsigned int m_SampleCount;
			unsigned int m_SamplingRate;

			unsigned int m_FFTSize;

			OpenViBE::IMatrix * m_FrequencyAbscissa;
		};

		class CBoxAlgorithmSpectralAnalysisDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                  { return OpenViBE::CString("Spectral Analysis"); }
			virtual OpenViBE::CString getAuthorName(void) const            { return OpenViBE::CString("Laurent Bonnet / Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const     { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const      { return OpenViBE::CString("Performs a Spectral Analysis using FFT."); }
			virtual OpenViBE::CString getDetailedDescription(void) const   { return OpenViBE::CString("Performs a Spectral Analysis using FFT."); }
			virtual OpenViBE::CString getCategory(void) const              { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			virtual OpenViBE::CString getVersion(void) const               { return OpenViBE::CString("1.2"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const   { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.1.0"); }
			virtual OpenViBE::CString getStockItemName(void) const         { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_SpectralAnalysis; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSpectralAnalysis(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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
	}
}
