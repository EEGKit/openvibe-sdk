#pragma once

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Eigen>
#include <array>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmXDAWNTrainer final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	CBoxAlgorithmXDAWNTrainer();
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer)

protected:

	Toolkit::TStimulationDecoder<CBoxAlgorithmXDAWNTrainer> m_stimDecoder;
	std::array<Toolkit::TSignalDecoder<CBoxAlgorithmXDAWNTrainer>, 2> m_signalDecoder;
	Toolkit::TStimulationEncoder<CBoxAlgorithmXDAWNTrainer> m_stimEncoder;

	uint64_t m_trainStimulationID = 0;
	CString m_filterFilename;
	size_t m_filterDim     = 0;
	bool m_saveAsBoxConfig = false;
};

class CBoxAlgorithmXDAWNTrainerDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override {}
	CString getName() const override { return CString("xDAWN Trainer"); }
	CString getAuthorName() const override { return CString("Yann Renard"); }
	CString getAuthorCompanyName() const override { return CString("Mensia Technologies SA"); }

	CString getShortDescription() const override { return CString("Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300"); }

	CString getDetailedDescription() const override
	{
		return CString("Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300");
	}

	CString getCategory() const override { return CString("Signal processing/Spatial Filtering"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getStockItemName() const override { return CString("gtk-zoom-100"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer; }
	IPluginObject* create() override { return new CBoxAlgorithmXDAWNTrainer; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Stimulations", OV_TypeId_Stimulations);
		prototype.addInput("Session signal", OV_TypeId_Signal);
		prototype.addInput("Evoked potential epochs", OV_TypeId_Signal);

		prototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

		prototype.addSetting("Train stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
		prototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "");
		prototype.addSetting("Filter dimension", OV_TypeId_Integer, "4");
		prototype.addSetting("Save as box config", OV_TypeId_Boolean, "true");
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainerDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
