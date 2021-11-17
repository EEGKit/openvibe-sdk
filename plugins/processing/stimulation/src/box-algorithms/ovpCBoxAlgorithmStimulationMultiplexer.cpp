#include "ovpCBoxAlgorithmStimulationMultiplexer.h"

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {


bool CBoxAlgorithmStimulationMultiplexer::initialize()
{
	const Kernel::IBox& staticBoxContext = this->getStaticBoxContext();

	m_decoders.resize(staticBoxContext.getInputCount());
	size_t index = 0;
	for (auto& stimulationDecoder : m_decoders)
	{
		stimulationDecoder.initialize(*this, index);
		index += 1;
	}

	m_encoder.initialize(*this, 0);

	m_decoderEndTimes = std::vector<uint64_t>(staticBoxContext.getInputCount(), 0ULL);

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
	const Kernel::IBox& staticBoxContext = this->getStaticBoxContext();
	Kernel::IBoxIO& dynamicBoxContext    = this->getDynamicBoxContext();

	if (!m_wasHeaderSent)
	{
		m_encoder.encodeHeader();
		dynamicBoxContext.markOutputAsReadyToSend(0, m_lastEndTime, m_lastEndTime);
		m_wasHeaderSent = true;
	}

	uint64_t earliestReceivedChunkEndTime = 0xffffffffffffffffULL;

	for (size_t input = 0; input < staticBoxContext.getInputCount(); ++input)
	{
		for (size_t chunk = 0; chunk < dynamicBoxContext.getInputChunkCount(input); ++chunk)
		{
			m_decoders[input].decode(chunk);

			if (m_decoders[input].isBufferReceived())
			{
				for (size_t stimulation = 0; stimulation < m_decoders[input].getOutputStimulationSet()->size(); ++stimulation)
				{
					m_stimulations.insert(std::make_pair(m_decoders[input].getOutputStimulationSet()->getDate(stimulation),
														 std::make_tuple(
															 m_decoders[input].getOutputStimulationSet()->getId(stimulation),
															 m_decoders[input].getOutputStimulationSet()->getDate(stimulation),
															 m_decoders[input].getOutputStimulationSet()->getDuration(stimulation))));
				}
			}

			m_decoderEndTimes[input] = dynamicBoxContext.getInputChunkEndTime(input, chunk);
		}

		if (earliestReceivedChunkEndTime > m_decoderEndTimes[input]) { earliestReceivedChunkEndTime = m_decoderEndTimes[input]; }
	}

	if (earliestReceivedChunkEndTime >= m_lastEndTime)
	{
		m_encoder.getInputStimulationSet()->clear();

		for (auto stimulation = m_stimulations.begin(); stimulation != m_stimulations.end();)
		{
			if (stimulation->first < earliestReceivedChunkEndTime)
			{
				m_encoder.getInputStimulationSet()->push_back(std::get<0>(stimulation->second), std::get<1>(stimulation->second),
																	  std::get<2>(stimulation->second));
				stimulation = m_stimulations.erase(stimulation);
			}
			else { ++stimulation; }
		}

		m_encoder.encodeBuffer();

		dynamicBoxContext.markOutputAsReadyToSend(0, m_lastEndTime, earliestReceivedChunkEndTime);

		m_lastStartTime = m_lastEndTime;
		m_lastEndTime   = earliestReceivedChunkEndTime;
	}

	return true;
}

}  // namespace Stimulation
}  // namespace Plugins
}  // namespace OpenViBE
