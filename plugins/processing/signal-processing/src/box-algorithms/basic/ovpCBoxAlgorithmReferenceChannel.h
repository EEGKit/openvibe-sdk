#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmReferenceChannel final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ReferenceChannel)

protected:

	Toolkit::TSignalDecoder<CBoxAlgorithmReferenceChannel> m_decoder;
	Toolkit::TSignalEncoder<CBoxAlgorithmReferenceChannel> m_encoder;
	size_t m_referenceChannelIdx = 0;
};

class CBoxAlgorithmReferenceChannelDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Reference Channel"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }

	CString getShortDescription() const override { return "Subtracts the value of the reference channel from all other channels"; }

	CString getDetailedDescription() const override { return "Reference channel must be specified as a parameter for the box"; }

	CString getCategory() const override { return "Signal processing/Channels"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ReferenceChannel; }
	IPluginObject* create() override { return new CBoxAlgorithmReferenceChannel; }
	// virtual IBoxListener* createBoxListener() const               { return new CBoxAlgorithmReferenceChannelListener; }
	// virtual void releaseBoxListener(IBoxListener* listener) const { delete listener; }
	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Output signal", OV_TypeId_Signal);
		prototype.addSetting("Channel", OV_TypeId_String, "Ref_Nose");
		prototype.addSetting("Channel Matching Method", OVP_TypeId_MatchMethod, "Smart");
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ReferenceChannelDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
