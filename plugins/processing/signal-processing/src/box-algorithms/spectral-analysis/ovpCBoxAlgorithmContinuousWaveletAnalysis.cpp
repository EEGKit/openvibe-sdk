#define _USE_MATH_DEFINES
#include <cmath>

#include "ovpCBoxAlgorithmContinuousWaveletAnalysis.h"

#include <string.h>

using namespace OpenViBE;
using namespace Kernel;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace OpenViBEToolkit;

namespace SigProSTD
{
	double WaveletFourierFactor(char* waveletType, const double waveletParameter)
	{
		double factor = -1;
		if (strcmp(waveletType, "morlet") == 0) { factor = 4.0 * M_PI / (waveletParameter + std::sqrt(2 + waveletParameter * waveletParameter)); }
		else if (strcmp(waveletType, "paul") == 0) { factor = 4.0 * M_PI / (2 * waveletParameter + 1); }
		else if (strcmp(waveletType, "dog") == 0) { factor = 2.0 * M_PI / std::sqrt(waveletParameter + 0.5); }
		return factor;
	}

	double WaveletScale2Period(char* waveletType, const double waveletParameter, const double scale) { return WaveletFourierFactor(waveletType, waveletParameter) * scale; }

	double WaveletScale2Freq(char* waveletType, const double waveletParameter, const double scale) { return 1.0 / (WaveletFourierFactor(waveletType, waveletParameter) * scale); }

	double WaveletFreq2Scale(char* waveletType, const double waveletParameter, const double frequency) { return 1.0 / (WaveletFourierFactor(waveletType, waveletParameter) * frequency); }
} // namespace SigProSTD

bool CBoxAlgorithmContinuousWaveletAnalysis::initialize()
{
	m_oDecoder.initialize(*this, 0);
	m_vEncoder[0].initialize(*this, 0);
	m_vEncoder[1].initialize(*this, 1);
	m_vEncoder[2].initialize(*this, 2);
	m_vEncoder[3].initialize(*this, 3);

	const uint64_t waveletType    = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_waveletParam                = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_nScaleJ                     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_highestFreq                 = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	const double frequencySpacing = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	if (waveletType == OVP_TypeId_ContinuousWaveletType_Morlet.toUInteger())
	{
		m_waveletType = "morlet";

		if (m_waveletParam < 0)
		{
			this->getLogManager() << LogLevel_Error << "Morlet wavelet parameter should be positive.\n";
			return false;
		}
	}
	else if (waveletType == OVP_TypeId_ContinuousWaveletType_Paul.toUInteger())
	{
		m_waveletType = "paul";

		if (m_waveletParam <= 0 || m_waveletParam > 20)
		{
			this->getLogManager() << LogLevel_Error << "Paul wavelet parameter should be included in ]0,20].\n";
			return false;
		}
		if (std::ceil(m_waveletParam) != m_waveletParam)
		{
			this->getLogManager() << LogLevel_Error << "Paul wavelet parameter should be an integer.\n";
			return false;
		}
	}
	else if (waveletType == OVP_TypeId_ContinuousWaveletType_DOG.toUInteger())
	{
		m_waveletType = "dog";

		if (m_waveletParam <= 0 || uint32_t(m_waveletParam) % 2 == 1)
		{
			this->getLogManager() << LogLevel_Error << "Derivative of Gaussian wavelet parameter should be strictly positive and even.\n";
			return false;
		}
		if (std::ceil(m_waveletParam) != m_waveletParam)
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

	if (m_nScaleJ <= 0)
	{
		this->getLogManager() << LogLevel_Error << "Number of frequencies can not be negative.\n";
		return false;
	}
	if (m_highestFreq <= 0)
	{
		this->getLogManager() << LogLevel_Error << "Highest frequency can not be negative.\n";
		return false;
	}
	if (frequencySpacing <= 0)
	{
		this->getLogManager() << LogLevel_Error << "Frequency spacing can not be negative.\n";
		return false;
	}

	m_smallestScaleS0 = SigProSTD::WaveletFreq2Scale(const_cast<char *>(m_waveletType), m_waveletParam, m_highestFreq);
	m_scaleSpacingDj  = SigProSTD::WaveletFreq2Scale(const_cast<char *>(m_waveletType), m_waveletParam, frequencySpacing);

	m_scaleType        = "pow";
	m_scalePowerBaseA0 = 2; // base of power if ScaleType = "pow"

	return true;
}

bool CBoxAlgorithmContinuousWaveletAnalysis::uninitialize()
{
	m_oDecoder.uninitialize();
	m_vEncoder[0].uninitialize();
	m_vEncoder[1].uninitialize();
	m_vEncoder[2].uninitialize();
	m_vEncoder[3].uninitialize();

	cwt_free(m_waveletTransform);
	m_waveletTransform = nullptr;

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
		size_t nSample   = iMatrix->getDimensionSize(1);

		if (m_oDecoder.isHeaderReceived())
		{
			size_t samplingRate = m_oDecoder.getOutputSamplingRate();
			this->getLogManager() << LogLevel_Trace << "Input signal is [" << nChannel << " x " << nSample << "] @ " << samplingRate << "Hz.\n";
			if (samplingRate == 0)
			{
				this->getLogManager() << LogLevel_Error << "Input sampling frequency is equal to 0. Plugin can not process.\n";
				return false;
			}
			m_samplingPeriodDt = 1.0 / samplingRate;

			if (m_highestFreq > 0.5 * samplingRate)
			{
				this->getLogManager() << LogLevel_Error << "Highest frequency (" << m_highestFreq << " Hz) is above Nyquist criterion (sampling rate is " << samplingRate << " Hz), can not proceed!\n";
				return false;
			}

			const int nScaleLimit = int(std::log2(nSample * m_samplingPeriodDt / m_smallestScaleS0) / m_scaleSpacingDj); // Eq.(10)
			if (int(m_nScaleJ) > nScaleLimit)
			{
				this->getLogManager() << LogLevel_Error << "Frequency count [" << m_nScaleJ << "] is superior to the limit [" << nScaleLimit << "].\n";
				return false;
			}

			// initialize CWT
			m_waveletTransform = cwt_init(const_cast<char *>(m_waveletType), m_waveletParam, nSample, m_samplingPeriodDt, m_nScaleJ);
			if (!m_waveletTransform)
			{
				this->getLogManager() << LogLevel_Error << "Error during CWT initialization.\n";
				return false;
			}

			// define scales of CWT
			if (setCWTScales(m_waveletTransform, m_smallestScaleS0, m_scaleSpacingDj, const_cast<char*>(m_scaleType), m_scalePowerBaseA0) != 0)
			{
				this->getLogManager() << LogLevel_Error << "Error during CWT scales definition.\n";
				return false;
			}
			//cwt_summary(m_waveletTransform); // FOR DEBUG

			for (size_t j = 0; j < 4; ++j)
			{
				IMatrix* oMatrix = m_vEncoder[j].getInputMatrix();
				oMatrix->setDimensionCount(3);
				oMatrix->setDimensionSize(0, nChannel);
				oMatrix->setDimensionSize(1, uint32_t(m_nScaleJ));
				oMatrix->setDimensionSize(2, uint32_t(nSample));

				for (size_t c = 0; c < nChannel; ++c) { oMatrix->setDimensionLabel(0, c, iMatrix->getDimensionLabel(0, c)); }
				for (size_t scaleIndex = 0; scaleIndex < m_nScaleJ; ++scaleIndex)
				{
					const double scaleValue     = m_waveletTransform->scale[scaleIndex];
					const double frequencyValue = SigProSTD::WaveletScale2Freq(const_cast<char *>(m_waveletType), m_waveletParam, scaleValue);

					std::string frequencyString = std::to_string(frequencyValue);
					oMatrix->setDimensionLabel(1, scaleIndex, frequencyString.c_str());
				}
				for (size_t sampleIdx = 0; sampleIdx < nSample; ++sampleIdx)
				{
					std::string sampleString = std::to_string(sampleIdx * m_samplingPeriodDt);
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

			for (size_t c = 0; c < nChannel; ++c)
			{
				// compute CWT
				if (cwt(m_waveletTransform, ibuffer) != 0)
				{
					this->getLogManager() << LogLevel_Error << "Error during CWT computation.\n";
					return false;
				}

				// format of m_waveletTransform->output: dimensions = m_nScaleJ * l_iSampleCount, stored in row major format
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
