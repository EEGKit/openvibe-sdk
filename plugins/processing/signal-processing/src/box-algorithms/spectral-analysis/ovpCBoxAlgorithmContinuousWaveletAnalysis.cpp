#define _USE_MATH_DEFINES
#include <cmath>

#include "ovpCBoxAlgorithmContinuousWaveletAnalysis.h"

#include <string.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;

using namespace /*OpenViBE::*/Toolkit;

namespace SigProSTD {
double WaveletFourierFactor(const char* type, const double param)
{
	double factor = -1;
	if (strcmp(type, "morlet") == 0) { factor = 4.0 * M_PI / (param + std::sqrt(2 + param * param)); }
	else if (strcmp(type, "paul") == 0) { factor = 4.0 * M_PI / (2 * param + 1); }
	else if (strcmp(type, "dog") == 0) { factor = 2.0 * M_PI / std::sqrt(param + 0.5); }
	return factor;
}

double WaveletScale2Period(const char* type, const double param, const double scale) { return WaveletFourierFactor(type, param) * scale; }
double WaveletScale2Freq(const char* type, const double param, const double scale) { return 1.0 / (WaveletFourierFactor(type, param) * scale); }
double WaveletFreq2Scale(const char* type, const double param, const double frequency) { return 1.0 / (WaveletFourierFactor(type, param) * frequency); }
}  // namespace SigProSTD

bool CBoxAlgorithmContinuousWaveletAnalysis::initialize()
{
	m_decoder.initialize(*this, 0);
	for (size_t i = 0; i < 4; ++i) { m_encoders[i].initialize(*this, i); }

	const EContinuousWaveletType type = EContinuousWaveletType(uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0)));
	m_waveletParam                    = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_nScaleJ                         = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_highestFreq                     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	const double frequencySpacing     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	if (type == EContinuousWaveletType::Morlet)
	{
		m_waveletType = "morlet";

		if (m_waveletParam < 0)
		{
			getLogManager() << LogLevel_Error << "Morlet wavelet parameter should be positive.\n";
			return false;
		}
	}
	else if (type == EContinuousWaveletType::Paul)
	{
		m_waveletType = "paul";

		if (m_waveletParam <= 0 || m_waveletParam > 20)
		{
			getLogManager() << LogLevel_Error << "Paul wavelet parameter should be included in ]0,20].\n";
			return false;
		}
		if (std::ceil(m_waveletParam) != m_waveletParam)
		{
			getLogManager() << LogLevel_Error << "Paul wavelet parameter should be an integer.\n";
			return false;
		}
	}
	else if (type == EContinuousWaveletType::DOG)
	{
		m_waveletType = "dog";

		if (m_waveletParam <= 0 || size_t(m_waveletParam) % 2 == 1)
		{
			getLogManager() << LogLevel_Error << "Derivative of Gaussian wavelet parameter should be strictly positive and even.\n";
			return false;
		}
		if (std::ceil(m_waveletParam) != m_waveletParam)
		{
			getLogManager() << LogLevel_Error << "Derivative of Gaussian wavelet parameter should be an integer.\n";
			return false;
		}
	}
	else
	{
		getLogManager() << LogLevel_Error << "Unknown wavelet type.\n";
		return false;
	}

	if (m_nScaleJ <= 0)
	{
		getLogManager() << LogLevel_Error << "Number of frequencies can not be negative.\n";
		return false;
	}
	if (m_highestFreq <= 0)
	{
		getLogManager() << LogLevel_Error << "Highest frequency can not be negative.\n";
		return false;
	}
	if (frequencySpacing <= 0)
	{
		getLogManager() << LogLevel_Error << "Frequency spacing can not be negative.\n";
		return false;
	}

	m_smallestScaleS0 = SigProSTD::WaveletFreq2Scale(m_waveletType, m_waveletParam, m_highestFreq);
	m_scaleSpacingDj  = SigProSTD::WaveletFreq2Scale(m_waveletType, m_waveletParam, frequencySpacing);

	m_scaleType        = "pow";
	m_scalePowerBaseA0 = 2; // base of power if ScaleType = "pow"

	return true;
}

bool CBoxAlgorithmContinuousWaveletAnalysis::uninitialize()
{
	m_decoder.uninitialize();
	for (auto& e : m_encoders) { e.uninitialize(); }

	cwt_free(m_waveletTransform);
	m_waveletTransform = nullptr;

	return true;
}

bool CBoxAlgorithmContinuousWaveletAnalysis::processInput(const size_t /*index*/)
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
		m_decoder.decode(i);
		IMatrix* iMatrix = m_decoder.getOutputMatrix();
		size_t nChannel  = iMatrix->getDimensionSize(0);
		size_t nSample   = iMatrix->getDimensionSize(1);

		if (m_decoder.isHeaderReceived())
		{
			size_t sampling = m_decoder.getOutputSamplingRate();
			getLogManager() << LogLevel_Trace << "Input signal is [" << nChannel << " x " << nSample << "] @ " << sampling << "Hz.\n";
			if (sampling == 0)
			{
				getLogManager() << LogLevel_Error << "Input sampling frequency is equal to 0. Plugin can not process.\n";
				return false;
			}
			m_samplingPeriodDt = 1.0 / sampling;

			if (m_highestFreq > 0.5 * sampling)
			{
				getLogManager() << LogLevel_Error << "Highest frequency (" << m_highestFreq << " Hz) is above Nyquist criterion (sampling rate is "
						<< sampling << " Hz), can not proceed!\n";
				return false;
			}

			const int nScaleLimit = int(std::log2(nSample * m_samplingPeriodDt / m_smallestScaleS0) / m_scaleSpacingDj); // Eq.(10)
			if (int(m_nScaleJ) > nScaleLimit)
			{
				getLogManager() << LogLevel_Error << "Frequency count [" << m_nScaleJ << "] is superior to the limit [" << nScaleLimit << "].\n";
				return false;
			}

			// initialize CWT
			m_waveletTransform = cwt_init(const_cast<char*>(m_waveletType), m_waveletParam, int(nSample), m_samplingPeriodDt, int(m_nScaleJ));
			if (!m_waveletTransform)
			{
				getLogManager() << LogLevel_Error << "Error during CWT initialization.\n";
				return false;
			}

			// define scales of CWT
			if (setCWTScales(m_waveletTransform, m_smallestScaleS0, m_scaleSpacingDj, const_cast<char*>(m_scaleType), m_scalePowerBaseA0) != 0)
			{
				getLogManager() << LogLevel_Error << "Error during CWT scales definition.\n";
				return false;
			}
			//cwt_summary(m_waveletTransform); // FOR DEBUG

			for (size_t j = 0; j < 4; ++j)
			{
				IMatrix* oMatrix = m_encoders[j].getInputMatrix();
				oMatrix->setDimensionCount(3);
				oMatrix->setDimensionSize(0, nChannel);
				oMatrix->setDimensionSize(1, m_nScaleJ);
				oMatrix->setDimensionSize(2, nSample);

				for (size_t c = 0; c < nChannel; ++c) { oMatrix->setDimensionLabel(0, c, iMatrix->getDimensionLabel(0, c)); }
				for (size_t scaleIndex = 0; scaleIndex < m_nScaleJ; ++scaleIndex)
				{
					const double scaleValue     = m_waveletTransform->scale[scaleIndex];
					const double frequencyValue = SigProSTD::WaveletScale2Freq(m_waveletType, m_waveletParam, scaleValue);

					std::string frequencyString = std::to_string(frequencyValue);
					oMatrix->setDimensionLabel(1, scaleIndex, frequencyString.c_str());
				}
				for (size_t sampleIdx = 0; sampleIdx < nSample; ++sampleIdx)
				{
					std::string sampleString = std::to_string(sampleIdx * m_samplingPeriodDt);
					oMatrix->setDimensionLabel(2, sampleIdx, sampleString.c_str());
				}
				m_encoders[j].encodeHeader();
			}
		}
		if (m_decoder.isBufferReceived())
		{
			double* ibuffer          = iMatrix->getBuffer();
			double* oAmplitudeBuffer = m_encoders[0].getInputMatrix()->getBuffer();
			double* oPhaseBuffer     = m_encoders[1].getInputMatrix()->getBuffer();
			double* oRealPartBuffer  = m_encoders[2].getInputMatrix()->getBuffer();
			double* oImagPartBuffer  = m_encoders[3].getInputMatrix()->getBuffer();

			for (size_t c = 0; c < nChannel; ++c)
			{
				// compute CWT
				if (cwt(m_waveletTransform, ibuffer) != 0)
				{
					getLogManager() << LogLevel_Error << "Error during CWT computation.\n";
					return false;
				}

				// format of m_waveletTransform->output: dimensions = m_nScaleJ * nSample, stored in row major format
				for (size_t scaleIdx = 0; scaleIdx < m_nScaleJ; ++scaleIdx)
				{
					for (size_t sampleIdx = 0; sampleIdx < nSample; ++sampleIdx)
					{
						const double real      = m_waveletTransform->output[sampleIdx + scaleIdx * nSample].re;
						const double imag      = m_waveletTransform->output[sampleIdx + scaleIdx * nSample].im;
						const size_t outputIdx = sampleIdx + (m_nScaleJ - scaleIdx - 1) * nSample + c * nSample * m_nScaleJ; // t+f*T+c*T*F

						oAmplitudeBuffer[outputIdx] = std::sqrt(real * real + imag * imag);
						oPhaseBuffer[outputIdx]     = std::atan2(imag, real);
						oRealPartBuffer[outputIdx]  = real;
						oImagPartBuffer[outputIdx]  = imag;
					}
				}
				ibuffer += nSample;
			}

			for (auto& e : m_encoders) { e.encodeBuffer(); }
		}
		if (m_decoder.isEndReceived()) { for (auto& e : m_encoders) { e.encodeEnd(); } }
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(1, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(2, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		boxContext.markOutputAsReadyToSend(3, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
