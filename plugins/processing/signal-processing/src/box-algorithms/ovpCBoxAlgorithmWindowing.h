#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>

#include <vector>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmWindowing final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_Windowing)

protected:
	Toolkit::TSignalDecoder<CBoxAlgorithmWindowing> m_decoder;
	Toolkit::TSignalEncoder<CBoxAlgorithmWindowing> m_encoder;

	EWindowMethod m_windowMethod = EWindowMethod::None;
	std::vector<double> m_windowCoefs;
};

class CBoxAlgorithmWindowingDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Windowing"); }
	CString getAuthorName() const override { return CString("Laurent Bonnet"); }
	CString getAuthorCompanyName() const override { return CString("Mensia Technologies SA"); }
	CString getShortDescription() const override { return CString("Applies a windowing function to the signal."); }
	CString getDetailedDescription() const override { return CString("Applies a windowing function to the signal."); }
	CString getCategory() const override { return CString("Signal processing/Temporal Filtering"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getIconName() const override { return CString("gtk-execute"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_Windowing; }
	IPluginObject* create() override { return new CBoxAlgorithmWindowing(); }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Output signal", OV_TypeId_Signal);
		prototype.addSetting("Window method", OVP_TypeId_WindowMethod, "Hamming");
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_WindowingDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
