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
	double wavelet_FourierFactor(char* waveletType, const double waveletParameter)
	{
		double factor = -1;
		if (strcmp(waveletType, "morlet") == 0) { factor = 4.0 * M_PI / (waveletParameter + std::sqrt(2 + waveletParameter * waveletParameter)); }
		else if (strcmp(waveletType, "paul") == 0) { factor = 4.0 * M_PI / (2 * waveletParameter + 1); }
		else if (strcmp(waveletType, "dog") == 0) { factor = 2.0 * M_PI / std::sqrt(waveletParameter + 0.5); }
		return factor;
	}

	double wavelet_scale2period(char* waveletType, const double waveletParameter, const double scale)
	{
		const double fourierFactor = wavelet_FourierFactor(waveletType, waveletParameter);

		return fourierFactor * scale;
	}

	double wavelet_scale2freq(char* waveletType, const double waveletParameter, const double scale)
	{
		const double fourierFactor = wavelet_FourierFactor(waveletType, waveletParameter);

		return 1.0 / (fourierFactor * scale);
	}

	double wavelet_freq2scale(char* waveletType, const double waveletParameter, const double frequency)
	{
		const double fourierFactor = wavelet_FourierFactor(waveletType, waveletParameter);

		return 1.0 / (fourierFactor * frequency);
	}
} // namespace SigProSTD

bool CBoxAlgorithmContinuousWaveletAnalysis::initialize()
{
	m_oDecoder.initialize(*this, 0);
	m_vEncoder[0].initialize(*this, 0);
	m_vEncoder[1].initialize(*this, 1);
	m_vEncoder[2].initialize(*this, 2);
	m_vEncoder[3].initialize(*this, 3);

	const uint64_t waveletType    = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_dWaveletParameter           = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_iScaleCount_J               = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_dHighestFrequency           = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	const double frequencySpacing = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	if (waveletType == OVP_TypeId_ContinuousWaveletType_Morlet.toUInteger())
	{
		m_pWaveletType = "morlet";

		if (m_dWaveletParameter < 0)
		{
			this->getLogManager() << LogLevel_Error << "Morlet wavelet parameter should be positive.\n";
			return false;
		}
	}
	else if (waveletType == OVP_TypeId_ContinuousWaveletType_Paul.toUInteger())
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
	else if (waveletType == OVP_TypeId_ContinuousWaveletType_DOG.toUInteger())
	{
		m_pWaveletType = "dog";

		if (m_dWaveletParameter <= 0 || uint32_t(m_dWaveletParameter) % 2 == 1)
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
		this->getLogManager() << LogLevel_Error << "Unknown wavelet type [" << waveletType << "].\n";
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
	if (frequencySpacing <= 0)
	{
		this->getLogManager() << LogLevel_Error << "Frequency spacing can not be negative.\n";
		return false;
	}

	m_dSmallestScale_s0 = SigProSTD::wavelet_freq2scale(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, m_dHighestFrequency);
	m_dScaleSpacing_dj  = SigProSTD::wavelet_freq2scale(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, frequencySpacing);

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

bool CBoxAlgorithmContinuousWaveletAnalysis::processInput(const uint32_t /*index*/)
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
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		m_oDecoder.decode(i);
		IMatrix* iMatrix = m_oDecoder.getOutputMatrix();
		size_t nChannel  = iMatrix->getDimensionSize(0);
		int nSample      = iMatrix->getDimensionSize(1);

		if (m_oDecoder.isHeaderReceived())
		{
			size_t samplingRate = m_oDecoder.getOutputSamplingRate();
			this->getLogManager() << LogLevel_Trace << "Input signal is [" << nChannel << " x " << nSample << "] @ " << samplingRate << "Hz.\n";
			if (samplingRate == 0)
			{
				this->getLogManager() << LogLevel_Error << "Input sampling frequency is equal to 0. Plugin can not process.\n";
				return false;
			}
			m_dSamplingPeriod_dt = 1.0 / samplingRate;

			if (m_dHighestFrequency > 0.5 * samplingRate)
			{
				this->getLogManager() << LogLevel_Error << "Highest frequency (" << m_dHighestFrequency << " Hz) is above Nyquist criterion (sampling rate is "
						<< samplingRate << " Hz), can not proceed!\n";
				return false;
			}

			const int nScaleLimit = int(std::log2(nSample * m_dSamplingPeriod_dt / m_dSmallestScale_s0) / m_dScaleSpacing_dj); // Eq.(10)
			if (m_iScaleCount_J > nScaleLimit)
			{
				this->getLogManager() << LogLevel_Error << "Frequency count [" << m_iScaleCount_J << "] is superior to the limit [" << nScaleLimit << "].\n";
				return false;
			}

			// initialize CWT
			m_oWaveletTransform = cwt_init(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, nSample, m_dSamplingPeriod_dt, m_iScaleCount_J);
			if (!m_oWaveletTransform)
			{
				this->getLogManager() << LogLevel_Error << "Error during CWT initialization.\n";
				return false;
			}

			// define scales of CWT
			if (setCWTScales(m_oWaveletTransform, m_dSmallestScale_s0, m_dScaleSpacing_dj, const_cast<char*>(m_pScaleType), m_iScalePowerBase_a0) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Error during CWT scales definition.\n";
				return false;
			}
			//cwt_summary(m_oWaveletTransform); // FOR DEBUG

			for (size_t j = 0; j < 4; ++j)
			{
				IMatrix* oMatrix = m_vEncoder[j].getInputMatrix();
				oMatrix->setDimensionCount(3);
				oMatrix->setDimensionSize(0, nChannel);
				oMatrix->setDimensionSize(1, uint32_t(m_iScaleCount_J));
				oMatrix->setDimensionSize(2, uint32_t(nSample));

				for (size_t c = 0; c < nChannel; ++c)
				{
					oMatrix->setDimensionLabel(0, c, iMatrix->getDimensionLabel(0, c));
				}
				for (size_t scaleIndex = 0; scaleIndex < m_iScaleCount_J; ++scaleIndex)
				{
					const double scaleValue     = m_oWaveletTransform->scale[scaleIndex];
					const double frequencyValue = SigProSTD::wavelet_scale2freq(const_cast<char *>(m_pWaveletType), m_dWaveletParameter, scaleValue);

					std::string frequencyString = std::to_string(frequencyValue);
					oMatrix->setDimensionLabel(1, scaleIndex, frequencyString.c_str());
				}
				for (size_t sampleIdx = 0; sampleIdx < nSample; ++sampleIdx)
				{
					std::string sampleString = std::to_string(sampleIdx * m_dSamplingPeriod_dt);
					oMatrix->setDimensionLabel(2, sampleIdx, sampleString.c_str());
				}
				m_vEncoder[j].encodeHeader();
			}
		}
		if (m_oDecoder.isBufferReceived())
		{
			double* ibuffer          = iMatrix->getBuffer();
			double* oAmplitudeBuffer = m_vEncoder[0].getInputMatrix()->getBuffer();
			double* oPhaseBuffer     = m_vEncoder[1].getInputMatrix()->getBuffer();
			double* oRealPartBuffer  = m_vEncoder[2].getInputMatrix()->getBuffer();
			double* oImagPartBuffer  = m_vEncoder[3].getInputMatrix()->getBuffer();

			for (size_t c = 0; c < nChannel; c++)
			{
				// compute CWT
				if (cwt(m_oWaveletTransform, ibuffer) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Error during CWT computation.\n";
					return false;
				}

				// format of m_oWaveletTransform->output: dimensions = m_iScaleCount_J * l_iSampleCount, stored in row major format
				for (size_t scaleIdx = 0; scaleIdx < m_iScaleCount_J; scaleIdx++)
				{
					for (size_t sampleIdx = 0; sampleIdx < nSample; sampleIdx++)
					{
						const double real = m_oWaveletTransform->output[sampleIdx + scaleIdx * nSample].re;
						const double imag = m_oWaveletTransform->output[sampleIdx + scaleIdx * nSample].im;
						const size_t outputIdx = sampleIdx + (m_iScaleCount_J - scaleIdx - 1) * nSample + c * nSample * m_iScaleCount_J; // t+f*T+c*T*F

						oAmplitudeBuffer[outputIdx] = std::sqrt(real * real + imag * imag);
						oPhaseBuffer[outputIdx]     = std::atan2(imag, real);
						oRealPartBuffer[outputIdx]  = real;
						oImagPartBuffer[outputIdx]  = imag;
					}
				}
				ibuffer += nSample;
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
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(1, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(2, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(3, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
