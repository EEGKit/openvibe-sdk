#include "ovpCBoxAlgorithmMatrixValidityChecker.h"

#include <cmath>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Tools;

bool CBoxAlgorithmMatrixValidityChecker::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();

	uint64_t logLevel         = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_eLogLevel               = ELogLevel(logLevel);
	m_ui64ValidityCheckerType = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	if (boxContext.getSettingCount() == 1) { m_ui64ValidityCheckerType = OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger(); } // note that for boxes with one setting, we fallback to the old behavior 

	OV_ERROR_UNLESS_KRF(boxContext.getSettingCount() <= 1 || boxContext.getInputCount() == boxContext.getOutputCount(),
						"Invalid input count [" << boxContext.getInputCount() << "] (expected same value as output count [" << boxContext.getOutputCount() << "])",
						OpenViBE::Kernel::ErrorType::BadConfig);

	m_vStreamDecoder.resize(boxContext.getInputCount());
	m_vStreamEncoder.resize(boxContext.getInputCount());
	for (uint32_t i = 0; i < boxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i].initialize(*this, i);
		m_vStreamEncoder[i].initialize(*this, i);
		m_vStreamEncoder[i].getInputMatrix().setReferenceTarget(m_vStreamDecoder[i].getOutputMatrix());
	}

	m_vLastValidSample.clear();
	m_vLastValidSample.resize(boxContext.getInputCount());
	m_ui32TotalInterpolatedSampleCount.clear();
	m_ui32TotalInterpolatedSampleCount.resize(boxContext.getInputCount());
	m_ui32TotalInterpolatedChunkCount.clear();
	m_ui32TotalInterpolatedChunkCount.resize(boxContext.getInputCount());

	return true;
}

bool CBoxAlgorithmMatrixValidityChecker::uninitialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();
	for (uint32_t i = 0; i < nInput; i++)
	{
		m_vStreamDecoder[i].uninitialize();
		m_vStreamEncoder[i].uninitialize();
	}
	m_vStreamDecoder.clear();
	m_vStreamEncoder.clear();

	return true;
}

bool CBoxAlgorithmMatrixValidityChecker::processInput(const uint32_t ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmMatrixValidityChecker::process()
{
	IBoxIO& boxContext    = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();
	const size_t nSetting = this->getStaticBoxContext().getSettingCount();

	for (uint32_t i = 0; i < nInput; i++)
	{
		for (uint32_t j = 0; j < boxContext.getInputChunkCount(i); j++)
		{
			m_vStreamDecoder[i].decode(j);
			IMatrix* l_pMatrix = m_vStreamDecoder[i].getOutputMatrix();

			if (m_vStreamDecoder[i].isHeaderReceived())
			{
				if (nSetting > 1) { m_vStreamEncoder[i].encodeHeader(); }

				if (m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger())
				{
					m_ui32TotalInterpolatedSampleCount[i] = 0;
					m_ui32TotalInterpolatedChunkCount[i]  = 0;
					// for each channel, save of the last valid sample
					m_vLastValidSample[i].resize(l_pMatrix->getDimensionSize(0));
				}
			}
			if (m_vStreamDecoder[i].isBufferReceived())
			{
				// log warning
				if (m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger())
				{
					if (!OpenViBEToolkit::Tools::Matrix::isContentValid(*l_pMatrix))
					{
						getLogManager() << m_eLogLevel << "Matrix on input " << i << " either contains NAN or Infinity between " << time64(boxContext.getInputChunkStartTime(i, j)) << " and " << time64(boxContext.getInputChunkEndTime(i, j)) << ".\n";
					}
				}
					// stop player
				else if (m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_StopPlayer.toUInteger())
				{
					if (!OpenViBEToolkit::Tools::Matrix::isContentValid(*l_pMatrix))
					{
						this->getPlayerContext().stop();
						OV_ERROR_KRF("Invalid matrix content on input [" << i << "]: either contains NAN or Infinity between [" << time64(boxContext.getInputChunkStartTime(i, j)) << "] and [" << time64(boxContext.getInputChunkEndTime(i, j)) << "]",
									 OpenViBE::Kernel::ErrorType::BadInput);
					}
				}
					// interpolate
				else if (m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger())
				{
					const uint32_t nChannel      = l_pMatrix->getDimensionSize(0);
					const uint32_t nSample       = l_pMatrix->getDimensionSize(1);
					double* buffer               = l_pMatrix->getBuffer();
					uint32_t nInterpolatedSample = 0;

					for (uint32_t k = 0; k < nChannel; k++)
					{
						for (uint32_t l = 0; l < nSample; l++)
						{
							if (std::isnan(buffer[l + k * nSample]) || std::isinf(buffer[l + k * nSample]))
							{
								// interpolation : order 0 (easiest for online interpolation)
								buffer[l + k * nSample] = m_vLastValidSample[i][k];
								nInterpolatedSample++;
							}
							else
							{
								// save of the last valid sample of channel k
								m_vLastValidSample[i][k] = buffer[l + k * nSample];
							}
						}
					}
					m_ui32TotalInterpolatedSampleCount[i] += nInterpolatedSample;

					// log management
					if (nInterpolatedSample > 0 && m_ui32TotalInterpolatedSampleCount[i] == nInterpolatedSample) // beginning of interpolation
					{
						getLogManager() << m_eLogLevel << "Matrix on input " << i << " either contains NAN or Infinity from " << time64(boxContext.getInputChunkStartTime(i, j)) << ": interpolation is enable.\n";
					}
					if (nInterpolatedSample > 0) // update of ChunkCount during interpolation
					{
						m_ui32TotalInterpolatedChunkCount[i]++;
					}
					if (nInterpolatedSample == 0 && m_ui32TotalInterpolatedSampleCount[i] > 0) // end of interpolation
					{
						getLogManager() << m_eLogLevel << "Matrix on input " << i << " contained " << 100.0 * m_ui32TotalInterpolatedSampleCount[i] / (m_ui32TotalInterpolatedChunkCount[i] * nSample * nChannel) << " % of NAN or Infinity. Interpolation disable from " << time64(boxContext.getInputChunkStartTime(i, j)) << ".\n";
						m_ui32TotalInterpolatedSampleCount[i] = 0; // reset
						m_ui32TotalInterpolatedChunkCount[i]  = 0;
					}
				}
				else
				{
					OV_WARNING_K("Invalid action type [" << m_ui64ValidityCheckerType << "]");
				}

				if (nSetting > 1) { m_vStreamEncoder[i].encodeBuffer(); }
			}
			if (m_vStreamDecoder[i].isEndReceived())
			{
				if (nSetting > 1) { m_vStreamEncoder[i].encodeEnd(); }
			}
			if (nSetting > 1) { boxContext.markOutputAsReadyToSend(i, boxContext.getInputChunkStartTime(i, j), boxContext.getInputChunkEndTime(i, j)); }
		}
	}

	return true;
}
