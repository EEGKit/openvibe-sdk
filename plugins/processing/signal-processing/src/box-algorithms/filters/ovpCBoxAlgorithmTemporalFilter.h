#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <memory>
#include <dsp-filters/Dsp.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmTemporalFilter final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_TemporalFilter)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmTemporalFilter> m_oDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmTemporalFilter> m_oEncoder;

			uint64_t m_filterMethod = 0;
			uint64_t m_filterType   = 0;
			uint64_t m_filterOrder  = 0;

			double m_lowCutFrequency  = 0;
			double m_highCutFrequency = 0;
			double m_bandPassRipple   = 0; // for Chebyshev

			std::vector<std::shared_ptr<Dsp::Filter>> m_filters;
			//std::vector < std::shared_ptr < Dsp::Filter > > m_vFilter2;

			std::vector<double> m_firstSamples;
		};

		class CBoxAlgorithmTemporalFilterDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Temporal Filter"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard & Laurent Bonnet"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Temporal filtering based on various one-way IIR filter designs");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Applies a temporal filter, based on various one-way IIR filter designs, to the input stream.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_TemporalFilter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmTemporalFilter; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Output signal", OV_TypeId_Signal);
				prototype.addSetting("Filter Method", OVP_TypeId_FilterMethod, OVP_TypeId_FilterMethod_Butterworth.toString());
				prototype.addSetting("Filter Type", OVP_TypeId_FilterType, OVP_TypeId_FilterType_BandPass.toString());
				prototype.addSetting("Filter Order", OV_TypeId_Integer, "4");
				prototype.addSetting("Low Cut-off Frequency (Hz)", OV_TypeId_Float, "1");
				prototype.addSetting("High Cut-off Frequency (Hz)", OV_TypeId_Float, "40");
				prototype.addSetting("Band Pass Ripple (dB)", OV_TypeId_Float, "0.5");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TemporalFilterDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
