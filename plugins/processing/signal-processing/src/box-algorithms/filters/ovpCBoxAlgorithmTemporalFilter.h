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
		class CBoxAlgorithmTemporalFilter final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_TemporalFilter)

		protected:

			OpenViBE::Toolkit::TSignalDecoder<CBoxAlgorithmTemporalFilter> m_decoder;
			OpenViBE::Toolkit::TSignalEncoder<CBoxAlgorithmTemporalFilter> m_encoder;


			size_t m_method = 0;
			size_t m_type   = 0;
			size_t m_order  = 0;

			double m_lowCut  = 0;
			double m_highCut = 0;
			double m_ripple  = 0; // for Chebyshev

			std::vector<std::shared_ptr<Dsp::Filter>> m_filters;
			//std::vector < std::shared_ptr < Dsp::Filter > > m_filters;

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
				prototype.addSetting("Filter Method", OVP_TypeId_FilterMethod, "Butterworth");
				prototype.addSetting("Filter Type", OVP_TypeId_FilterType, "Band Pass");
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
