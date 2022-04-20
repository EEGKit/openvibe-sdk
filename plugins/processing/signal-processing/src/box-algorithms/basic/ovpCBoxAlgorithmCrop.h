#pragma once

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmCrop final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_Crop)

protected:

	CMatrix* m_matrix                  = nullptr;
	Kernel::IAlgorithmProxy* m_decoder = nullptr;
	Kernel::IAlgorithmProxy* m_encoder = nullptr;
	double m_minCropValue              = 0;
	double m_maxCropValue              = 0;
	ECropMethod m_cropMethod           = ECropMethod::MinMax;
};

class CBoxAlgorithmCropListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getInputType(index, typeID);
		box.setOutputType(index, typeID);
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

class CBoxAlgorithmCropDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Crop"); }
	CString getAuthorName() const override { return CString("Yann Renard"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
	CString getShortDescription() const override { return CString("Truncates signal values to a specified range"); }
	CString getDetailedDescription() const override { return CString("Minimum or maximum or both limits can be specified"); }
	CString getCategory() const override { return CString("Signal processing/Basic"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Crop; }
	IPluginObject* create() override { return new CBoxAlgorithmCrop; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmCropListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input matrix", OV_TypeId_StreamedMatrix);
		prototype.addOutput("Output matrix", OV_TypeId_StreamedMatrix);
		prototype.addSetting("Crop method", OVP_TypeId_CropMethod, "MinMax");
		prototype.addSetting("Min crop value", OV_TypeId_Float, "-1");
		prototype.addSetting("Max crop value", OV_TypeId_Float, "1");
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);

		prototype.addInputSupport(OV_TypeId_Signal);
		prototype.addInputSupport(OV_TypeId_Spectrum);
		prototype.addInputSupport(OV_TypeId_StreamedMatrix);
		prototype.addInputSupport(OV_TypeId_FeatureVector);

		prototype.addOutputSupport(OV_TypeId_Signal);
		prototype.addOutputSupport(OV_TypeId_Spectrum);
		prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
		prototype.addOutputSupport(OV_TypeId_FeatureVector);
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CropDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
