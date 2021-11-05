#pragma once

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmSpatialFilter final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SpatialFilter)

protected:

	Toolkit::TDecoder<CBoxAlgorithmSpatialFilter>* m_decoder = nullptr;
	Toolkit::TEncoder<CBoxAlgorithmSpatialFilter>* m_encoder = nullptr;

	CMatrix m_filterBank;

private:
	// Loads the m_vCoefficient vector (representing a matrix) from the given string. c1 and c2 are separator characters between floats.
	size_t loadCoefs(const CString& coefs, char c1, char c2, size_t nRows, size_t nCols);
};

class CBoxAlgorithmSpatialFilterListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputTypeChanged(Kernel::IBox& box, const size_t /*index*/) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getInputType(0, typeID);
		box.setOutputType(0, typeID);
		return true;
	}

	bool onOutputTypeChanged(Kernel::IBox& box, const size_t /*index*/) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getOutputType(0, typeID);
		box.setInputType(0, typeID);
		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, CIdentifier::undefined())
};

class CBoxAlgorithmSpatialFilterDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Spatial Filter"); }
	CString getAuthorName() const override { return CString("Yann Renard, Jussi T. Lindgren"); }
	CString getAuthorCompanyName() const override { return CString("Inria"); }

	CString getShortDescription() const override { return CString("Maps M inputs to N outputs by multiplying the each input vector with a matrix"); }

	CString getDetailedDescription() const override
	{
		return CString(
			"The applied coefficient matrix must be specified as a box parameter. The filter processes each sample independently of the past samples.");
	}

	CString getCategory() const override { return CString("Signal processing/Filtering"); }
	CString getVersion() const override { return CString("1.1"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.1.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SpatialFilter; }
	IPluginObject* create() override { return new CBoxAlgorithmSpatialFilter; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmSpatialFilterListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input Signal", OV_TypeId_Signal);
		prototype.addOutput("Output Signal", OV_TypeId_Signal);
		prototype.addSetting("Spatial Filter Coefficients", OV_TypeId_String, "1;0;0;0;0;1;0;0;0;0;1;0;0;0;0;1");
		prototype.addSetting("Number of Output Channels", OV_TypeId_Integer, "4");
		prototype.addSetting("Number of Input Channels", OV_TypeId_Integer, "4");
		prototype.addSetting("Filter matrix file", OV_TypeId_Filename, "");
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);

		prototype.addInputSupport(OV_TypeId_StreamedMatrix);
		prototype.addInputSupport(OV_TypeId_Spectrum);
		prototype.addInputSupport(OV_TypeId_Signal);

		prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
		prototype.addOutputSupport(OV_TypeId_Spectrum);
		prototype.addOutputSupport(OV_TypeId_Signal);

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SpatialFilterDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
