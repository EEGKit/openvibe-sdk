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

#include "ovpCBoxAlgorithmSignalResampling.h"

#include <cstdio>
#include <iostream>
#include <vector>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

namespace SigProSTD {
	template<typename T,
	         typename std::enable_if<std::is_integral<T>::value>::type* = nullptr,
	         typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
	T gcd(T a, T b)
	{
		T t;

		if (a > b) // ensure b > a
		{
			t = b;
			b = a;
			a = t;
		}

		while (b != 0)
		{
			t = a % b;
			a = b;
			b = t;
		}

		return a;
	}
}

OpenViBE::boolean CBoxAlgorithmSignalResampling::initialize(void)
{
	m_oDecoder.initialize(*this, 0);
	m_oEncoder.initialize(*this, 0);

	int64 l_i64OutputSamplingRate = FSettingValueAutoCast(
	            *this->getBoxAlgorithmContext(),
	            OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_NewSamplingFrequency
	            );
	int64 l_i64OutputSampleCount = FSettingValueAutoCast(
	            *this->getBoxAlgorithmContext(), 
	            OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_SampleCountPerBuffer
	            );

	OV_ERROR_UNLESS_KRF(
		l_i64OutputSamplingRate > 0,
		"Invalid output sampling rate [" << l_i64OutputSamplingRate << "] (expected value > 0)",
		OpenViBE::Kernel::ErrorType::BadSetting
	);

	OV_ERROR_UNLESS_KRF(
		l_i64OutputSampleCount > 0,
		"Invalid sample count per buffer [" << l_i64OutputSampleCount << "] (expected value > 0)",
		OpenViBE::Kernel::ErrorType::BadSetting
	);

	m_ui32OutputSamplingRate = static_cast<uint32>(l_i64OutputSamplingRate);
	m_ui32OutputSampleCount = static_cast<uint32>(l_i64OutputSampleCount);

	m_iFractionalDelayFilterSampleCount = 6;
	m_f64TransitionBandInPercent = 45;
	m_f64StopBandAttenuation = 49;

	m_ui32InputSamplingRate = 0;

	m_oEncoder.getInputSamplingRate() = static_cast<uint64>(m_ui32OutputSamplingRate);

	return true;
}

OpenViBE::boolean CBoxAlgorithmSignalResampling::uninitialize(void)
{
	m_oDecoder.uninitialize();
	m_oEncoder.uninitialize();
	return true;
}

OpenViBE::boolean CBoxAlgorithmSignalResampling::processInput(uint32 ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

OpenViBE::boolean CBoxAlgorithmSignalResampling::process(void)
{
	m_pDynamicBoxContext = &this->getDynamicBoxContext();
	uint32 i;

	for(i=0; i<m_pDynamicBoxContext->getInputChunkCount(0); i++)
	{
		m_oDecoder.decode(i);

		IMatrix* l_pInputMatrix=m_oDecoder.getOutputMatrix();
		IMatrix* l_pOutputMatrix=m_oEncoder.getInputMatrix();

		uint32 l_ui32ChannelCount = l_pInputMatrix->getDimensionSize(0);
		uint32 l_ui32SampleCount = l_pInputMatrix->getDimensionSize(1);

		if(m_oDecoder.isHeaderReceived())
		{
			m_ui32InputSamplingRate = static_cast<uint32>(m_oDecoder.getOutputSamplingRate());

			OV_ERROR_UNLESS_KRF(
				m_ui32InputSamplingRate > 0,
				"Invalid input sampling rate [" << m_ui32InputSamplingRate << "] (expected value > 0)",
				OpenViBE::Kernel::ErrorType::BadInput
			);

			this->getLogManager() << LogLevel_Info << "Resampling from [" << m_ui32InputSamplingRate <<  "] Hz to [" << m_ui32OutputSamplingRate <<  "] Hz.\n";

			float64 l_f64SRC = 1.0 * m_ui32OutputSamplingRate / m_ui32InputSamplingRate;
			uint32 l_ui32GreatestCommonDivisor = static_cast<uint32>(SigProSTD::gcd(m_ui32InputSamplingRate, m_ui32OutputSamplingRate));
			uint32 l_ui32FactorUpsampling = m_ui32OutputSamplingRate/l_ui32GreatestCommonDivisor;
			uint32 l_ui32FactorDownsampling = m_ui32InputSamplingRate/l_ui32GreatestCommonDivisor;
			if(l_f64SRC<=0.5 || l_f64SRC>1.0)
			{
				this->getLogManager() << LogLevel_Info << "Sampling rate conversion [" << l_f64SRC << "] : upsampling by a factor of [" << l_ui32FactorUpsampling <<  "], low-pass filtering, and downsampling by a factor of [" << l_ui32FactorDownsampling << "].\n";
			}
			else
			{
				OV_WARNING_K("Sampling rate conversion [" << l_f64SRC << "] : upsampling by a factor of [" << l_ui32FactorUpsampling <<  "], low-pass filtering, and downsampling by a factor of [" << l_ui32FactorDownsampling << "]");
			}

			m_oResampler.setFractionalDelayFilterSampleCount(m_iFractionalDelayFilterSampleCount);
			m_oResampler.setTransitionBand(m_f64TransitionBandInPercent);
			m_oResampler.setStopBandAttenuation(m_f64StopBandAttenuation);
			m_oResampler.reset(l_ui32ChannelCount, m_ui32InputSamplingRate, m_ui32OutputSamplingRate);

			float l_f32BuiltInLatency = m_oResampler.getBuiltInLatency();
			if(l_f32BuiltInLatency <= 0.15)
			{
				this->getLogManager() << LogLevel_Trace << "Latency induced by the resampling is [" << l_f32BuiltInLatency << "] s.\n";
			}
			else if(0.15 < l_f32BuiltInLatency && l_f32BuiltInLatency <= 0.5)
			{
				this->getLogManager() << LogLevel_Info << "Latency induced by the resampling is [" << l_f32BuiltInLatency << "] s.\n";
			}
			else if(0.5 < l_f32BuiltInLatency)
			{
				OV_WARNING_K("Latency induced by the resampling is [" << l_f32BuiltInLatency << "] s.");
			}

			OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pOutputMatrix, *l_pInputMatrix);
			l_pOutputMatrix->setDimensionSize(1, m_ui32OutputSampleCount);

			m_ui64TotalOutputSampleCount = 0;

			m_oEncoder.encodeHeader();
			m_pDynamicBoxContext->markOutputAsReadyToSend(0, 0, 0);
		}
		if(m_oDecoder.isBufferReceived())
		{
			// re-sampling sample-wise via a callback
			size_t l_ui32Count = m_oResampler.resample(*this, l_pInputMatrix->getBuffer(), l_ui32SampleCount);
			(void)l_ui32Count;
			//this->getLogManager() << LogLevel_Info << "l_ui32Count = " << l_ui32Count << ".\n";

			// encoding made in the callback (see next function)
		}
		if(m_oDecoder.isEndReceived())
		{
			m_oEncoder.encodeEnd();
			m_pDynamicBoxContext->markOutputAsReadyToSend(0,
				(uint64((m_ui64TotalOutputSampleCount%m_ui32OutputSampleCount) << 32) / m_ui32OutputSamplingRate),
				(uint64((m_ui64TotalOutputSampleCount%m_ui32OutputSampleCount) << 32) / m_ui32OutputSamplingRate));
		}
	}

	return true;
}

void CBoxAlgorithmSignalResampling::processResampler(const float64* pSample, size_t ui32ChannelCount) const
{
	float64* l_pBuffer = m_oEncoder.getInputMatrix()->getBuffer();
	uint64 l_ui64OutputSampleIndex = m_ui64TotalOutputSampleCount%m_ui32OutputSampleCount;

	for(uint32 j=0; j<ui32ChannelCount; j++)
	{
		l_pBuffer[j*m_ui32OutputSampleCount + l_ui64OutputSampleIndex] = pSample[j];
	}
	m_ui64TotalOutputSampleCount++;

	if ((m_ui64TotalOutputSampleCount%m_ui32OutputSampleCount) == 0)
	{
		m_oEncoder.encodeBuffer();
		m_pDynamicBoxContext->markOutputAsReadyToSend(0,
			(uint64((m_ui64TotalOutputSampleCount - m_ui32OutputSampleCount) << 32) / m_ui32OutputSamplingRate),
			(uint64((m_ui64TotalOutputSampleCount) << 32) / m_ui32OutputSamplingRate));
	}
}
