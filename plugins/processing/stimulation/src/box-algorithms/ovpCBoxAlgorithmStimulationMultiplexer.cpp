#include "ovpCBoxAlgorithmStimulationMultiplexer.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace Stimulation;

using namespace std;

bool CBoxAlgorithmStimulationMultiplexer::initialize()
{
	const IBox& staticBoxContext = this->getStaticBoxContext();

	m_decoders.resize(staticBoxContext.getInputCount());
	size_t index = 0;
	for (auto& stimulationDecoder : m_decoders)
	{
		stimulationDecoder.initialize(*this, index);
		index += 1;
	}

	m_encoder.initialize(*this, 0);

	m_decoderEndTimes = std::vector<CTime>(staticBoxContext.getInputCount(), 0);

	m_lastStartTime = 0;
	m_lastEndTime   = 0;
	m_wasHeaderSent = false;

	return true;
}

bool CBoxAlgorithmStimulationMultiplexer::uninitialize()
{
	for (auto& stimulationDecoder : m_decoders) { stimulationDecoder.uninitialize(); }

	m_encoder.uninitialize();

	return true;
}

bool CBoxAlgorithmStimulationMultiplexer::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmStimulationMultiplexer::process()
{
	IBoxIO& boxCtx      = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	if (!m_wasHeaderSent)
	{
		m_encoder.encodeHeader();
		boxCtx.markOutputAsReadyToSend(0, m_lastEndTime, m_lastEndTime);
		m_wasHeaderSent = true;
	}

	CTime earliestReceivedChunkEndTime = CTime::max();

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxCtx.getInputChunkCount(i); ++j)
		{
			m_decoders[i].decode(j);

			if (m_decoders[i].isBufferReceived())
			{
				CStimulationSet& set = *m_decoders[i].getOutputStimulationSet();
				for (size_t s = 0; s < set.size(); ++s) { m_stimulations.insert(std::make_pair(set[s].m_Date, set[s])); }
			}

			m_decoderEndTimes[i] = boxCtx.getInputChunkEndTime(i, j);
		}

		if (earliestReceivedChunkEndTime > m_decoderEndTimes[i]) { earliestReceivedChunkEndTime = m_decoderEndTimes[i]; }
	}

	if (earliestReceivedChunkEndTime >= m_lastEndTime)
	{
		m_encoder.getInputStimulationSet()->clear();

		for (auto stimulation = m_stimulations.begin(); stimulation != m_stimulations.end();)
		{
			if (stimulation->first < earliestReceivedChunkEndTime)
			{
				m_encoder.getInputStimulationSet()->append(stimulation->second);
				stimulation = m_stimulations.erase(stimulation);
			}
			else { ++stimulation; }
		}

		m_encoder.encodeBuffer();

		boxCtx.markOutputAsReadyToSend(0, m_lastEndTime, earliestReceivedChunkEndTime);

		m_lastStartTime = m_lastEndTime;
		m_lastEndTime   = earliestReceivedChunkEndTime;
	}

	return true;
}
