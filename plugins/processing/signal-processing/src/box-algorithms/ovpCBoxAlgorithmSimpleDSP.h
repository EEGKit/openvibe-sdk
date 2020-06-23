#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>
#include "../ovpCSimpleDSP/ovpCEquationParser.h"

#include <vector>
#include <cstdio>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmSimpleDSP final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:

	CBoxAlgorithmSimpleDSP() { }
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;
	void evaluate();

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SimpleDSP)

protected:
	std::vector<Kernel::IAlgorithmProxy*> m_decoders;
	Kernel::IAlgorithmProxy* m_encoder = nullptr;
	std::vector<IMatrix*> m_matrices;

	CEquationParser* m_parser = nullptr;

	uint64_t m_equationType = OP_USERDEF;
	double m_equationParam  = 0;
	double** m_variables    = nullptr;

	bool m_checkDates = false;
};

class CBoxAlgorithmSimpleDSPListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputAdded(Kernel::IBox& box, const size_t index) override
	{
		char name[1024];
		sprintf(name, "Input - %c", char('A' + index));
		CIdentifier typeID = CIdentifier::undefined();
		box.getOutputType(0, typeID);
		box.setInputType(index, typeID);
		box.setInputName(index, name);
		return true;
	}

	bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getInputType(index, typeID);
		box.setOutputType(0, typeID);
		for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, typeID); }
		return true;
	}

	bool onOutputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getOutputType(index, typeID);
		box.setOutputType(0, typeID);
		for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, typeID); }
		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, CIdentifier::undefined())
};

class CBoxAlgorithmSimpleDSPDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Simple DSP"; }
	CString getAuthorName() const override { return "Bruno Renier / Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA / IRISA"; }
	CString getShortDescription() const override { return "Apply mathematical formulaes to matrices."; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Signal processing/Basic"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SimpleDSP; }
	IPluginObject* create() override { return new CBoxAlgorithmSimpleDSP(); }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmSimpleDSPListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input - A", OV_TypeId_Signal);
		prototype.addOutput("Output", OV_TypeId_Signal);
		prototype.addSetting("Equation", OV_TypeId_String, "x");

		prototype.addFlag(Kernel::BoxFlag_CanAddInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);

		prototype.addInputSupport(OV_TypeId_StreamedMatrix);
		prototype.addInputSupport(OV_TypeId_FeatureVector);
		prototype.addInputSupport(OV_TypeId_Signal);
		prototype.addInputSupport(OV_TypeId_Spectrum);

		prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
		prototype.addOutputSupport(OV_TypeId_FeatureVector);
		prototype.addOutputSupport(OV_TypeId_Signal);
		prototype.addOutputSupport(OV_TypeId_Spectrum);

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SimpleDSPDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
