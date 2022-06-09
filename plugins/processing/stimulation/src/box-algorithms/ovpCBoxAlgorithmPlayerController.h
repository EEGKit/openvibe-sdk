#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {
class CBoxAlgorithmPlayerController final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_PlayerController)

protected:
	Kernel::IAlgorithmProxy* m_decoder = nullptr;
	Kernel::TParameterHandler<const IMemoryBuffer*> ip_buffer;
	Kernel::TParameterHandler<CStimulationSet*> op_stimulationSet;

	uint64_t m_stimulationID = 0;
	uint64_t m_actionID      = 0;
};

class CBoxAlgorithmPlayerControllerDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }

	CString getName() const override { return "Player Controller"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }
	CString getShortDescription() const override { return "Controls the player execution"; }
	CString getDetailedDescription() const override { return "Add some settings to configure the way you want to control the player"; }
	CString getCategory() const override { return "Stimulation"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_PlayerController; }
	IPluginObject* create() override { return new CBoxAlgorithmPlayerController; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Stimulations", OV_TypeId_Stimulations);
		prototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
		prototype.addSetting("Action to perform", OV_TypeId_PlayerAction, OV_TypeId_PlayerAction_Pause.toString());

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_PlayerControllerDesc)
};
}  // namespace Stimulation
}  // namespace Plugins
}  // namespace OpenViBE
