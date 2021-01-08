#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <map>
#include <deque>

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {
class CBoxAlgorithmStimulationVoter final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StimulationVoter)

protected:

	uint64_t m_minimumVotes         = 0;
	double m_timeWindow             = 0;
	uint64_t m_rejectClassLabel     = 0;
	CIdentifier m_clearVotes        = CIdentifier::undefined();
	CIdentifier m_outputDateMode    = CIdentifier::undefined();
	CIdentifier m_rejectClassCanWin = CIdentifier::undefined();

private:

	Kernel::IAlgorithmProxy* m_encoder = nullptr;
	Kernel::IAlgorithmProxy* m_decoder = nullptr;
	Kernel::TParameterHandler<const IMemoryBuffer*> ip_buffer;
	Kernel::TParameterHandler<IStimulationSet*> op_stimulationSet;

	std::deque<std::pair<uint64_t, uint64_t>> m_oStimulusDeque; // <label,time>

	uint64_t m_latestStimulusDate = 0;
	uint64_t m_lastTime           = 0;
};


class CBoxAlgorithmStimulationVoterListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:

	CBoxAlgorithmStimulationVoterListener() : m_inputTypeID(OV_TypeId_Stimulations) { }

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, CIdentifier::undefined())

protected:

	CIdentifier m_inputTypeID = CIdentifier::undefined();
};

class CBoxAlgorithmStimulationVoterDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Stimulation Voter"); }
	CString getAuthorName() const override { return CString("Jussi T. Lindgren"); }
	CString getAuthorCompanyName() const override { return CString("Inria"); }
	CString getShortDescription() const override { return CString("Performs majority vote on the input stimuli"); }

	CString getDetailedDescription() const override
	{
		return CString(
			"Votes the most frequent stimulus ID in a given time window. Outputs the winning stimulus type. Several options are possible. To process multiple inputs, use Stimulation Multiplexer first.");
	}

	CString getCategory() const override { return CString("Streaming"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationVoter; }
	IPluginObject* create() override { return new CBoxAlgorithmStimulationVoter; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Stimulus input", OV_TypeId_Stimulations);
		prototype.addOutput("Selected stimulus", OV_TypeId_Stimulations);
		prototype.addSetting("Number of stimuli required for vote", OV_TypeId_Integer, "4");
		prototype.addSetting("Time window (secs)", OV_TypeId_Float, "2");
		prototype.addSetting("Clear votes", OVP_TypeId_Voting_ClearVotes, "After output");
		prototype.addSetting("Output timestamp", OVP_TypeId_Voting_OutputTime, "Time of last voting stimulus");
		prototype.addSetting("Reject class label", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
		prototype.addSetting("Reject class can win", OVP_TypeId_Voting_RejectClass_CanWin, "No");

		return true;
	}

	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStimulationVoterListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationVoterDesc)
};
}  // namespace Stimulation
}  // namespace Plugins
}  // namespace OpenViBE
