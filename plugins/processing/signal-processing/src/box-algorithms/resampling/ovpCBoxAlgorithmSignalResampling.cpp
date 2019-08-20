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
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

namespace SigProSTD
{
	template <typename T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr, typename std::enable_if<std::is_unsigned<T>::value>::type* = nullptr>
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
} // namespace SigProSTD

bool CBoxAlgorithmSignalResampling::initialize()
{
	m_oDecoder.initialize(*this, 0);
	m_oEncoder.initialize(*this, 0);

	int64_t l_i64OutputSamplingRate = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_NewSamplingFrequency);
	int64_t l_i64OutputSampleCount  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_SampleCountPerBuffer);

	OV_ERROR_UNLESS_KRF(l_i64OutputSamplingRate > 0, "Invalid output sampling rate [" << l_i64OutputSamplingRate << "] (expected value > 0)", OpenViBE::Kernel::ErrorType::BadSetting);

	OV_ERROR_UNLESS_KRF(l_i64OutputSampleCount > 0, "Invalid sample count per buffer [" << l_i64OutputSampleCount << "] (expected value > 0)", OpenViBE::Kernel::ErrorType::BadSetting);

	m_outSamplingRate = uint32_t(l_i64OutputSamplingRate);
	m_outSampleCount  = uint32_t(l_i64OutputSampleCount);

	m_iFractionalDelayFilterSampleCount = 6;
	m_f64TransitionBandInPercent        = 45;
	m_f64StopBandAttenuation            = 49;

	m_inSamplingRate = 0;

	m_oEncoder.getInputSamplingRate() = uint64_t(m_outSamplingRate);

	return true;
}

bool CBoxAlgorithmSignalResampling::uninitialize()
{
	m_oDecoder.uninitialize();
	m_oEncoder.uninitialize();
	return true;
}

bool CBoxAlgorithmSignalResampling::processInput(const uint32_t ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSignalResampling::process()
{
	m_pDynamicBoxContext = &this->getDynamicBoxContext();

	for (uint32_t i = 0; i < m_pDynamicBoxContext->getInputChunkCount(0); i++)
	{
		m_oDecoder.decode(i);

		IMatrix* iMatrix = m_oDecoder.getOutputMatrix();
		IMatrix* oMatrix = m_oEncoder.getInputMatrix();

		uint32_t channelCount = iMatrix->getDimensionSize(0);
		//uint32_t sampleCount  = iMatrix->getDimensionSize(1);

		if (m_oDecoder.isHeaderReceived())
		{
			m_inSamplingRate = uint32_t(m_oDecoder.getOutputSamplingRate());

			OV_ERROR_UNLESS_KRF(m_inSamplingRate > 0, "Invalid input sampling rate [" << m_inSamplingRate << "] (expected value > 0)", OpenViBE::Kernel::ErrorType::BadInput);

			this->getLogManager() << LogLevel_Info << "Resampling from [" << m_inSamplingRate << "] Hz to [" << m_outSamplingRate << "] Hz.\n";

			double src                     = 1.0 * m_outSamplingRate / m_inSamplingRate;
			uint32_t greatestCommonDivisor = uint32_t(SigProSTD::gcd(m_inSamplingRate, m_outSamplingRate));
			uint32_t factorUpsampling      = m_outSamplingRate / greatestCommonDivisor;
			uint32_t factorDownsampling    = m_inSamplingRate / greatestCommonDivisor;
			if (src <= 0.5 || src > 1.0)
			{
				this->getLogManager() << LogLevel_Info << "Sampling rate conversion [" << src << "] : upsampling by a factor of [" << factorUpsampling << "], low-pass filtering, and downsampling by a factor of [" << factorDownsampling << "].\n";
			}
			else
			{
				OV_WARNING_K("Sampling rate conversion [" << src << "] : upsampling by a factor of [" << factorUpsampling << "], low-pass filtering, and downsampling by a factor of [" << factorDownsampling << "]");
			}

			m_oResampler.setFractionalDelayFilterSampleCount(m_iFractionalDelayFilterSampleCount);
			m_oResampler.setTransitionBand(m_f64TransitionBandInPercent);
			m_oResampler.setStopBandAttenuation(m_f64StopBandAttenuation);
			m_oResampler.reset(channelCount, m_inSamplingRate, m_outSamplingRate);

			float l_f32BuiltInLatency = m_oResampler.getBuiltInLatency();
			if (l_f32BuiltInLatency <= 0.15)
			{
				this->getLogManager() << LogLevel_Trace << "Latency induced by the resampling is [" << l_f32BuiltInLatency << "] s.\n";
			}
			else if (0.15 < l_f32BuiltInLatency && l_f32BuiltInLatency <= 0.5)
			{
				this->getLogManager() << LogLevel_Info << "Latency induced by the resampling is [" << l_f32BuiltInLatency << "] s.\n";
			}
			else if (0.5 < l_f32BuiltInLatency)
			{
				OV_WARNING_K("Latency induced by the resampling is [" << l_f32BuiltInLatency << "] s.");
			}

			OpenViBEToolkit::Tools::Matrix::copyDescription(*oMatrix, *iMatrix);
			oMatrix->setDimensionSize(1, m_outSampleCount);

			m_totalOutSampleCount = 0;

			m_oEncoder.encodeHeader();
			m_pDynamicBoxContext->markOutputAsReadyToSend(0, 0, 0);
		}
		if (m_oDecoder.isBufferReceived())
		{
			// re-sampling sample-wise via a callback
			//size_t count = m_oResampler.resample(*this, l_pInputMatrix->getBuffer(), l_ui32SampleCount);
			//this->getLogManager() << LogLevel_Info << "count = " << l_ui32Count << ".\n";

			// encoding made in the callback (see next function)
		}
		if (m_oDecoder.isEndReceived())
		{
			m_oEncoder.encodeEnd();
			m_pDynamicBoxContext->markOutputAsReadyToSend(0, (uint64_t((m_totalOutSampleCount % m_outSampleCount) << 32) / m_outSamplingRate), (uint64_t((m_totalOutSampleCount % m_outSampleCount) << 32) / m_outSamplingRate));
		}
	}

	return true;
}

void CBoxAlgorithmSignalResampling::processResampler(const double* pSample, size_t ui32ChannelCount) const
{
	double* l_pBuffer                = m_oEncoder.getInputMatrix()->getBuffer();
	uint64_t l_ui64OutputSampleIndex = m_totalOutSampleCount % m_outSampleCount;

	for (uint32_t j = 0; j < ui32ChannelCount; j++)
	{
		l_pBuffer[j * m_outSampleCount + l_ui64OutputSampleIndex] = pSample[j];
	}
	m_totalOutSampleCount++;

	if ((m_totalOutSampleCount % m_outSampleCount) == 0)
	{
		m_oEncoder.encodeBuffer();
		m_pDynamicBoxContext->markOutputAsReadyToSend(0, (uint64_t((m_totalOutSampleCount - m_outSampleCount) << 32) / m_outSamplingRate), (uint64_t((m_totalOutSampleCount) << 32) / m_outSamplingRate));
	}
}
