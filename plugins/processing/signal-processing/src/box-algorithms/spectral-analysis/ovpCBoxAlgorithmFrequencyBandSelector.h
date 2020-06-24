#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>
#include <algorithm>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
typedef std::pair<double, double> BandRange;

class CBoxAlgorithmFrequencyBandSelector final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_FrequencyBandSelector)

protected:

	Kernel::IAlgorithmProxy* m_decoder = nullptr;
	Kernel::TParameterHandler<const IMemoryBuffer*> ip_buffer;
	Kernel::TParameterHandler<CMatrix*> op_matrix;
	Kernel::TParameterHandler<CMatrix*> op_bands;

	Kernel::IAlgorithmProxy* m_encoder = nullptr;
	Kernel::TParameterHandler<CMatrix*> ip_matrix;
	Kernel::TParameterHandler<CMatrix*> ip_frequencyAbscissa;
	Kernel::TParameterHandler<IMemoryBuffer*> op_buffer;

	CMatrix m_oMatrix;
	std::vector<BandRange> m_selecteds;
	std::vector<double> m_selectionFactors;
};

class CBoxAlgorithmFrequencyBandSelectorDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Frequency Band Selector"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }

	CString getShortDescription() const override
	{
		return "Preserves some spectrum coefficients and puts the others to zero depending on a list of frequencies / frequency bands to select";
	}

	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Signal processing/Spectral Analysis"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_FrequencyBandSelector; }
	IPluginObject* create() override { return new CBoxAlgorithmFrequencyBandSelector; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input spectrum", OV_TypeId_Spectrum);
		prototype.addOutput("Output spectrum", OV_TypeId_Spectrum);
		prototype.addSetting("Frequencies to select", OV_TypeId_String, "8:12;16:24");
		// @fixme Use OV_Value_RangeStringSeparator / OV_Value_EnumeratedStringSeparator tokens above

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
