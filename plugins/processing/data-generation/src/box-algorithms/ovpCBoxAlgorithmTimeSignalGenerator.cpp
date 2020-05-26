#include "ovpCBoxAlgorithmTimeSignalGenerator.h"


using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;
using namespace DataGeneration;
using namespace std;

bool CBoxAlgorithmTimeSignalGenerator::initialize()
{
	m_encoder.initialize(*this, 0);

	// Parses box settings to try connecting to server
	m_sampling              = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_nGeneratedEpochSample = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_headerSent            = false;

	m_nSentSample = 0;

	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::uninitialize()
{
	m_encoder.uninitialize();
	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::processClock(CMessage& /*msg*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmTimeSignalGenerator::process()
{
	Kernel::IBoxIO* boxCtx = getBoxAlgorithmContext()->getDynamicBoxContext();

	if (!m_headerSent)
	{
		m_encoder.getInputSamplingRate() = m_sampling;

		IMatrix* matrix = m_encoder.getInputMatrix();

		matrix->setDimensionCount(2);
		matrix->setDimensionSize(0, 1);
		matrix->setDimensionSize(1, m_nGeneratedEpochSample);
		matrix->setDimensionLabel(0, 0, "Time signal");

		m_encoder.encodeHeader();

		m_headerSent = true;

		boxCtx->markOutputAsReadyToSend(0, 0, 0);
	}
	else
	{
		// Create sample chunks up until the next step (current time + 1/128) but do not overshoot it
		// This way we will always create the correct number of samples for frequencies that are above 128Hz
		const CTime nextStepDate = CTime(uint64_t(this->getPlayerContext().getCurrentTime() + (1ULL << 25))).toSampleCount(m_sampling);
		while (m_nSentSample + m_nGeneratedEpochSample < nextStepDate)
		{
			double* buffer = m_encoder.getInputMatrix()->getBuffer();

			for (size_t i = 0; i < m_nGeneratedEpochSample; ++i) { buffer[i] = (i + m_nSentSample) / double(m_sampling); }

			m_encoder.encodeBuffer();

			const CTime tStart = CTime(m_sampling, m_nSentSample);
			m_nSentSample += m_nGeneratedEpochSample;
			const CTime tEnd = CTime(m_sampling, m_nSentSample);

			boxCtx->markOutputAsReadyToSend(0, tStart, tEnd);
		}
	}

	return true;
}
