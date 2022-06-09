#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {
class CBoxAlgorithmClockStimulator final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	uint64_t getClockFrequency() override { return (1LL << 32) * 32; }
	bool initialize() override;
	bool uninitialize() override;
	bool processClock(Kernel::CMessageClock& msg) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ClockStimulator)

protected:
	Toolkit::TStimulationEncoder<CBoxAlgorithmClockStimulator> m_encoder;
	uint64_t m_stimulationID       = 0;
	uint64_t m_lastStimulationDate = 0;
	uint64_t m_lastEndTime         = 0;

	uint64_t m_nSentStimulation  = 0;
	double m_stimulationInterval = 0;
};

class CBoxAlgorithmClockStimulatorDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }

	CString getName() const override { return "Clock stimulator"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA/IRISA"; }
	CString getShortDescription() const override { return "Periodic stimulation generator"; }
	CString getDetailedDescription() const override { return "Triggers stimulation at fixed frequency"; }
	CString getCategory() const override { return "Stimulation"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ClockStimulator; }
	IPluginObject* create() override { return new CBoxAlgorithmClockStimulator; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addOutput("Generated stimulations", OV_TypeId_Stimulations);
		prototype.addSetting("Interstimulation interval (in sec)", OV_TypeId_Float, "1.0");
		prototype.addSetting("Stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClockStimulatorDesc)
};
}  // namespace Stimulation
}  // namespace Plugins
}  // namespace OpenViBE
