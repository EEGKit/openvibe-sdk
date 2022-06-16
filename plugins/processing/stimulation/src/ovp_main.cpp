#include "ovp_defines.h"

#include "box-algorithms/ovpCBoxAlgorithmClockStimulator.h"
#include "box-algorithms/ovpCBoxAlgorithmPlayerController.h"

#include "box-algorithms/CBoxStimulationMultiplexer.hpp"
#include "box-algorithms/ovpCBoxAlgorithmStimulationVoter.h"

#include "box-algorithms/ovpCBoxAlgorithmStreamEndDetector.h"
#include "box-algorithms/ovpCBoxAlgorithmTimeout.h"

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {

OVP_Declare_Begin()
	context.getTypeManager().registerEnumerationType(OV_TypeId_PlayerAction, "Player Action");
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Play", OV_TypeId_PlayerAction_Play.id());
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Stop", OV_TypeId_PlayerAction_Stop.id());
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Pause", OV_TypeId_PlayerAction_Pause.id());
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Forward", OV_TypeId_PlayerAction_Forward.id());

	OVP_Declare_New(CBoxAlgorithmClockStimulatorDesc)
	OVP_Declare_New(CBoxAlgorithmPlayerControllerDesc)
	OVP_Declare_New(CBoxStimulationMultiplexerDesc)

	OVP_Declare_New(CBoxAlgorithmStreamEndDetectorDesc)
	OVP_Declare_New(CBoxAlgorithmTimeoutDesc)

	OVP_Declare_New(CBoxAlgorithmStimulationVoterDesc)
	context.getTypeManager().registerEnumerationType(OVP_TypeId_Voting_ClearVotes, "Clear votes");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_ClearVotes, "When expires", OVP_TypeId_Voting_ClearVotes_WhenExpires.id());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_ClearVotes, "After output", OVP_TypeId_Voting_ClearVotes_AfterOutput.id());
	context.getTypeManager().registerEnumerationType(OVP_TypeId_Voting_OutputTime, "Output time");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_OutputTime, "Time of voting", OVP_TypeId_Voting_OutputTime_Vote.id());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_OutputTime, "Time of last winning stimulus", OVP_TypeId_Voting_OutputTime_Winner.id());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_OutputTime, "Time of last voting stimulus", OVP_TypeId_Voting_OutputTime_Last.id());
	context.getTypeManager().registerEnumerationType(OVP_TypeId_Voting_RejectClass_CanWin, "Reject can win");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_RejectClass_CanWin, "Yes", OVP_TypeId_Voting_RejectClass_CanWin_Yes.id());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_RejectClass_CanWin, "No", OVP_TypeId_Voting_RejectClass_CanWin_No.id());

OVP_Declare_End()

}  // namespace Stimulation
}  // namespace Plugins
}  // namespace OpenViBE
