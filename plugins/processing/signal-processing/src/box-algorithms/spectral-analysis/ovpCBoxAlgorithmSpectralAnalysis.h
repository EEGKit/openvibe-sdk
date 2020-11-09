#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmSpectralAnalysis final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_SpectralAnalysis)

protected:
	Toolkit::TSignalDecoder<CBoxAlgorithmSpectralAnalysis> m_decoder;

	std::vector<Toolkit::TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>*> m_spectrumEncoders;
	std::vector<bool> m_isSpectrumEncoderActive;

	size_t m_nChannel = 0;
	size_t m_nSample  = 0;
	size_t m_sampling = 0;

	size_t m_sizeFFT = 0;

	IMatrix* m_frequencyAbscissa = nullptr;
};

class CBoxAlgorithmSpectralAnalysisDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Spectral Analysis"); }
	CString getAuthorName() const override { return CString("Laurent Bonnet / Quentin Barthelemy"); }
	CString getAuthorCompanyName() const override { return CString("Mensia Technologies SA"); }
	CString getShortDescription() const override { return CString("Performs a Spectral Analysis using FFT."); }
	CString getDetailedDescription() const override { return CString("Performs a Spectral Analysis using FFT."); }
	CString getCategory() const override { return CString("Signal processing/Spectral Analysis"); }
	CString getVersion() const override { return CString("1.2"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.1.0"); }
	CString getStockItemName() const override { return CString("gtk-execute"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_SpectralAnalysis; }
	IPluginObject* create() override { return new CBoxAlgorithmSpectralAnalysis(); }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);

		prototype.addOutput("Amplitude", OV_TypeId_Spectrum);
		prototype.addOutput("Phase", OV_TypeId_Spectrum);
		prototype.addOutput("Real Part", OV_TypeId_Spectrum);
		prototype.addOutput("Imaginary Part", OV_TypeId_Spectrum);

		prototype.addSetting("Amplitude", OV_TypeId_Boolean, "true");
		prototype.addSetting("Phase", OV_TypeId_Boolean, "false");
		prototype.addSetting("Real Part", OV_TypeId_Boolean, "false");
		prototype.addSetting("Imaginary Part", OV_TypeId_Boolean, "false");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_SpectralAnalysisDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
