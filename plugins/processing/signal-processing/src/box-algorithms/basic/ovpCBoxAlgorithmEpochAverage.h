#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmEpochAverage final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_EpochAverage)

protected:

	Kernel::IAlgorithmProxy* m_decoder       = nullptr;
	Kernel::IAlgorithmProxy* m_encoder       = nullptr;
	Kernel::IAlgorithmProxy* m_matrixAverage = nullptr;

	Kernel::TParameterHandler<uint64_t> ip_matrixCount;
	Kernel::TParameterHandler<uint64_t> ip_averagingMethod;
};

class CBoxAlgorithmEpochAverageListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = OV_UndefinedIdentifier;
		box.getInputType(index, typeID);
		box.setOutputType(index, typeID);
		return true;
	}

	bool onOutputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = OV_UndefinedIdentifier;
		box.getOutputType(index, typeID);
		box.setInputType(index, typeID);
		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
};

class CBoxAlgorithmEpochAverageDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Epoch average"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA/IRISA"; }

	CString getShortDescription() const override { return "Averages matrices among time, this can be used to enhance ERPs"; }

	CString getDetailedDescription() const override
	{
		return "This box can average matrices of different types including signal, spectrum or feature vectors";
	}

	CString getCategory() const override { return "Signal processing/Averaging"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_EpochAverage; }
	IPluginObject* create() override { return new CBoxAlgorithmEpochAverage(); }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmEpochAverageListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input epochs", OV_TypeId_StreamedMatrix);
		prototype.addOutput("Averaged epochs", OV_TypeId_StreamedMatrix);
		prototype.addSetting("Averaging type", OVP_TypeId_EpochAverageMethod, "Moving epoch average");
		prototype.addSetting("Epoch count", OV_TypeId_Integer, "4");
		prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);

		prototype.addInputSupport(OV_TypeId_Signal);
		prototype.addInputSupport(OV_TypeId_Spectrum);
		prototype.addInputSupport(OV_TypeId_StreamedMatrix);
		prototype.addInputSupport(OV_TypeId_FeatureVector);
		prototype.addInputSupport(OV_TypeId_TimeFrequency);

		prototype.addOutputSupport(OV_TypeId_Signal);
		prototype.addOutputSupport(OV_TypeId_Spectrum);
		prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
		prototype.addOutputSupport(OV_TypeId_FeatureVector);
		prototype.addOutputSupport(OV_TypeId_TimeFrequency);
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EpochAverageDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
