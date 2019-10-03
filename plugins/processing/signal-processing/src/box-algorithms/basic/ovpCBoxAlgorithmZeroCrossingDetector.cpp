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
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

bool CBoxAlgorithmZeroCrossingDetector::initialize()
{
	m_oEncoder1.initialize(*this, 1);
	m_oEncoder2.initialize(*this, 2);

	m_f64HysteresisThreshold = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_f64WindowTime          = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(m_f64WindowTime > 0, "Invalid negative number for window length", OpenViBE::Kernel::ErrorType::BadSetting);

	m_stimulationId1 = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_stimulationId2 = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

	CIdentifier typeID;
	this->getStaticBoxContext().getInputType(0, typeID);

	if (typeID == OV_TypeId_Signal)
	{
		OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmZeroCrossingDetector>* decoder = new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmZeroCrossingDetector>* encoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		encoder->getInputSamplingRate().setReferenceTarget(decoder->getOutputSamplingRate());
		m_oDecoder  = decoder;
		m_oEncoder0 = encoder;
	}
	else if (typeID == OV_TypeId_StreamedMatrix)
	{
		OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmZeroCrossingDetector>* decoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmZeroCrossingDetector>* encoder = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmZeroCrossingDetector>(*this, 0);
		m_oDecoder  = decoder;
		m_oEncoder0 = encoder;
	}
	else { OV_ERROR_KRF("Invalid input type [" << typeID.toString() << "]", OpenViBE::Kernel::ErrorType::BadInput); }

	return true;
}

bool CBoxAlgorithmZeroCrossingDetector::uninitialize()
{
	m_oEncoder0.uninitialize();
	m_oEncoder1.uninitialize();
	m_oEncoder2.uninitialize();
	m_oDecoder.uninitialize();
	return true;
}

bool CBoxAlgorithmZeroCrossingDetector::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmZeroCrossingDetector::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();
	uint32_t j, k;

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		m_oDecoder.decode(i);
		m_oEncoder1.getInputStimulationSet()->clear();

		const uint32_t nChannel = m_oDecoder.getOutputMatrix()->getDimensionSize(0);
		const uint32_t nSample  = m_oDecoder.getOutputMatrix()->getDimensionSize(1);

		if (m_oDecoder.isHeaderReceived())
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(*m_oEncoder0.getInputMatrix(), *m_oDecoder.getOutputMatrix());
			IMatrix* o2Matrix = m_oEncoder2.getInputMatrix();
			o2Matrix->setDimensionCount(2);
			o2Matrix->setDimensionSize(0, nChannel);
			o2Matrix->setDimensionSize(1, nSample);

			m_vSignalHistory.clear();
			m_vSignalHistory.resize(nChannel, 0);
			m_vStateHistory.clear();
			m_vStateHistory.resize(nChannel);

			m_vMemoryChunk.clear();
			m_vMemoryChunk.resize(nChannel);
			m_vMemorySample.clear();
			m_vMemorySample.resize(nChannel);

			m_nChunk = 0;

			m_oEncoder0.encodeHeader();
			m_oEncoder1.encodeHeader();
			m_oEncoder2.encodeHeader();
		}
		if (m_oDecoder.isBufferReceived())
		{
			if (m_nChunk == 0)
			{
				m_samplingRate = nSample * uint32_t((1LL << 32) / (boxContext.getInputChunkEndTime(0, i) - boxContext.getInputChunkStartTime(0, i)));
				m_ui32WindowTime = uint32_t(m_f64WindowTime * m_samplingRate);
			}

			double* iBuffer   = m_oDecoder.getOutputMatrix()->getBuffer();
			double* oBuffer0 = m_oEncoder0.getInputMatrix()->getBuffer();
			double* oBuffer2 = m_oEncoder2.getInputMatrix()->getBuffer();

			// ZC detector, with hysteresis
			std::vector<double> signals(nSample + 1, 0);

			for (j = 0; j < nChannel; j++)
			{
				// signal, with the last sample of the previous chunk
				signals[0] = m_vSignalHistory[j];
				for (k = 0; k < nSample; k++) { signals[k + 1] = iBuffer[k + j * nSample]; }
				m_vSignalHistory[j] = signals.back();

				if (m_nChunk == 0) { m_vStateHistory[j] = (signals[1] >= 0) ? 1 : -1; }

				for (k = 0; k < nSample; k++)
				{
					uint64_t stimulationDate;
					if (m_samplingRate > 0)
					{
						stimulationDate = boxContext.getInputChunkStartTime(0, i) + TimeArithmetics::sampleCountToTime(m_samplingRate, k);
					}
					else if (nSample == 1) { stimulationDate = boxContext.getInputChunkEndTime(0, i); }
					else
					{
						OV_ERROR_KRF("Can only process chunks with sampling rate larger or equal to 1 or chunks with exactly one sample.",
									 ErrorType::OutOfBound);
					}

					if ((m_vStateHistory[j] == 1) && (signals[k] > -m_f64HysteresisThreshold) && (signals[k + 1] < -m_f64HysteresisThreshold))
					{
						// negative ZC : positive-to-negative
						oBuffer0[k + j * nSample] = -1;
						m_oEncoder1.getInputStimulationSet()->appendStimulation(m_stimulationId2, stimulationDate, 0);
						m_vStateHistory[j] = -1;
					}
					else if ((m_vStateHistory[j] == -1) && (signals[k] < m_f64HysteresisThreshold) && (signals[k + 1] > m_f64HysteresisThreshold))
					{
						// positive ZC : negative-to-positive
						oBuffer0[k + j * nSample] = 1;
						m_oEncoder1.getInputStimulationSet()->appendStimulation(m_stimulationId1, stimulationDate, 0);
						m_vStateHistory[j] = 1;

						// for the rythm estimation
						m_vMemoryChunk[j].push_back(m_nChunk);
						m_vMemorySample[j].push_back(k);
					}
					else { oBuffer0[k + j * nSample] = 0; }
				}
			}

			// rythm estimation, in events per min
			for (j = 0; j < nChannel; j++)
			{
				int compt = 0;

				// supression of peaks older than m_ui32WindowTime by decreasing indices, to avoid overflow
				for (size_t index = m_vMemoryChunk[j].size(); index >= 1; index--)
				{
					const size_t kk = index - 1;
					if (((m_nChunk + 1) * nSample - (m_vMemorySample[j][kk] + m_vMemoryChunk[j][kk] * nSample)) < m_ui32WindowTime)
					{
						compt += 1;
					}
					else
					{
						m_vMemorySample[j].erase(m_vMemorySample[j].begin() + kk);
						m_vMemoryChunk[j].erase(m_vMemoryChunk[j].begin() + kk);
					}
				}

				for (k = 0; k < nSample; k++) { oBuffer2[k + j * nSample] = 60.0 * compt / m_f64WindowTime; }
			}

			m_nChunk++;

			m_oEncoder0.encodeBuffer();
			m_oEncoder1.encodeBuffer();
			m_oEncoder2.encodeBuffer();
		}
		if (m_oDecoder.isEndReceived())
		{
			m_oEncoder0.encodeEnd();
			m_oEncoder1.encodeEnd();
			m_oEncoder2.encodeEnd();
		}
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(1, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(2, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
