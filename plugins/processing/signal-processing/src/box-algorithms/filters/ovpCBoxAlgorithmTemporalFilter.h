#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <memory>
#include <dsp-filters/Dsp.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmTemporalFilter final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TemporalFilter)

protected:

	Toolkit::TSignalDecoder<CBoxAlgorithmTemporalFilter> m_decoder;
	Toolkit::TSignalEncoder<CBoxAlgorithmTemporalFilter> m_encoder;


	EFilterMethod m_method = EFilterMethod::Butterworth;
	EFilterType m_type     = EFilterType::BandPass;
	size_t m_order         = 0;

	double m_lowCut  = 0;
	double m_highCut = 0;
	double m_ripple  = 0; // for Chebyshev

	std::vector<std::shared_ptr<Dsp::Filter>> m_filters;
	//std::vector < std::shared_ptr < Dsp::Filter > > m_filters;

	std::vector<double> m_firstSamples;
};

class CBoxAlgorithmTemporalFilterDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Temporal Filter"; }
	CString getAuthorName() const override { return "Yann Renard & Laurent Bonnet"; }
	CString getAuthorCompanyName() const override { return "Mensia Technologies SA"; }

	CString getShortDescription() const override { return "Temporal filtering based on various one-way IIR filter designs"; }

	CString getDetailedDescription() const override
	{
		return "Applies a temporal filter, based on various one-way IIR filter designs, to the input stream.";
	}

	CString getCategory() const override { return "Signal processing/Temporal Filtering"; }
	CString getVersion() const override { return "1.1"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_TemporalFilter; }
	IPluginObject* create() override { return new CBoxAlgorithmTemporalFilter; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
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

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TemporalFilterDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
