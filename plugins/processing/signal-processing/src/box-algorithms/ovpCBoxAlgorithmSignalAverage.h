#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>

#include <string>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
/**
*/
class CBoxAlgorithmSignalAverage final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:

	CBoxAlgorithmSignalAverage() {}
	void release() override {}
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SignalAverage)

protected:
	void computeAverage();

	// Needed to read the input and write the output
	Toolkit::TSignalDecoder<CBoxAlgorithmSignalAverage> m_decoder;
	Toolkit::TSignalEncoder<CBoxAlgorithmSignalAverage> m_encoder;
};

/**
* Description of the channel selection plugin
*/
class CSignalAverageDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Signal average"; }
	CString getAuthorName() const override { return "Bruno Renier"; }
	CString getAuthorCompanyName() const override { return "INRIA/IRISA"; }
	CString getShortDescription() const override { return "Computes the average of each input buffer."; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Signal processing/Averaging"; }
	CString getVersion() const override { return "0.5"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalAverage; }
	IPluginObject* create() override { return new CBoxAlgorithmSignalAverage(); }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Filtered signal", OV_TypeId_Signal);

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalAverageDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
