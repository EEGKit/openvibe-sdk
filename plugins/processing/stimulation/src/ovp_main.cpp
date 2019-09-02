#include "ovp_defines.h"

#include "box-algorithms/ovpCBoxAlgorithmClockStimulator.h"
#include "box-algorithms/ovpCBoxAlgorithmPlayerController.h"

#include "box-algorithms/ovpCBoxAlgorithmStimulationMultiplexer.h"
#include "box-algorithms/ovpCBoxAlgorithmStimulationVoter.h"

#include "box-algorithms/ovpCBoxAlgorithmStreamEndDetector.h"
#include "box-algorithms/ovpCBoxAlgorithmTimeout.h"

OVP_Declare_Begin()
	context.getTypeManager().registerEnumerationType(OV_TypeId_PlayerAction, "Player Action");
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Play", OV_TypeId_PlayerAction_Play.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Stop", OV_TypeId_PlayerAction_Stop.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Pause", OV_TypeId_PlayerAction_Pause.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OV_TypeId_PlayerAction, "Forward", OV_TypeId_PlayerAction_Forward.toUInteger());

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmClockStimulatorDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmPlayerControllerDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmStimulationMultiplexerDesc);

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmStreamEndDetectorDesc);
	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmTimeoutDesc);

	OVP_Declare_New(OpenViBEPlugins::Stimulation::CBoxAlgorithmStimulationVoterDesc);
	context.getTypeManager().registerEnumerationType(OVP_TypeId_Voting_ClearVotes, "Clear votes");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_ClearVotes, "When expires", OVP_TypeId_Voting_ClearVotes_WhenExpires.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_ClearVotes, "After output", OVP_TypeId_Voting_ClearVotes_AfterOutput.toUInteger());
	context.getTypeManager().registerEnumerationType(OVP_TypeId_Voting_OutputTime, "Output time");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_OutputTime, "Time of voting", OVP_TypeId_Voting_OutputTime_Vote.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_OutputTime, "Time of last winning stimulus",
													  OVP_TypeId_Voting_OutputTime_Winner.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_OutputTime, "Time of last voting stimulus",
													  OVP_TypeId_Voting_OutputTime_Last.toUInteger());
	context.getTypeManager().registerEnumerationType(OVP_TypeId_Voting_RejectClass_CanWin, "Reject can win");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_RejectClass_CanWin, "Yes", OVP_TypeId_Voting_RejectClass_CanWin_Yes.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_Voting_RejectClass_CanWin, "No", OVP_TypeId_Voting_RejectClass_CanWin_No.toUInteger());

OVP_Declare_End()
