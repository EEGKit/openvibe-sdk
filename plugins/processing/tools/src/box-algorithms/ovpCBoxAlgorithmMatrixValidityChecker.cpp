#include "ovpCBoxAlgorithmMatrixValidityChecker.h"

#include <cmath>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Tools;

bool CBoxAlgorithmMatrixValidityChecker::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();

	uint64_t logLevel     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_logLevel           = ELogLevel(logLevel);
	m_validityCheckerType = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	if (boxContext.getSettingCount() == 1
	)
	{
		m_validityCheckerType = OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger();
	} // note that for boxes with one setting, we fallback to the old behavior 

	OV_ERROR_UNLESS_KRF(boxContext.getSettingCount() <= 1 || boxContext.getInputCount() == boxContext.getOutputCount(),
						"Invalid input count [" << boxContext.getInputCount() << "] (expected same value as output count [" << boxContext.getOutputCount() <<
						"])",
						OpenViBE::Kernel::ErrorType::BadConfig);

	m_decoders.resize(boxContext.getInputCount());
	m_encoders.resize(boxContext.getInputCount());
	for (size_t i = 0; i < boxContext.getInputCount(); ++i)
	{
		m_decoders[i].initialize(*this, i);
		m_encoders[i].initialize(*this, i);
		m_encoders[i].getInputMatrix().setReferenceTarget(m_decoders[i].getOutputMatrix());
	}

	m_lastValidSamples.clear();
	m_lastValidSamples.resize(boxContext.getInputCount());
	m_nTotalInterpolatedSample.clear();
	m_nTotalInterpolatedSample.resize(boxContext.getInputCount());
	m_nTotalInterpolatedChunk.clear();
	m_nTotalInterpolatedChunk.resize(boxContext.getInputCount());

	return true;
}

bool CBoxAlgorithmMatrixValidityChecker::uninitialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();
	for (size_t i = 0; i < nInput; ++i)
	{
		m_decoders[i].uninitialize();
		m_encoders[i].uninitialize();
	}
	m_decoders.clear();
	m_encoders.clear();

	return true;
}

bool CBoxAlgorithmMatrixValidityChecker::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmMatrixValidityChecker::process()
{
	IBoxIO& boxContext    = this->getDynamicBoxContext();
	const size_t nInput   = this->getStaticBoxContext().getInputCount();
	const size_t nSetting = this->getStaticBoxContext().getSettingCount();

	for (size_t i = 0; i < nInput; ++i)
	{
		for (size_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
		{
			m_decoders[i].decode(j);
			IMatrix* matrix = m_decoders[i].getOutputMatrix();

			if (m_decoders[i].isHeaderReceived())
			{
				if (nSetting > 1) { m_encoders[i].encodeHeader(); }

				if (m_validityCheckerType == OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger())
				{
					m_nTotalInterpolatedSample[i] = 0;
					m_nTotalInterpolatedChunk[i]  = 0;
					// for each channel, save of the last valid sample
					m_lastValidSamples[i].resize(matrix->getDimensionSize(0));
				}
			}
			if (m_decoders[i].isBufferReceived())
			{
				// log warning
				if (m_validityCheckerType == OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger())
				{
					if (!OpenViBEToolkit::Tools::Matrix::isContentValid(*matrix))
					{
						getLogManager() << m_logLevel << "Matrix on input " << i << " either contains NAN or Infinity between " <<
								time64(boxContext.getInputChunkStartTime(i, j)) << " and " << time64(boxContext.getInputChunkEndTime(i, j)) << ".\n";
					}
				}
					// stop player
				else if (m_validityCheckerType == OVP_TypeId_ValidityCheckerType_StopPlayer.toUInteger())
				{
					if (!OpenViBEToolkit::Tools::Matrix::isContentValid(*matrix))
					{
						this->getPlayerContext().stop();
						OV_ERROR_KRF(
							"Invalid matrix content on input [" << i << "]: either contains NAN or Infinity between [" << time64(boxContext.
								getInputChunkStartTime(i, j)) << "] and [" << time64(boxContext.getInputChunkEndTime(i, j)) << "]",
							OpenViBE::Kernel::ErrorType::BadInput);
					}
				}
					// interpolate
				else if (m_validityCheckerType == OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger())
				{
					const size_t nChannel      = matrix->getDimensionSize(0);
					const size_t nSample       = matrix->getDimensionSize(1);
					double* buffer             = matrix->getBuffer();
					size_t nInterpolatedSample = 0;

					for (size_t k = 0; k < nChannel; ++k)
					{
						for (size_t l = 0; l < nSample; ++l)
						{
							if (std::isnan(buffer[l + k * nSample]) || std::isinf(buffer[l + k * nSample]))
							{
								// interpolation : order 0 (easiest for online interpolation)
								buffer[l + k * nSample] = m_lastValidSamples[i][k];
								nInterpolatedSample++;
							}
							else
							{
								// save of the last valid sample of channel k
								m_lastValidSamples[i][k] = buffer[l + k * nSample];
							}
						}
					}
					m_nTotalInterpolatedSample[i] += nInterpolatedSample;

					// log management
					if (nInterpolatedSample > 0 && m_nTotalInterpolatedSample[i] == nInterpolatedSample) // beginning of interpolation
					{
						getLogManager() << m_logLevel << "Matrix on input " << i << " either contains NAN or Infinity from " << time64(
							boxContext.getInputChunkStartTime(i, j)) << ": interpolation is enable.\n";
					}
					if (nInterpolatedSample > 0) // update of ChunkCount during interpolation
					{
						m_nTotalInterpolatedChunk[i]++;
					}
					if (nInterpolatedSample == 0 && m_nTotalInterpolatedSample[i] > 0) // end of interpolation
					{
						getLogManager() << m_logLevel << "Matrix on input " << i << " contained " << 100.0 * m_nTotalInterpolatedSample[i] / (
							m_nTotalInterpolatedChunk[i] * nSample * nChannel) << " % of NAN or Infinity. Interpolation disable from " << time64(
							boxContext.getInputChunkStartTime(i, j)) << ".\n";
						m_nTotalInterpolatedSample[i] = 0; // reset
						m_nTotalInterpolatedChunk[i]  = 0;
					}
				}
				else { OV_WARNING_K("Invalid action type [" << m_validityCheckerType << "]"); }

				if (nSetting > 1) { m_encoders[i].encodeBuffer(); }
			}
			if (m_decoders[i].isEndReceived()) { if (nSetting > 1) { m_encoders[i].encodeEnd(); } }
			if (nSetting > 1) { boxContext.markOutputAsReadyToSend(i, boxContext.getInputChunkStartTime(i, j), boxContext.getInputChunkEndTime(i, j)); }
		}
	}

	return true;
}
