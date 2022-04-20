#pragma once

#include "defines.hpp"
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmIdentity final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_Identity)
};

class CBoxAlgorithmIdentityListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:

	static bool check(Kernel::IBox& box)
	{
		size_t i;
		for (i = 0; i < box.getInputCount(); ++i) { box.setInputName(i, ("Input stream " + std::to_string(i + 1)).c_str()); }
		for (i = 0; i < box.getOutputCount(); ++i) { box.setOutputName(i, ("Output stream " + std::to_string(i + 1)).c_str()); }
		return true;
	}

	bool onDefaultInitialized(Kernel::IBox& box) override
	{
		box.setInputType(0, OV_TypeId_Signal);
		box.setOutputType(0, OV_TypeId_Signal);
		return true;
	}

	bool onInputAdded(Kernel::IBox& box, const size_t index) override
	{
		box.setInputType(index, OV_TypeId_Signal);
		box.addOutput("", OV_TypeId_Signal, box.getUnusedInputIdentifier());
		check(box);
		return true;
	}

	bool onInputRemoved(Kernel::IBox& box, const size_t index) override
	{
		box.removeOutput(index);
		check(box);
		return true;
	}

	bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getInputType(index, typeID);
		box.setOutputType(index, typeID);
		return true;
	}

	bool onOutputAdded(Kernel::IBox& box, const size_t index) override
	{
		box.setOutputType(index, OV_TypeId_Signal);
		box.addInput("", OV_TypeId_Signal, box.getUnusedOutputIdentifier());
		check(box);
		return true;
	}

	bool onOutputRemoved(Kernel::IBox& box, const size_t index) override
	{
		box.removeInput(index);
		check(box);
		return true;
	}

	bool onOutputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getOutputType(index, typeID);
		box.setInputType(index, typeID);
		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, CIdentifier::undefined())
};

class CBoxAlgorithmIdentityDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Identity"); }
	CString getAuthorName() const override { return CString("Yann Renard"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
	CString getShortDescription() const override { return CString("Duplicates input to output"); }
	CString getDetailedDescription() const override { return CString("This simply duplicates intput on its output"); }
	CString getCategory() const override { return CString("Signal processing/Basic"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Identity; }
	IPluginObject* create() override { return new CBoxAlgorithmIdentity(); }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmIdentityListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input stream", OV_TypeId_Signal);
		prototype.addOutput("Output stream", OV_TypeId_Signal);
		prototype.addFlag(Kernel::BoxFlag_CanAddOutput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);
		prototype.addFlag(Kernel::BoxFlag_CanAddInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_IdentityDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
