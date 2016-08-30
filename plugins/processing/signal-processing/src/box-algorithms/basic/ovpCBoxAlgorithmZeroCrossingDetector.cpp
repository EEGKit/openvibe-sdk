/*
 * MENSIA TECHNOLOGIES CONFIDENTIAL
 * ________________________________
 *
 *  [2012] - [2013] Mensia Technologies SA
 *  Copyright, All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Mensia Technologies SA.
 * The intellectual and technical concepts contained
 * herein are proprietary to Mensia Technologies SA
 * and are covered copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Mensia Technologies SA.
 */

#include "ovpCBoxAlgorithmZeroCrossingDetector.h"

#include <system/ovCMemory.h>
#include <openvibe/ovITimeArithmetics.h>

#include <vector>
#include <algorithm>
#include <cstdio>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

boolean CBoxAlgorithmZeroCrossingDetector::initialize(void)
{
	m_oEncoder1.initialize(*this, 1);
	m_oEncoder2.initialize(*this, 2);

	m_f64HysteresisThreshold = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_f64WindowTime = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(
		m_f64WindowTime > 0,
		"Invalid negative number for window length",
		OpenViBE::Kernel::ErrorType::BadSetting
	);

	m_ui64StimulationId1 = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_ui64StimulationId2 = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

	CIdentifier l_oTypeIdentifier;
	this->getStaticBoxContext().getInputType(0, l_oTypeIdentifier);

	if(l_oTypeIdentifier==OV_TypeId_Signal)
	{
		OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmZeroCrossingDetector >* l_pDecoder=new OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmZeroCrossingDetector >(*this, 0);
		OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmZeroCrossingDetector >* l_pEncoder=new OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmZeroCrossingDetector >(*this, 0);
		l_pEncoder->getInputSamplingRate().setReferenceTarget(l_pDecoder->getOutputSamplingRate());
		m_oDecoder=l_pDecoder;
		m_oEncoder0=l_pEncoder;
	}
	else if(l_oTypeIdentifier==OV_TypeId_StreamedMatrix)
	{
		OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmZeroCrossingDetector >* l_pDecoder=new OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmZeroCrossingDetector >(*this, 0);
		OpenViBEToolkit::TStreamedMatrixEncoder < CBoxAlgorithmZeroCrossingDetector >* l_pEncoder=new OpenViBEToolkit::TStreamedMatrixEncoder < CBoxAlgorithmZeroCrossingDetector >(*this, 0);
		m_oDecoder=l_pDecoder;
		m_oEncoder0=l_pEncoder;
	}
	else
	{
		OV_ERROR_KRF("Invalid input type [" << l_oTypeIdentifier.toString() << "]", OpenViBE::Kernel::ErrorType::BadInput);
	}

	return true;
}

boolean CBoxAlgorithmZeroCrossingDetector::uninitialize(void)
{
	m_oEncoder0.uninitialize();
	m_oEncoder1.uninitialize();
	m_oEncoder2.uninitialize();
	m_oDecoder.uninitialize();
	return true;
}

boolean CBoxAlgorithmZeroCrossingDetector::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmZeroCrossingDetector::process(void)
{
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();
	uint32 i, j, k;

	for(i=0; i<l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_oDecoder.decode(i);
		m_oEncoder1.getInputStimulationSet()->clear();

		uint32 l_ui32ChannelCount=m_oDecoder.getOutputMatrix()->getDimensionSize(0);
		uint32 l_ui32SampleCount=m_oDecoder.getOutputMatrix()->getDimensionSize(1);

		if(m_oDecoder.isHeaderReceived())
		{
			OpenViBEToolkit::Tools::Matrix::copyDescription(*m_oEncoder0.getInputMatrix(), *m_oDecoder.getOutputMatrix());
			IMatrix* l_pOutputMatrix2=m_oEncoder2.getInputMatrix();
			l_pOutputMatrix2->setDimensionCount(2);
			l_pOutputMatrix2->setDimensionSize(0, l_ui32ChannelCount);
			l_pOutputMatrix2->setDimensionSize(1, l_ui32SampleCount);

			m_vSignalHistory.clear();
			m_vSignalHistory.resize(l_ui32ChannelCount,0);
			m_vStateHistory.clear();
			m_vStateHistory.resize(l_ui32ChannelCount);

			m_vMemoryChunk.clear();
			m_vMemoryChunk.resize(l_ui32ChannelCount);
			m_vMemorySample.clear();
			m_vMemorySample.resize(l_ui32ChannelCount);

			m_ui64ChunkCount = 0;

			m_oEncoder0.encodeHeader();
			m_oEncoder1.encodeHeader();
			m_oEncoder2.encodeHeader();
		}
		if (m_oDecoder.isBufferReceived())
		{
			if (m_ui64ChunkCount == 0)
			{
				m_ui32SamplingRate = l_ui32SampleCount * static_cast<uint32>( (1LL<<32) / (l_rDynamicBoxContext.getInputChunkEndTime(0, i)-l_rDynamicBoxContext.getInputChunkStartTime(0, i)) );
				m_ui32WindowTime = static_cast<uint32>(m_f64WindowTime*m_ui32SamplingRate);
			}

			float64* l_pInputBuffer = m_oDecoder.getOutputMatrix()->getBuffer();
			float64* l_pOutputBuffer0 = m_oEncoder0.getInputMatrix()->getBuffer();
			float64* l_pOutputBuffer2 = m_oEncoder2.getInputMatrix()->getBuffer();

			// ZC detector, with hysteresis
			std::vector < float64 > l_vSignal(l_ui32SampleCount + 1, 0);

			for (j=0; j<l_ui32ChannelCount; j++)
			{
				// signal, with the last sample of the previous chunk
				l_vSignal[0] = m_vSignalHistory[j];
				for (k=0; k<l_ui32SampleCount; k++)
				{
					l_vSignal[k + 1] = l_pInputBuffer[k + j*l_ui32SampleCount];
				}
				m_vSignalHistory[j] = l_vSignal.back();

				if (m_ui64ChunkCount == 0)
				{
					m_vStateHistory[j] = (l_vSignal[1] >= 0) ? 1 : -1;
				}

				for (k=0; k<l_ui32SampleCount; k++)
				{

					if ((m_vStateHistory[j] == 1) && (l_vSignal[k] > -m_f64HysteresisThreshold) && (l_vSignal[k + 1] < -m_f64HysteresisThreshold))
					{
						// negative ZC : positive-to-negative
						l_pOutputBuffer0[k + j*l_ui32SampleCount] = -1;
						m_oEncoder1.getInputStimulationSet()->appendStimulation(m_ui64StimulationId2,
							l_rDynamicBoxContext.getInputChunkStartTime(0, i) + ITimeArithmetics::sampleCountToTime(m_ui32SamplingRate, k),
							0);
						m_vStateHistory[j] = -1;
					}
					else if ((m_vStateHistory[j] == -1) && (l_vSignal[k] < m_f64HysteresisThreshold) && (l_vSignal[k + 1] > m_f64HysteresisThreshold))
					{
						// positive ZC : negative-to-positive
						l_pOutputBuffer0[k + j*l_ui32SampleCount] = 1;
						m_oEncoder1.getInputStimulationSet()->appendStimulation(m_ui64StimulationId1,
							l_rDynamicBoxContext.getInputChunkStartTime(0, i) + ITimeArithmetics::sampleCountToTime(m_ui32SamplingRate, k),
							0);
						m_vStateHistory[j] = 1;

						// for the rythm estimation
						m_vMemoryChunk[j].push_back(m_ui64ChunkCount);
						m_vMemorySample[j].push_back(k);
					}
					else
					{
						l_pOutputBuffer0[k + j*l_ui32SampleCount] = 0;
					}

				}
			}

			// rythm estimation, in events per min
			for (j=0; j<l_ui32ChannelCount; j++)
			{
				int compt = 0;

				// supression of peaks older than m_ui32WindowTime by decreasing indices, to avoid overflow
				for (int kk=m_vMemoryChunk[j].size()-1; kk>=0; kk--)
				{
					if (((m_ui64ChunkCount+1)*l_ui32SampleCount - (m_vMemorySample[j][kk] + m_vMemoryChunk[j][kk]*l_ui32SampleCount)) < m_ui32WindowTime)
					{
						compt += 1;
					}
					else
					{
						m_vMemorySample[j].erase(m_vMemorySample[j].begin() + kk);
						m_vMemoryChunk[j].erase(m_vMemoryChunk[j].begin() + kk);
					}
				}

				for (k=0; k<l_ui32SampleCount; k++)
				{
					l_pOutputBuffer2[k + j*l_ui32SampleCount] = 60.0*compt/m_f64WindowTime;
				}
			}

			m_ui64ChunkCount++;

			m_oEncoder0.encodeBuffer();
			m_oEncoder1.encodeBuffer();
			m_oEncoder2.encodeBuffer();
		}
		if(m_oDecoder.isEndReceived())
		{
			m_oEncoder0.encodeEnd();
			m_oEncoder1.encodeEnd();
			m_oEncoder2.encodeEnd();
		}
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markOutputAsReadyToSend(2, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
