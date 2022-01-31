///-------------------------------------------------------------------------------------------------
/// 
/// \file CBoxXDAWNTrainer.hpp
/// \brief Class of the box XDAWN Trainer.
/// \author Yann Renard (Mensia Technologies SA).
/// \version 1.0.
/// \copyright (C) 2022 INRIA
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU Affero General Public License as published
/// by the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU Affero General Public License for more details.
///
/// You should have received a copy of the GNU Affero General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.
/// 
///-------------------------------------------------------------------------------------------------
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
/// <summary> The class CBoxXDAWNTrainer describes the box XDAWN Trainer. </summary>
class CBoxXDAWNTrainer final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	CBoxXDAWNTrainer();
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer)

protected:

	Toolkit::TStimulationDecoder<CBoxXDAWNTrainer> m_stimDecoder;
	std::array<Toolkit::TSignalDecoder<CBoxXDAWNTrainer>, 2> m_signalDecoder;
	Toolkit::TStimulationEncoder<CBoxXDAWNTrainer> m_stimEncoder;

	uint64_t m_trainStimulationID = 0;
	std::string m_filename;
	size_t m_filterDim     = 0;
	bool m_saveAsBoxConfig = false;

	bool saveFilter(const CMatrix& m, const size_t nChannel);
};

/// <summary> Descriptor of the box XDAWN Trainer. </summary>
class CBoxXDAWNTrainerDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override {}
	CString getName() const override { return "xDAWN Trainer"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "Mensia Technologies SA"; }
	CString getShortDescription() const override { return "Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300"; }
	CString getDetailedDescription() const override { return "Trains spatial filters that best highlight Evoked Response Potentials (ERP) such as P300"; }
	CString getCategory() const override { return "Signal processing/Spatial Filtering"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CString getStockItemName() const override { return "gtk-zoom-100"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainer; }
	IPluginObject* create() override { return new CBoxXDAWNTrainer; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Stimulations", OV_TypeId_Stimulations);
		prototype.addInput("Session signal", OV_TypeId_Signal);
		prototype.addInput("Evoked potential epochs", OV_TypeId_Signal);

		prototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

		prototype.addSetting("Train stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
		prototype.addSetting("Spatial filter configuration", OV_TypeId_Filename, "${Player_ScenarioDirectory}/config.xml");
		prototype.addSetting("Filter dimension", OV_TypeId_Integer, "4");
		prototype.addSetting("Save as box config", OV_TypeId_Boolean, "true");
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_InriaXDAWNTrainerDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
