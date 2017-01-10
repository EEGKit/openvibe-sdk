#pragma once

#include <map>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_SpectralAnalysis         OpenViBE::CIdentifier(0x84218FF8, 0xA87E7995)
#define OVP_ClassId_SpectralAnalysisDesc     OpenViBE::CIdentifier(0x0051E63C, 0x68E83AD1)

// Spectral Analysis: ID names and values taken from the signal-processing-gpl project
#define OVP_TypeId_SpectralComponent                                                    OpenViBE::CIdentifier(0x764E148A, 0xC704D4F5)
#define OVP_TypeId_SpectralComponent_Amplitude                                          OpenViBE::CIdentifier(0x00000000, 0x00000001)
#define OVP_TypeId_SpectralComponent_Phase                                              OpenViBE::CIdentifier(0x00000000, 0x00000002)
#define OVP_TypeId_SpectralComponent_RealPart                                           OpenViBE::CIdentifier(0x00000000, 0x00000004)
#define OVP_TypeId_SpectralComponent_ImaginaryPart                                      OpenViBE::CIdentifier(0x00000000, 0x00000008)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpectralAnalysis :  public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_SpectralAnalysis)

		protected:
			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmSpectralAnalysis > m_oDecoder;

			std::vector < OpenViBEToolkit::TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis > * > m_vSpectrumEncoders;
			std::vector < OpenViBE::boolean > m_vIsSpectrumEncoderActive;

			OpenViBE::uint32 m_ui32ChannelCount;
			OpenViBE::uint32 m_ui32SampleCount;
			OpenViBE::uint32 m_ui32SamplingRate;

			OpenViBE::uint32 m_ui32FFTSize;

			OpenViBE::IMatrix * m_pFrequencyBandDescription;

			OpenViBE::uint64 m_ui64BitmaskSpectralComponents;
			inline OpenViBE::boolean isAmplitudeProcessingEnabled()     { return  (m_ui64BitmaskSpectralComponents & OVP_TypeId_SpectralComponent_Amplitude.toUInteger())!=0; }
			inline OpenViBE::boolean isPhaseProcessingEnabled()         { return  (m_ui64BitmaskSpectralComponents & OVP_TypeId_SpectralComponent_Phase.toUInteger())!=0; }
			inline OpenViBE::boolean isRealPartProcessingEnabled()      { return  (m_ui64BitmaskSpectralComponents & OVP_TypeId_SpectralComponent_RealPart.toUInteger())!=0; }
			inline OpenViBE::boolean isImaginaryPartProcessingEnabled() { return  (m_ui64BitmaskSpectralComponents & OVP_TypeId_SpectralComponent_ImaginaryPart.toUInteger())!=0; }
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
			virtual OpenViBE::CString getVersion(void) const               { return OpenViBE::CString("1.1"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const  { return OpenViBE::CString("1.0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const{ return OpenViBE::CString("2.4.0.0"); }
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

				rPrototype.addSetting("Spectral components", OVP_TypeId_SpectralComponent, "Amplitude");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_SpectralAnalysisDesc)
		};
	}
}
