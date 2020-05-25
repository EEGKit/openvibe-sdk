#pragma once

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {

/**
 * \class CBoxAlgorithmTimeout
 * \author Jozef Legény (Inria)
 * \date Thu Mar 21 14:40:23 2013
 * \brief The class CBoxAlgorithmTimeout describes the box Timeout.
 *
 */
class CBoxAlgorithmTimeout final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processClock(CMessage& msg) override;
	bool processInput(const size_t index) override;
	uint64_t getClockFrequency() override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_Timeout)

protected:
	Toolkit::TStimulationEncoder<CBoxAlgorithmTimeout> m_encoder;

private:
	enum class ETimeoutState { No, Occurred, Sent };

	ETimeoutState m_timeoutState = ETimeoutState::No;
	bool m_isHeaderSent          = false;

	uint64_t m_timeout           = 0;
	uint64_t m_lastTimePolled    = 0;
	uint64_t m_previousTime      = 0;
	uint64_t m_stimulationToSend = 0;
};

/**
 * \class CBoxAlgorithmTimeoutDesc
 * \author Jozef Legény (Inria)
 * \date Thu Mar 21 14:40:23 2013
 * \brief Descriptor of the box Timeout.
 *
 */
class CBoxAlgorithmTimeoutDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Timeout"; }
	CString getAuthorName() const override { return "Jozef Legény"; }
	CString getAuthorCompanyName() const override { return "Inria"; }
	CString getShortDescription() const override { return "Sends a stimulation after a period of time without receiving signal"; }
	CString getDetailedDescription() const override
	{
		return "Sends a stimulation after a period of time without receiving signal. Useful for stopping scenarios after hardware disconnection.";
	}

	CString getCategory() const override { return "Stimulation"; }
	CString getVersion() const override { return "1.1"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Timeout; }
	IPluginObject* create() override { return new CBoxAlgorithmTimeout; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input Stream",OV_TypeId_StreamedMatrix);

		prototype.addOutput("Output Stimulations",OV_TypeId_Stimulations);

		prototype.addSetting("Timeout delay",OV_TypeId_Integer, "5");
		prototype.addSetting("Output Stimulation",OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeoutDesc)
};

} // namespace Stimulation
} // namespace Plugins
} // namespace OpenViBE
