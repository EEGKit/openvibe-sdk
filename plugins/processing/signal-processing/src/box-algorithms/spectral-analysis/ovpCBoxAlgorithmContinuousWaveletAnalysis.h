#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>
#include <wavelib/header/wavelib.h>

#define OVP_ClassId_ContinuousWaveletAnalysis         OpenViBE::CIdentifier(0x0A43133D, 0x6EAF25A7)
#define OVP_ClassId_ContinuousWaveletAnalysisDesc     OpenViBE::CIdentifier(0x5B397A82, 0x76AE6F81)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmContinuousWaveletAnalysis : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_ContinuousWaveletAnalysis)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmContinuousWaveletAnalysis> m_oDecoder;
			OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmContinuousWaveletAnalysis> m_vEncoder[4];

			const char* m_pWaveletType;
			double m_dWaveletParameter = 0;
			int m_iScaleCount_J        = 0;
			double m_dHighestFrequency = 0;
			double m_dSmallestScale_s0 = 0;
			double m_dScaleSpacing_dj  = 0;

			const char* m_pScaleType;
			int m_iScalePowerBase_a0       = 0;
			double m_dSamplingPeriod_dt    = 0;
			cwt_object m_oWaveletTransform = nullptr;
		};

		class CBoxAlgorithmContinuousWaveletAnalysisDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Continuous Wavelet Analysis"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Quentin Barthelemy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Performs a Time-Frequency Analysis using CWT."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Performs a Time-Frequency Analysis using Continuous Wavelet Transform."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("1.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("1.0.0"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-execute"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_ContinuousWaveletAnalysis; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmContinuousWaveletAnalysis(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);

				rPrototype.addOutput("Amplitude", OV_TypeId_TimeFrequency);
				rPrototype.addOutput("Phase", OV_TypeId_TimeFrequency);
				rPrototype.addOutput("Real Part", OV_TypeId_TimeFrequency);
				rPrototype.addOutput("Imaginary Part", OV_TypeId_TimeFrequency);

				rPrototype.addSetting("Wavelet type", OVP_TypeId_ContinuousWaveletType, OVP_TypeId_ContinuousWaveletType_Morlet.toString());
				rPrototype.addSetting("Wavelet parameter", OV_TypeId_Float, "4");
				rPrototype.addSetting("Number of frequencies", OV_TypeId_Integer, "60");
				rPrototype.addSetting("Highest frequency", OV_TypeId_Float, "35");
				rPrototype.addSetting("Frequency spacing", OV_TypeId_Float, "12.5");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_ContinuousWaveletAnalysisDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
