#include "ovpCBoxAlgorithmStimulationVoter.h"

#include <openvibe/ovITimeArithmetics.h>
#include <system/ovCMath.h>

#include <string>
#include <algorithm>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Stimulation;

bool CBoxAlgorithmStimulationVoter::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();

	OV_ERROR_UNLESS_KRF(boxContext.getInputCount() == 1, "Invalid number of inputs [" << boxContext.getInputCount() << "] (expected 1 single input)",
						OpenViBE::Kernel::ErrorType::BadInput);

	CIdentifier typeID;
	boxContext.getInputType(0, typeID);

	OV_ERROR_UNLESS_KRF(typeID == OV_TypeId_Stimulations, "Invalid input type [" << typeID.toString() << "] (expected OV_TypeId_Stimulations type)",
						OpenViBE::Kernel::ErrorType::BadInput);

	m_pEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pEncoder->initialize();

	m_pDecoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pStimulationSet.initialize(m_pDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	m_ui64MinimumVotes     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_f64TimeWindow        = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_oClearVotes          = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));
	m_oOutputDateMode      = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));
	m_ui64RejectClassLabel = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
	m_oRejectClass_CanWin  = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5));

	this->getLogManager() << LogLevel_Debug << "Vote clear mode " << m_oClearVotes << ", timestamp at " << m_oOutputDateMode << ", reject mode " <<
			m_oRejectClass_CanWin << "\n";

	m_ui64LatestStimulusDate = 0;
	m_ui64LastTime           = 0;

	m_oStimulusDeque.clear();

	return true;
}

bool CBoxAlgorithmStimulationVoter::uninitialize()
{
	m_pDecoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pDecoder);

	m_pEncoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_pEncoder);

	return true;
}

bool CBoxAlgorithmStimulationVoter::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStimulationVoter::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	TParameterHandler<IStimulationSet*> ip_pStimulationSet(
		m_pEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	TParameterHandler<IMemoryBuffer*> op_pMemoryBuffer(
		m_pEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pMemoryBuffer = boxContext.getOutputChunk(0);

	// Push the stimulations to a queue
	bool newStimulus = false;
	for (size_t j = 0; j < boxContext.getInputChunkCount(0); j++)
	{
		ip_pMemoryBuffer = boxContext.getInputChunk(0, j);
		m_pDecoder->process();
		if (m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader)) { }
		if (m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			for (size_t k = 0; k < op_pStimulationSet->getStimulationCount(); k++)
			{
				uint64_t stimulationId   = op_pStimulationSet->getStimulationIdentifier(k);
				uint64_t stimulationDate = op_pStimulationSet->getStimulationDate(k);
				m_ui64LatestStimulusDate = std::max(m_ui64LatestStimulusDate, stimulationDate);
				if (ITimeArithmetics::timeToSeconds(m_ui64LatestStimulusDate - stimulationDate) <= m_f64TimeWindow)
				{
					// Stimulus is fresh, append
					m_oStimulusDeque.push_back(std::pair<uint64_t, uint64_t>(stimulationId, stimulationDate));
					newStimulus = true;
				}
			}
		}
		if (m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd)) { }
		boxContext.markInputAsDeprecated(0, j);
	}

	if (m_oStimulusDeque.empty() || !newStimulus) { return true; }

	// Always clear too old votes that have slipped off the time window. The time window is relative to the time of the latest stimulus received.
	while (!m_oStimulusDeque.empty())
	{
		const uint64_t frontDate = m_oStimulusDeque.front().second;
		if (ITimeArithmetics::timeToSeconds(m_ui64LatestStimulusDate - frontDate) > m_f64TimeWindow)
		{
			// Drop it
			m_oStimulusDeque.pop_front();
		}
		else
		{
			// Assume stimuli are received in time order. Since the stimulus at the head wasn't too old, the rest aren't either
			break;
		}
	}

	this->getLogManager() << LogLevel_Debug << "Queue size is " << uint64_t(m_oStimulusDeque.size()) << "\n";

	if (m_oStimulusDeque.size() < m_ui64MinimumVotes)
	{
		// Not enough stimuli to vote
		return true;
	}

	std::map<uint64_t, uint64_t> lastSeen;			// The last occurrence of each type in time
	std::map<uint64_t, uint32_t> votes;				// Histogram of votes

	// Make a histogram of the votes
	for (auto it = m_oStimulusDeque.begin(); it != m_oStimulusDeque.end(); ++it)
	{
		uint64_t stimulusType = (*it).first;
		uint64_t stimulusDate = (*it).second;

		votes[stimulusType]++;
		lastSeen[stimulusType] = std::max(lastSeen[stimulusType], stimulusDate);
		// Never pop here, only pop on expiration
	}

	// Find the winner
	uint64_t resultClassLabel = m_ui64RejectClassLabel;
	uint64_t maxVotes         = 0;

	for (auto it = votes.begin(); it != votes.end(); ++it)
	{
		const uint64_t stimulusType  = (*it).first;
		const uint64_t stimulusVotes = (*it).second; // can not be zero by construction above

		if (m_oRejectClass_CanWin == OVP_TypeId_Voting_RejectClass_CanWin_No && stimulusType == m_ui64RejectClassLabel)
		{
			// Reject class never wins
			continue;
		}

		if (stimulusVotes > maxVotes)
		{
			resultClassLabel = stimulusType;
			maxVotes         = stimulusVotes;
		}
		else if (stimulusVotes == maxVotes)
		{
			// Break ties arbitrarily
			if (System::Math::randomFloat32BetweenZeroAndOne() > 0.5)
			{
				resultClassLabel = stimulusType;
				maxVotes         = stimulusVotes;
			}
		}
	}

	if (m_ui64LastTime == 0)
	{
		m_pEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
		boxContext.markOutputAsReadyToSend(0, m_ui64LastTime, m_ui64LastTime);
	}

	if (m_oRejectClass_CanWin == OVP_TypeId_Voting_RejectClass_CanWin_No && resultClassLabel == m_ui64RejectClassLabel)
	{
		this->getLogManager() << LogLevel_Debug << "Winning class " << resultClassLabel << " was 'rejected' with " << maxVotes << "votes. Dropped.\n";
	}
	else
	{
		const uint64_t currentTime = getPlayerContext().getCurrentTime();

		uint64_t timeStamp;
		if (m_oOutputDateMode == OVP_TypeId_Voting_OutputTime_Vote) { timeStamp = currentTime; }
		else if (m_oOutputDateMode == OVP_TypeId_Voting_OutputTime_Winner) { timeStamp = lastSeen[resultClassLabel]; }
		else { timeStamp = m_ui64LatestStimulusDate; }

		this->getLogManager() << LogLevel_Debug << "Appending winning stimulus " << resultClassLabel << " at " << timeStamp << " (" << maxVotes << " votes)\n";

		ip_pStimulationSet->setStimulationCount(0);
		ip_pStimulationSet->appendStimulation(resultClassLabel, timeStamp, 0);
		m_pEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		boxContext.markOutputAsReadyToSend(0, m_ui64LastTime, currentTime);
		m_ui64LastTime = currentTime;

		if (m_oClearVotes == OVP_TypeId_Voting_ClearVotes_AfterOutput) { m_oStimulusDeque.clear(); }
	}

	return true;
}
