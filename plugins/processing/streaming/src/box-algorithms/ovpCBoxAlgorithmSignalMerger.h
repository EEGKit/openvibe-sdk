#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>
#include <vector>

namespace OpenViBE {
namespace Plugins {
namespace Streaming {
class CBoxAlgorithmSignalMerger final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SignalMerger)

protected:

	std::vector<Toolkit::TSignalDecoder<CBoxAlgorithmSignalMerger>*> m_decoders;
	Toolkit::TSignalEncoder<CBoxAlgorithmSignalMerger>* m_encoder = nullptr;
};

class CBoxAlgorithmSignalMergerListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputAdded(Kernel::IBox& box, const size_t index) override
	{
		box.setInputName(index, ("Input " + std::to_string(index + 1)).c_str());
		box.setInputType(index, OV_TypeId_Signal);
		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
};

class CBoxAlgorithmSignalMergerDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Signal Merger"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }
	CString getShortDescription() const override { return "Merges several input streams into a single output stream"; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Streaming"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalMerger; }
	IPluginObject* create() override { return new CBoxAlgorithmSignalMerger; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmSignalMergerListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input 1", OV_TypeId_Signal);
		prototype.addInput("Input 2", OV_TypeId_Signal);
		prototype.addOutput("Merged", OV_TypeId_Signal);
		prototype.addFlag(Kernel::BoxFlag_CanAddInput);

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalMergerDesc)
};
}  // namespace Streaming
}  // namespace Plugins
}  // namespace OpenViBE
