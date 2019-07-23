#define _USE_MATH_DEFINES
#include <cmath>

#include "ovpCBoxAlgorithmContinuousWaveletAnalysis.h"

#include <sstream>
#include <cstdio>
#include <string.h>
#include <iomanip>

using namespace OpenViBE;
using namespace Kernel;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace OpenViBEToolkit;

namespace SigProSTD
{
	double wavelet_FourierFactor(char* pWaveletType, double dWaveletParameter)
	{
		double l_FourierFactor = -1;
		if (strcmp(pWaveletType, "morlet") == 0)
		{
			l_FourierFactor = 4.0 * M_PI / (dWaveletParameter + std::sqrt(2 + dWaveletParameter * dWaveletParameter));
		}
		else if (strcmp(pWaveletType, "paul") == 0)
		{
			l_FourierFactor = 4.0 * M_PI / (2 * dWaveletParameter + 1);
		}
		else if (strcmp(pWaveletType, "dog") == 0)
		{
			l_FourierFactor = 2.0 * M_PI / std::sqrt(dWaveletParameter + 0.5);
		}
		return l_FourierFactor;
	}

	double wavelet_scale2period(char* pWaveletType, double dWaveletParameter, double dScale)
	{
		double l_FourierFactor = wavelet_FourierFactor(pWaveletType, dWaveletParameter);

		return l_FourierFactor * dScale;
	}

	double wavelet_scale2freq(char* pWaveletType, double dWaveletParameter, double dScale)
	{
		double l_FourierFactor = wavelet_FourierFactor(pWaveletType, dWaveletParameter);

		return 1.0 / (l_FourierFactor * dScale);
	}

	double wavelet_freq2scale(char* pWaveletType, double dWaveletParameter, double dFrequency)
	{
		double l_FourierFactor = wavelet_FourierFactor(pWaveletType, dWaveletParameter);

		return 1.0 / (l_FourierFactor * dFrequency);
	}
}  // namespace SigProSTD

bool CBoxAlgorithmContinuousWaveletAnalysis::initialize()
{
	m_oDecoder.initialize(*this, 0);
	m_vEncoder[0].initialize(*this, 0);
	m_vEncoder[1].initialize(*this, 1);
	m_vEncoder[2].initialize(*this, 2);
	m_vEncoder[3].initialize(*this, 3);

	uint64_t l_ui64WaveletType = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_dWaveletParameter        = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_iScaleCount_J            = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_dHighestFrequency        = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	double l_dFrequencySpacing = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	if (l_ui64WaveletType == OVP_TypeId_ContinuousWaveletType_Morlet.toUInteger())
	{
		m_pWaveletType = "morlet";

		if (m_dWaveletParameter < 0)
		{
			this->getLogManager() << LogLevel_Error << "Morlet wavelet parameter should be positive.\n";
			return false;
		}
	}
	else if (l_ui64WaveletType == OVP_TypeId_ContinuousWaveletType_Paul.toUInteger())
	{
		m_pWaveletType = "paul";

		if (m_dWaveletParameter <= 0 || m_dWaveletParameter > 20)
		{
			this->getLogManager() << LogLevel_Error << "Paul wavelet parameter should be included in ]0,20].\n";
			return false;
		}
		if (std::ceil(m_dWaveletParameter) != m_dWaveletParameter)
		{
			this->getLogManager() << LogLevel_Error << "Paul wavelet parameter should be an integer.\n";
			return false;
		}
	}
	else if (l_ui64WaveletType == OVP_TypeId_ContinuousWaveletType_DOG.toUInteger())
	{
		m_pWaveletType = "dog";

		if (m_dWaveletParameter <= 0 || static_cast<uint32_t>(m_dWaveletParameter) % 2 == 1)
		{
			this->getLogManager() << LogLevel_Error << "Derivative of Gaussian wavelet parameter should be strictly positive and even.\n";
			return false;
		}
		if (std::ceil(m_dWaveletParameter) != m_dWaveletParameter)
		{
			this->getLogManager() << LogLevel_Error << "Derivative of Gaussian wavelet parameter should be an integer.\n";
			return false;
		}
	}
	else
	{
		this->getLogManager() << LogLevel_Error << "Unknown wavelet type [" << l_ui64WaveletType << "].\n";
		return false;
	}

	if (m_iScaleCount_J <= 0)
	{
		this->getLogManager() << LogLevel_Error << "Number of frequencies can not be negative.\n";
		return false;
	}
	if (m_dHighestFrequency <= 0)
	{
		this->getLogManager() << LogLevel_Error << "Highest frequency can not be negative.\n";
		return false;
	}
	if (l_dFrequencySpacing <= 0)
	{
		this->getLogManager() << LogLevel_Error << "Frequency spacing can not be negative.\n";
		return false;
	}

	m_dSmallestScale_s0 = SigProSTD::wavelet_freq2scale(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, m_dHighestFrequency);
	m_dScaleSpacing_dj  = SigProSTD::wavelet_freq2scale(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, l_dFrequencySpacing);

	m_pScaleType         = "pow";
	m_iScalePowerBase_a0 = 2; // base of power if ScaleType = "pow"

	return true;
}

bool CBoxAlgorithmContinuousWaveletAnalysis::uninitialize()
{
	m_oDecoder.uninitialize();
	m_vEncoder[0].uninitialize();
	m_vEncoder[1].uninitialize();
	m_vEncoder[2].uninitialize();
	m_vEncoder[3].uninitialize();

	cwt_free(m_oWaveletTransform);
	m_oWaveletTransform = nullptr;

	return true;
}

bool CBoxAlgorithmContinuousWaveletAnalysis::processInput(uint32_t ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

/*The following algorithm is taken from:
Wavelib library, https://github.com/rafat/wavelib

A Practical Guide to Wavelet Analysis, 1998
C Torrence, and GP Compo*/

bool CBoxAlgorithmContinuousWaveletAnalysis::process()
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < l_rDynamicBoxContext.getInputChunkCount(0); ++i)
	{
		m_oDecoder.decode(i);
		IMatrix* l_pInputMatrix     = m_oDecoder.getOutputMatrix();
		uint32_t l_ui32ChannelCount = l_pInputMatrix->getDimensionSize(0);
		int l_iSampleCount          = l_pInputMatrix->getDimensionSize(1);

		if (m_oDecoder.isHeaderReceived())
		{
			uint64_t l_uiSamplingRate = m_oDecoder.getOutputSamplingRate();
			this->getLogManager() << LogLevel_Trace << "Input signal is [" << l_ui32ChannelCount << " x " << l_iSampleCount << "] @ " << l_uiSamplingRate << "Hz.\n";
			if (l_uiSamplingRate == 0)
			{
				this->getLogManager() << LogLevel_Error << "Input sampling frequency is equal to 0. Plugin can not process.\n";
				return false;
			}
			m_dSamplingPeriod_dt = 1.0 / l_uiSamplingRate;

			if (m_dHighestFrequency > 0.5 * l_uiSamplingRate)
			{
				this->getLogManager() << LogLevel_Error << "Highest frequency (" << m_dHighestFrequency << " Hz) is above Nyquist criterion (sampling rate is " << l_uiSamplingRate << " Hz), can not proceed!\n";
				return false;
			}

			int l_iScaleCountLimit = static_cast<int>(std::log2(l_iSampleCount * m_dSamplingPeriod_dt / m_dSmallestScale_s0) / m_dScaleSpacing_dj); // Eq.(10)
			if (m_iScaleCount_J > l_iScaleCountLimit)
			{
				this->getLogManager() << LogLevel_Error << "Frequency count [" << m_iScaleCount_J << "] is superior to the limit [" << l_iScaleCountLimit << "].\n";
				return false;
			}

			// initialize CWT
			m_oWaveletTransform = cwt_init(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, l_iSampleCount, m_dSamplingPeriod_dt, m_iScaleCount_J);
			if (!m_oWaveletTransform)
			{
				this->getLogManager() << LogLevel_Error << "Error during CWT initialization.\n";
				return false;
			}

			// define scales of CWT
			int l_iReturn = setCWTScales(m_oWaveletTransform, m_dSmallestScale_s0, m_dScaleSpacing_dj, const_cast<char *>(m_pScaleType), m_iScalePowerBase_a0);
			if (l_iReturn != 0)
			{
				this->getLogManager() << LogLevel_Error << "Error during CWT scales definition.\n";
				return false;
			}
			//cwt_summary(m_oWaveletTransform); // FOR DEBUG

			for (uint32_t l_ui32EncoderIndex = 0; l_ui32EncoderIndex < 4; ++l_ui32EncoderIndex)
			{
				IMatrix* l_pOutputMatrix = m_vEncoder[l_ui32EncoderIndex].getInputMatrix();
				l_pOutputMatrix->setDimensionCount(3);
				l_pOutputMatrix->setDimensionSize(0, l_ui32ChannelCount);
				l_pOutputMatrix->setDimensionSize(1, static_cast<uint32_t>(m_iScaleCount_J));
				l_pOutputMatrix->setDimensionSize(2, static_cast<uint32_t>(l_iSampleCount));

				for (uint32_t l_ui32ChannelIndex = 0; l_ui32ChannelIndex < l_ui32ChannelCount; ++l_ui32ChannelIndex)
				{
					l_pOutputMatrix->setDimensionLabel(0, l_ui32ChannelIndex, l_pInputMatrix->getDimensionLabel(0, l_ui32ChannelIndex));
				}
				// equivalence between wavelet scale and Fourier frequency, last column of Table 1 (m_oWaveletTransform->period accessible only after applying cwt function)
				double l_dScaleValue;
				double l_dFrequencyValue = -1;
				for (int l_iScaleIndex = 0; l_iScaleIndex < m_iScaleCount_J; ++l_iScaleIndex)
				{
					l_dScaleValue     = m_oWaveletTransform->scale[l_iScaleIndex];
					l_dFrequencyValue = SigProSTD::wavelet_scale2freq(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, l_dScaleValue);

					std::string l_sFrequencyString = std::to_string(l_dFrequencyValue);
					l_pOutputMatrix->setDimensionLabel(1, l_iScaleIndex, l_sFrequencyString.c_str());
				}
				for (int l_iSampleIndex = 0; l_iSampleIndex < l_iSampleCount; ++l_iSampleIndex)
				{
					std::string l_sSampleString = std::to_string(l_iSampleIndex * m_dSamplingPeriod_dt);
					l_pOutputMatrix->setDimensionLabel(2, l_iSampleIndex, l_sSampleString.c_str());
				}
				m_vEncoder[l_ui32EncoderIndex].encodeHeader();
			}
		}
		if (m_oDecoder.isBufferReceived())
		{
			double* l_pInputBuffer           = l_pInputMatrix->getBuffer();
			double* l_pOutputAmplitudeBuffer = m_vEncoder[0].getInputMatrix()->getBuffer();
			double* l_pOutputPhaseBuffer     = m_vEncoder[1].getInputMatrix()->getBuffer();
			double* l_pOutputRealPartBuffer  = m_vEncoder[2].getInputMatrix()->getBuffer();
			double* l_pOutputImagPartBuffer  = m_vEncoder[3].getInputMatrix()->getBuffer();

			for (uint32_t l_ui32ChannelIndex = 0; l_ui32ChannelIndex < l_ui32ChannelCount; l_ui32ChannelIndex++)
			{
				// compute CWT
				int l_iReturn = cwt(m_oWaveletTransform, l_pInputBuffer);
				if (l_iReturn != 0)
				{
					this->getLogManager() << LogLevel_Error << "Error during CWT computation.\n";
					return false;
				}

				// format of m_oWaveletTransform->output: dimensions = m_iScaleCount_J * l_iSampleCount, stored in row major format
				for (int l_iScaleIndex = 0; l_iScaleIndex < m_iScaleCount_J; l_iScaleIndex++)
				{
					for (int l_iSampleIndex = 0; l_iSampleIndex < l_iSampleCount; l_iSampleIndex++)
					{
						double l_dRealPart = m_oWaveletTransform->output[l_iSampleIndex + l_iScaleIndex * l_iSampleCount].re;
						double l_dImagPart = m_oWaveletTransform->output[l_iSampleIndex + l_iScaleIndex * l_iSampleCount].im;

						int l_iOutputIndex = l_iSampleIndex + (m_iScaleCount_J - l_iScaleIndex - 1) * l_iSampleCount + l_ui32ChannelIndex * l_iSampleCount * m_iScaleCount_J; // t+f*T+c*T*F

						l_pOutputAmplitudeBuffer[l_iOutputIndex] = std::sqrt(l_dRealPart * l_dRealPart + l_dImagPart * l_dImagPart);
						l_pOutputPhaseBuffer[l_iOutputIndex]     = std::atan2(l_dImagPart, l_dRealPart);
						l_pOutputRealPartBuffer[l_iOutputIndex]  = l_dRealPart;
						l_pOutputImagPartBuffer[l_iOutputIndex]  = l_dImagPart;
					}
				}
				l_pInputBuffer += l_iSampleCount;
			}

			m_vEncoder[0].encodeBuffer();
			m_vEncoder[1].encodeBuffer();
			m_vEncoder[2].encodeBuffer();
			m_vEncoder[3].encodeBuffer();
		}
		if (m_oDecoder.isEndReceived())
		{
			m_vEncoder[0].encodeEnd();
			m_vEncoder[1].encodeEnd();
			m_vEncoder[2].encodeEnd();
			m_vEncoder[3].encodeEnd();
		}
		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markOutputAsReadyToSend(1, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markOutputAsReadyToSend(2, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
		l_rDynamicBoxContext.markOutputAsReadyToSend(3, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
