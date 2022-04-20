#pragma once

#include "defines.hpp"
#include <toolkit/ovtk_all.h>
#include <wavelib/header/wavelib.h>
#include <array>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmContinuousWaveletAnalysis final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_ContinuousWaveletAnalysis)

protected:

	Toolkit::TSignalDecoder<CBoxAlgorithmContinuousWaveletAnalysis> m_decoder;
	std::array<Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmContinuousWaveletAnalysis>, 4> m_encoders;

	const char* m_waveletType = nullptr;
	double m_waveletParam     = 0;
	size_t m_nScaleJ          = 0;
	double m_highestFreq      = 0;
	double m_smallestScaleS0  = 0;
	double m_scaleSpacingDj   = 0;

	const char* m_scaleType       = nullptr;
	int m_scalePowerBaseA0        = 0;
	double m_samplingPeriodDt     = 0;
	cwt_object m_waveletTransform = nullptr;
};

class CBoxAlgorithmContinuousWaveletAnalysisDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Continuous Wavelet Analysis"); }
	CString getAuthorName() const override { return CString("Quentin Barthelemy"); }
	CString getAuthorCompanyName() const override { return CString("Mensia Technologies SA"); }
	CString getShortDescription() const override { return CString("Performs a Time-Frequency Analysis using CWT."); }

	CString getDetailedDescription() const override { return CString("Performs a Time-Frequency Analysis using Continuous Wavelet Transform."); }

	CString getCategory() const override { return CString("Signal processing/Spectral Analysis"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("1.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("1.0.0"); }
	CString getStockItemName() const override { return CString("gtk-execute"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_ContinuousWaveletAnalysis; }
	IPluginObject* create() override { return new CBoxAlgorithmContinuousWaveletAnalysis(); }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);

		prototype.addOutput("Amplitude", OV_TypeId_TimeFrequency);
		prototype.addOutput("Phase", OV_TypeId_TimeFrequency);
		prototype.addOutput("Real Part", OV_TypeId_TimeFrequency);
		prototype.addOutput("Imaginary Part", OV_TypeId_TimeFrequency);

		prototype.addSetting("Wavelet type", OVP_TypeId_ContinuousWaveletType, "Morlet wavelet");
		prototype.addSetting("Wavelet parameter", OV_TypeId_Float, "4");
		prototype.addSetting("Number of frequencies", OV_TypeId_Integer, "60");
		prototype.addSetting("Highest frequency", OV_TypeId_Float, "35");
		prototype.addSetting("Frequency spacing", OV_TypeId_Float, "12.5");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_ContinuousWaveletAnalysisDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
