#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <memory>
#include <dsp-filters/Dsp.h>

#define OVP_ClassId_BoxAlgorithm_TemporalFilter     OpenViBE::CIdentifier(0xB4F9D042, 0x9D79F2E5)
#define OVP_ClassId_BoxAlgorithm_TemporalFilterDesc OpenViBE::CIdentifier(0x7BF6BA62, 0xAF829A37)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmTemporalFilter : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_TemporalFilter)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmTemporalFilter> m_oDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmTemporalFilter> m_oEncoder;

			uint64_t m_ui64FilterMethod;
			uint64_t m_ui64FilterType;
			uint64_t m_ui64FilterOrder;

			double m_f64LowCutFrequency;
			double m_f64HighCutFrequency;
			double m_f64BandPassRipple; // for Chebyshev

			std::vector<std::shared_ptr<Dsp::Filter>> m_vFilter;
			//std::vector < std::shared_ptr < Dsp::Filter > > m_vFilter2;

			std::vector<double> m_vFirstSample;
		};

		class CBoxAlgorithmTemporalFilterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Temporal Filter"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard & Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Temporal filtering based on various one-way IIR filter designs"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Applies a temporal filter, based on various one-way IIR filter designs, to the input stream."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.1"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_TemporalFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmTemporalFilter; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Filter Method", OVP_TypeId_FilterMethod, OVP_TypeId_FilterMethod_Butterworth.toString());
				rBoxAlgorithmPrototype.addSetting("Filter Type", OVP_TypeId_FilterType, OVP_TypeId_FilterType_BandPass.toString());
				rBoxAlgorithmPrototype.addSetting("Filter Order", OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addSetting("Low Cut-off Frequency (Hz)", OV_TypeId_Float, "1");
				rBoxAlgorithmPrototype.addSetting("High Cut-off Frequency (Hz)", OV_TypeId_Float, "40");
				rBoxAlgorithmPrototype.addSetting("Band Pass Ripple (dB)", OV_TypeId_Float, "0.5");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TemporalFilterDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins
