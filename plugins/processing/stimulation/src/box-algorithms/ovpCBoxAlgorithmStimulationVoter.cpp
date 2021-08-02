#include "ovpCBoxAlgorithmStimulationVoter.h"

#include <system/ovCMath.h>

#include <string>
#include <algorithm>

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {

bool CBoxAlgorithmStimulationVoter::initialize()
{
	const Kernel::IBox& boxContext = this->getStaticBoxContext();

	OV_ERROR_UNLESS_KRF(boxContext.getInputCount() == 1, "Invalid number of inputs [" << boxContext.getInputCount() << "] (expected 1 single input)",
						Kernel::ErrorType::BadInput);

	CIdentifier typeID;
	boxContext.getInputType(0, typeID);

	OV_ERROR_UNLESS_KRF(typeID == OV_TypeId_Stimulations, "Invalid input type [" << typeID.str() << "] (expected OV_TypeId_Stimulations type)",
						Kernel::ErrorType::BadInput);

	m_encoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationEncoder));
	m_encoder->initialize();

	m_decoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationDecoder));
	m_decoder->initialize();

	ip_buffer.initialize(m_decoder->getInputParameter(OVP_GD_Algorithm_StimulationDecoder_InputParameterId_MemoryBufferToDecode));
	op_stimulationSet.initialize(m_decoder->getOutputParameter(OVP_GD_Algorithm_StimulationDecoder_OutputParameterId_StimulationSet));

	m_minimumVotes      = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_timeWindow        = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_clearVotes        = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));
	m_outputDateMode    = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));
	m_rejectClassLabel  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
	m_rejectClassCanWin = uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5));

	this->getLogManager() << Kernel::LogLevel_Debug << "Vote clear mode " << m_clearVotes << ", timestamp at " << m_outputDateMode << ", reject mode " <<
			m_rejectClassCanWin << "\n";

	m_latestStimulusDate = 0;
	m_lastTime           = 0;

	m_oStimulusDeque.clear();

	return true;
}

bool CBoxAlgorithmStimulationVoter::uninitialize()
{
	m_decoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_decoder);

	m_encoder->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*m_encoder);

	return true;
}

bool CBoxAlgorithmStimulationVoter::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStimulationVoter::process()
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();

	Kernel::TParameterHandler<IStimulationSet*> ip_stimSet(m_encoder->getInputParameter(OVP_GD_Algorithm_StimulationEncoder_InputParameterId_StimulationSet));
	Kernel::TParameterHandler<IMemoryBuffer*> op_buffer(m_encoder->getOutputParameter(OVP_GD_Algorithm_StimulationEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_buffer = boxContext.getOutputChunk(0);

	// Push the stimulations to a queue
	bool newStimulus = false;
	for (size_t j = 0; j < boxContext.getInputChunkCount(0); ++j)
	{
		ip_buffer = boxContext.getInputChunk(0, j);
		m_decoder->process();
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationDecoder_OutputTriggerId_ReceivedHeader)) { }
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationDecoder_OutputTriggerId_ReceivedBuffer))
		{
			for (size_t k = 0; k < op_stimulationSet->getStimulationCount(); ++k)
			{
				uint64_t stimulationId   = op_stimulationSet->getStimulationIdentifier(k);
				uint64_t stimulationDate = op_stimulationSet->getStimulationDate(k);
				m_latestStimulusDate     = std::max(m_latestStimulusDate, stimulationDate);
				if (CTime(m_latestStimulusDate - stimulationDate).toSeconds() <= m_timeWindow)
				{
					// Stimulus is fresh, append
					m_oStimulusDeque.push_back(std::pair<uint64_t, uint64_t>(stimulationId, stimulationDate));
					newStimulus = true;
				}
			}
		}
		if (m_decoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationDecoder_OutputTriggerId_ReceivedEnd)) { }
		boxContext.markInputAsDeprecated(0, j);
	}

	if (m_oStimulusDeque.empty() || !newStimulus) { return true; }

	// Always clear too old votes that have slipped off the time window. The time window is relative to the time of the latest stimulus received.
	while (!m_oStimulusDeque.empty())
	{
		const uint64_t frontDate = m_oStimulusDeque.front().second;
		if (CTime(m_latestStimulusDate - frontDate).toSeconds() > m_timeWindow)
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

	this->getLogManager() << Kernel::LogLevel_Debug << "Queue size is " << m_oStimulusDeque.size() << "\n";

	if (m_oStimulusDeque.size() < m_minimumVotes)
	{
		// Not enough stimuli to vote
		return true;
	}

	std::map<uint64_t, uint64_t> lastSeen;			// The last occurrence of each type in time
	std::map<uint64_t, size_t> votes;				// Histogram of votes

	// Make a histogram of the votes
	for (const auto& stim : m_oStimulusDeque)
	{
		const uint64_t type = stim.first;
		const uint64_t date = stim.second;

		votes[type]++;
		lastSeen[type] = std::max(lastSeen[type], date);
		// Never pop here, only pop on expiration
	}

	// Find the winner
	uint64_t resultClassLabel = m_rejectClassLabel;
	size_t maxVotes           = 0;

	for (const auto& vote : votes)
	{
		const uint64_t type = vote.first;
		const size_t nVotes = vote.second; // can not be zero by construction above

		if (m_rejectClassCanWin == OVP_TypeId_Voting_RejectClass_CanWin_No && type == m_rejectClassLabel)
		{
			// Reject class never wins
			continue;
		}

		if (nVotes > maxVotes)
		{
			resultClassLabel = type;
			maxVotes         = nVotes;
		}
		else if (nVotes == maxVotes)
		{
			// Break ties arbitrarily
			if (System::Math::random0To1() > 0.5)
			{
				resultClassLabel = type;
				maxVotes         = nVotes;
			}
		}
	}

	if (m_lastTime == 0)
	{
		m_encoder->process(OVP_GD_Algorithm_StimulationEncoder_InputTriggerId_EncodeHeader);
		boxContext.markOutputAsReadyToSend(0, m_lastTime, m_lastTime);
	}

	if (m_rejectClassCanWin == OVP_TypeId_Voting_RejectClass_CanWin_No && resultClassLabel == m_rejectClassLabel)
	{
		this->getLogManager() << Kernel::LogLevel_Debug << "Winning class " << resultClassLabel << " was 'rejected' with " << maxVotes << "votes. Dropped.\n";
	}
	else
	{
		const uint64_t currentTime = getPlayerContext().getCurrentTime();

		uint64_t timeStamp;
		if (m_outputDateMode == OVP_TypeId_Voting_OutputTime_Vote) { timeStamp = currentTime; }
		else if (m_outputDateMode == OVP_TypeId_Voting_OutputTime_Winner) { timeStamp = lastSeen[resultClassLabel]; }
		else { timeStamp = m_latestStimulusDate; }

		this->getLogManager() << Kernel::LogLevel_Debug << "Appending winning stimulus " << resultClassLabel << " at " << timeStamp << " (" << maxVotes << " votes)\n";

		ip_stimSet->setStimulationCount(0);
		ip_stimSet->appendStimulation(resultClassLabel, timeStamp, 0);
		m_encoder->process(OVP_GD_Algorithm_StimulationEncoder_InputTriggerId_EncodeBuffer);
		boxContext.markOutputAsReadyToSend(0, m_lastTime, currentTime);
		m_lastTime = currentTime;

		if (m_clearVotes == OVP_TypeId_Voting_ClearVotes_AfterOutput) { m_oStimulusDeque.clear(); }
	}

	return true;
}

}  // namespace Stimulation
}  // namespace Plugins
}  // namespace OpenViBE
