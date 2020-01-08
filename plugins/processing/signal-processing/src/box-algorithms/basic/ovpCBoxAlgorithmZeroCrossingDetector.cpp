/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * OpenViBE SDK
 * Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
 * Copyright (C) Inria, 2015-2017,V1.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include "ovpCBoxAlgorithmZeroCrossingDetector.h"

#include <system/ovCMemory.h>
#include <openvibe/ovTimeArithmetics.h>

#include <vector>
#include <iostream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;
using namespace OpenViBEToolkit;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmZeroCrossingDetector::initialize()
{
	m_encoder1.initialize(*this, 1);
	m_encoder2.initialize(*this, 2);

	m_hysteresis  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_windowTimeD = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(m_windowTimeD > 0, "Invalid negative number for window length", OpenViBE::Kernel::ErrorType::BadSetting);

	m_stimId1 = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_stimId2 = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

	CIdentifier typeID;
	this->getStaticBoxContext().getInputType(0, typeID);

	if (typeID == OV_TypeId_Signal)
	{
		TSignalDecoder<CBoxAlgorithmZeroCrossingDetector>* decoder = new TSignalDecoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		TSignalEncoder<CBoxAlgorithmZeroCrossingDetector>* encoder = new TSignalEncoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		encoder->getInputSamplingRate().setReferenceTarget(decoder->getOutputSamplingRate());
		m_decoder  = decoder;
		m_encoder0 = encoder;
	}
	else if (typeID == OV_TypeId_StreamedMatrix)
	{
		TStreamedMatrixDecoder<CBoxAlgorithmZeroCrossingDetector>* decoder = new TStreamedMatrixDecoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		TStreamedMatrixEncoder<CBoxAlgorithmZeroCrossingDetector>* encoder = new TStreamedMatrixEncoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		m_decoder                                                          = decoder;
		m_encoder0                                                         = encoder;
	}
	else { OV_ERROR_KRF("Invalid input type [" << typeID.toString() << "]", OpenViBE::Kernel::ErrorType::BadInput); }

	return true;
}

bool CBoxAlgorithmZeroCrossingDetector::uninitialize()
{
	m_encoder0.uninitialize();
	m_encoder1.uninitialize();
	m_encoder2.uninitialize();
	m_decoder.uninitialize();
	return true;
}

bool CBoxAlgorithmZeroCrossingDetector::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmZeroCrossingDetector::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	size_t j, k;

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		m_decoder.decode(i);
		m_encoder1.getInputStimulationSet()->clear();

		const size_t nChannel = m_decoder.getOutputMatrix()->getDimensionSize(0);
		const size_t nSample  = m_decoder.getOutputMatrix()->getDimensionSize(1);

		if (m_decoder.isHeaderReceived())
		{
			Tools::Matrix::copyDescription(*m_encoder0.getInputMatrix(), *m_decoder.getOutputMatrix());
			IMatrix* o2Matrix = m_encoder2.getInputMatrix();
			o2Matrix->setDimensionCount(2);
			o2Matrix->setDimensionSize(0, nChannel);
			o2Matrix->setDimensionSize(1, nSample);

			m_signals.clear();
			m_signals.resize(nChannel, 0);
			m_states.clear();
			m_states.resize(nChannel);

			m_chunks.clear();
			m_chunks.resize(nChannel);
			m_samples.clear();
			m_samples.resize(nChannel);

			m_nChunk = 0;

			m_encoder0.encodeHeader();
			m_encoder1.encodeHeader();
			m_encoder2.encodeHeader();
		}
		if (m_decoder.isBufferReceived())
		{
			if (m_nChunk == 0)
			{
				m_sampling   = nSample * size_t((1LL << 32) / (boxContext.getInputChunkEndTime(0, i) - boxContext.getInputChunkStartTime(0, i)));
				m_windowTime = size_t(m_windowTimeD * m_sampling);
			}

			double* iBuffer  = m_decoder.getOutputMatrix()->getBuffer();
			double* oBuffer0 = m_encoder0.getInputMatrix()->getBuffer();
			double* oBuffer2 = m_encoder2.getInputMatrix()->getBuffer();

			// ZC detector, with hysteresis
			std::vector<double> signals(nSample + 1, 0);

			for (j = 0; j < nChannel; ++j)
			{
				// signal, with the last sample of the previous chunk
				signals[0] = m_signals[j];
				for (k = 0; k < nSample; ++k) { signals[k + 1] = iBuffer[k + j * nSample]; }
				m_signals[j] = signals.back();

				if (m_nChunk == 0) { m_states[j] = (signals[1] >= 0) ? 1 : -1; }

				for (k = 0; k < nSample; ++k)
				{
					uint64_t stimulationDate;
					if (m_sampling > 0) { stimulationDate = boxContext.getInputChunkStartTime(0, i) + TimeArithmetics::sampleCountToTime(m_sampling, k); }
					else if (nSample == 1) { stimulationDate = boxContext.getInputChunkEndTime(0, i); }
					else
					{
						OV_ERROR_KRF("Can only process chunks with sampling rate larger or equal to 1 or chunks with exactly one sample.",
									 ErrorType::OutOfBound);
					}

					if ((m_states[j] == 1) && (signals[k] > -m_hysteresis) && (signals[k + 1] < -m_hysteresis))
					{
						// negative ZC : positive-to-negative
						oBuffer0[k + j * nSample] = -1;
						m_encoder1.getInputStimulationSet()->appendStimulation(m_stimId2, stimulationDate, 0);
						m_states[j] = -1;
					}
					else if ((m_states[j] == -1) && (signals[k] < m_hysteresis) && (signals[k + 1] > m_hysteresis))
					{
						// positive ZC : negative-to-positive
						oBuffer0[k + j * nSample] = 1;
						m_encoder1.getInputStimulationSet()->appendStimulation(m_stimId1, stimulationDate, 0);
						m_states[j] = 1;

						// for the rythm estimation
						m_chunks[j].push_back(m_nChunk);
						m_samples[j].push_back(k);
					}
					else { oBuffer0[k + j * nSample] = 0; }
				}
			}

			// rythm estimation, in events per min
			for (j = 0; j < nChannel; ++j)
			{
				int compt = 0;

				// supression of peaks older than m_windowTime by decreasing indices, to avoid overflow
				for (size_t index = m_chunks[j].size(); index >= 1; index--)
				{
					const size_t kk = index - 1;
					if (((m_nChunk + 1) * nSample - (m_samples[j][kk] + m_chunks[j][kk] * nSample)) < m_windowTime) { compt += 1; }
					else
					{
						m_samples[j].erase(m_samples[j].begin() + kk);
						m_chunks[j].erase(m_chunks[j].begin() + kk);
					}
				}

				for (k = 0; k < nSample; ++k) { oBuffer2[k + j * nSample] = 60.0 * compt / m_windowTimeD; }
			}

			m_nChunk++;

			m_encoder0.encodeBuffer();
			m_encoder1.encodeBuffer();
			m_encoder2.encodeBuffer();
		}
		if (m_decoder.isEndReceived())
		{
			m_encoder0.encodeEnd();
			m_encoder1.encodeEnd();
			m_encoder2.encodeEnd();
		}
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(1, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(2, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
