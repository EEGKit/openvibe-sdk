#include "ovpCBoxAlgorithmStimulationVoter.h"

#include <openvibe/ovITimeArithmetics.h>
#include <system/ovCMath.h>

#include <vector>
#include <string>
#include <algorithm>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Stimulation;

bool CBoxAlgorithmStimulationVoter::initialize()
{
	const IBox& l_rStaticBoxContext = this->getStaticBoxContext();

	OV_ERROR_UNLESS_KRF(l_rStaticBoxContext.getInputCount() == 1, "Invalid number of inputs [" << l_rStaticBoxContext.getInputCount() << "] (expected 1 single input)", OpenViBE::Kernel::ErrorType::BadInput);

	CIdentifier l_oTypeIdentifier;
	l_rStaticBoxContext.getInputType(0, l_oTypeIdentifier);

	OV_ERROR_UNLESS_KRF(l_oTypeIdentifier == OV_TypeId_Stimulations, "Invalid input type [" << l_oTypeIdentifier.toString() << "] (expected OV_TypeId_Stimulations type)", OpenViBE::Kernel::ErrorType::BadInput);

	m_pEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamEncoder));
	m_pEncoder->initialize();

	m_pDecoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StimulationStreamDecoder));
	m_pDecoder->initialize();

	ip_pMemoryBuffer.initialize(m_pDecoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_InputParameterId_MemoryBufferToDecode));
	op_pStimulationSet.initialize(m_pDecoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	m_ui64MinimumVotes     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_f64TimeWindow        = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_oClearVotes          = ((uint64_t)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));
	m_oOutputDateMode      = ((uint64_t)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));
	m_ui64RejectClassLabel = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
	m_oRejectClass_CanWin  = ((uint64_t)FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5));

	this->getLogManager() << LogLevel_Debug << "Vote clear mode " << m_oClearVotes << ", timestamp at " << m_oOutputDateMode << ", reject mode " << m_oRejectClass_CanWin << "\n";

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

bool CBoxAlgorithmStimulationVoter::processInput(const uint32_t ui32Index)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStimulationVoter::process()
{
	//	this->getLogManager() << LogLevel_Info << "Process called\n";

	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	TParameterHandler<IStimulationSet*> ip_pStimulationSet(m_pEncoder->getInputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	TParameterHandler<IMemoryBuffer*> op_pMemoryBuffer(m_pEncoder->getOutputParameter(OVP_GD_Algorithm_StimulationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
	op_pMemoryBuffer = l_rDynamicBoxContext.getOutputChunk(0);

	// Push the stimulations to a queue
	bool l_bNewStimulus = false;
	for (uint32_t j = 0; j < l_rDynamicBoxContext.getInputChunkCount(0); j++)
	{
		ip_pMemoryBuffer = l_rDynamicBoxContext.getInputChunk(0, j);
		m_pDecoder->process();
		if (m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedHeader)) { }
		if (m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedBuffer))
		{
			for (uint32_t k = 0; k < op_pStimulationSet->getStimulationCount(); k++)
			{
				uint64_t l_ui64StimulationIdentifier = op_pStimulationSet->getStimulationIdentifier(k);
				uint64_t l_ui64StimulationDate       = op_pStimulationSet->getStimulationDate(k);
				m_ui64LatestStimulusDate             = std::max(m_ui64LatestStimulusDate, l_ui64StimulationDate);
				if (ITimeArithmetics::timeToSeconds(m_ui64LatestStimulusDate - l_ui64StimulationDate) <= m_f64TimeWindow)
				{
					// Stimulus is fresh, append
					m_oStimulusDeque.push_back(std::pair<uint64_t, uint64_t>(l_ui64StimulationIdentifier, l_ui64StimulationDate));
					l_bNewStimulus = true;
				}
			}
		}
		if (m_pDecoder->isOutputTriggerActive(OVP_GD_Algorithm_StimulationStreamDecoder_OutputTriggerId_ReceivedEnd)) { }
		l_rDynamicBoxContext.markInputAsDeprecated(0, j);
	}

	if (m_oStimulusDeque.empty() || !l_bNewStimulus) { return true; }

	// Always clear too old votes that have slipped off the time window. The time window is relative to the time of the latest stimulus received.
	while (!m_oStimulusDeque.empty())
	{
		uint64_t l_ui64FrontDate = m_oStimulusDeque.front().second;
		if (ITimeArithmetics::timeToSeconds(m_ui64LatestStimulusDate - l_ui64FrontDate) > m_f64TimeWindow)
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

	this->getLogManager() << LogLevel_Debug << "Queue size is " << (uint64_t)m_oStimulusDeque.size() << "\n";

	if (m_oStimulusDeque.size() < m_ui64MinimumVotes)
	{
		// Not enough stimuli to vote
		return true;
	}

	std::map<uint64_t, uint64_t> l_oLastSeen;			// The last occurrence of each type in time
	std::map<uint64_t, uint32_t> l_mVotes;				// Histogram of votes

	// Make a histogram of the votes
	for (std::deque<std::pair<uint64_t, uint64_t>>::const_iterator it = m_oStimulusDeque.begin();
		 it != m_oStimulusDeque.end();
		 ++it)
	{
		uint64_t l_ui64StimulusType = (*it).first;
		uint64_t l_ui64StimulusDate = (*it).second;

		l_mVotes[l_ui64StimulusType]++;
		l_oLastSeen[l_ui64StimulusType] = std::max(l_oLastSeen[l_ui64StimulusType], l_ui64StimulusDate);
		// Never pop here, only pop on expiration
	}

	// Find the winner
	uint64_t l_ui64ResultClassLabel = m_ui64RejectClassLabel;
	uint64_t l_ui64MaxVotes         = 0;

	for (std::map<uint64_t, uint32_t>::const_iterator it = l_mVotes.begin();
		 it != l_mVotes.end();
		 ++it)
	{
		uint64_t l_ui64StimulusType  = (*it).first;
		uint64_t l_ui64StimulusVotes = (*it).second; // can not be zero by construction above

		if (m_oRejectClass_CanWin == OVP_TypeId_Voting_RejectClass_CanWin_No && l_ui64StimulusType == m_ui64RejectClassLabel)
		{
			// Reject class never wins
			continue;
		}

		if (l_ui64StimulusVotes > l_ui64MaxVotes)
		{
			l_ui64ResultClassLabel = l_ui64StimulusType;
			l_ui64MaxVotes         = l_ui64StimulusVotes;
		}
		else if (l_ui64StimulusVotes == l_ui64MaxVotes)
		{
			// Break ties arbitrarily
			if (System::Math::randomFloat32BetweenZeroAndOne() > 0.5)
			{
				l_ui64ResultClassLabel = l_ui64StimulusType;
				l_ui64MaxVotes         = l_ui64StimulusVotes;
			}
		}
	}

	if (m_ui64LastTime == 0)
	{
		m_pEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeHeader);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastTime, m_ui64LastTime);
	}

	if (m_oRejectClass_CanWin == OVP_TypeId_Voting_RejectClass_CanWin_No && l_ui64ResultClassLabel == m_ui64RejectClassLabel)
	{
		this->getLogManager() << LogLevel_Debug << "Winning class " << l_ui64ResultClassLabel << " was 'rejected' with " << l_ui64MaxVotes << "votes. Dropped.\n";
	}
	else
	{
		uint64_t l_ui64CurrentTime = getPlayerContext().getCurrentTime();

		uint64_t l_ui64TimeStamp;
		if (m_oOutputDateMode == OVP_TypeId_Voting_OutputTime_Vote)
		{
			l_ui64TimeStamp = l_ui64CurrentTime;
		}
		else if (m_oOutputDateMode == OVP_TypeId_Voting_OutputTime_Winner)
		{
			l_ui64TimeStamp = l_oLastSeen[l_ui64ResultClassLabel];
		}
		else
		{
			l_ui64TimeStamp = m_ui64LatestStimulusDate;
		}

		this->getLogManager() << LogLevel_Debug << "Appending winning stimulus " << l_ui64ResultClassLabel << " at " << l_ui64TimeStamp << " (" << l_ui64MaxVotes << " votes)\n";

		ip_pStimulationSet->setStimulationCount(0);
		ip_pStimulationSet->appendStimulation(l_ui64ResultClassLabel, l_ui64TimeStamp, 0);
		m_pEncoder->process(OVP_GD_Algorithm_StimulationStreamEncoder_InputTriggerId_EncodeBuffer);
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, m_ui64LastTime, l_ui64CurrentTime);
		m_ui64LastTime = l_ui64CurrentTime;

		if (m_oClearVotes == OVP_TypeId_Voting_ClearVotes_AfterOutput)
		{
			m_oStimulusDeque.clear();
		}
	}

	return true;
}
