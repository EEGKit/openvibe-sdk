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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_ContinuousWaveletAnalysis)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmContinuousWaveletAnalysis> m_oDecoder;
			OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmContinuousWaveletAnalysis> m_vEncoder[4];

			const char* m_pWaveletType;
			double m_dWaveletParameter;
			int m_iScaleCount_J;
			double m_dHighestFrequency;
			double m_dSmallestScale_s0;
			double m_dScaleSpacing_dj;

			const char* m_pScaleType;
			int m_iScalePowerBase_a0;
			double m_dSamplingPeriod_dt;
			cwt_object m_oWaveletTransform = nullptr;
		};

		class CBoxAlgorithmContinuousWaveletAnalysisDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Continuous Wavelet Analysis"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Performs a Time-Frequency Analysis using CWT."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Performs a Time-Frequency Analysis using Continuous Wavelet Transform."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("1.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("1.0.0"); }
			virtual OpenViBE::CString getStockItemName() const { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_ContinuousWaveletAnalysis; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmContinuousWaveletAnalysis(); }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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
	}
}


