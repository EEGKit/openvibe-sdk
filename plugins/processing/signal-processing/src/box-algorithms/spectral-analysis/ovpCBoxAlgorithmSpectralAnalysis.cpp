#include "ovpCBoxAlgorithmSpectralAnalysis.h"

#include <Eigen/Eigen>
// additional Eigen module
#include <unsupported/Eigen/FFT>

#include <cmath>
#include <sstream>
#include <cstdio>
#include <iostream>

using namespace Eigen;

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;

boolean CBoxAlgorithmSpectralAnalysis::initialize()
{
	m_Decoder.initialize(*this, 0);

	m_FrequencyBandDescription = new CMatrix();

	// Amplitude
	m_SpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 0));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));

	// Phase
	m_SpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 1));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));

	//Real Part
	m_SpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 2));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));

	// Imaginary part
	m_SpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 3));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));

	//All encoders share the same frequency band description
	m_SpectrumEncoders[0]->getInputMinMaxFrequencyBands().setReferenceTarget(m_FrequencyBandDescription);
	m_SpectrumEncoders[1]->getInputMinMaxFrequencyBands().setReferenceTarget(m_FrequencyBandDescription);
	m_SpectrumEncoders[2]->getInputMinMaxFrequencyBands().setReferenceTarget(m_FrequencyBandDescription);
	m_SpectrumEncoders[3]->getInputMinMaxFrequencyBands().setReferenceTarget(m_FrequencyBandDescription);

	this->getLogManager() << LogLevel_Trace << "Spectral components selected : [ "
		<< (m_IsSpectrumEncoderActive[0] ? CString("AMP ") : "")
		<< (m_IsSpectrumEncoderActive[1] ? CString("PHASE ") : "")
		<< (m_IsSpectrumEncoderActive[2] ? CString("REAL ") : "")
		<< (m_IsSpectrumEncoderActive[3] ? CString("IMG ") : "")
		<< "]\n";

	return true;
}

boolean CBoxAlgorithmSpectralAnalysis::uninitialize()
{
	for (size_t i = 0; i < m_SpectrumEncoders.size(); i++)
	{
		m_SpectrumEncoders[i]->uninitialize();
		delete m_SpectrumEncoders[i];
	}

	m_SpectrumEncoders.clear();

	m_Decoder.uninitialize();
	return true;
}

boolean CBoxAlgorithmSpectralAnalysis::processInput(unsigned int ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmSpectralAnalysis::process()
{
	IBoxIO* dynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for (unsigned int i = 0; i < dynamicBoxContext->getInputChunkCount(0); i++)
	{
		unsigned long long startTime = dynamicBoxContext->getInputChunkStartTime(0, i);
		unsigned long long endTime = dynamicBoxContext->getInputChunkEndTime(0, i);

		m_Decoder.decode(i);
		IMatrix * matrix = m_Decoder.getOutputMatrix();

		if (m_Decoder.isHeaderReceived())
		{
			m_ChannelCount = matrix->getDimensionSize(0);
			m_SampleCount = matrix->getDimensionSize(1);
			m_SamplingRate = (unsigned int)m_Decoder.getOutputSamplingRate();

			OV_ERROR_UNLESS_KRF(
				m_SamplingRate > 0,
				"Invalid sampling rate [" << m_SamplingRate << "] (expected value > 0)",
				OpenViBE::Kernel::ErrorType::BadInput
				);

			// size of the spectrum
			m_FFTSize = m_SampleCount / 2 + 1;

			// Constructing the frequency band description matrix, same for every possible output (and given through reference target mechanism)
			m_FrequencyBandDescription->setDimensionCount(2);  // a list of (min,max) pairs
			m_FrequencyBandDescription->setDimensionSize(0, 2); // Min and Max
			m_FrequencyBandDescription->setDimensionSize(1, m_FFTSize); // FFTSize bands 

			// Min Max frequency band values
			for (unsigned long long j = 0; j < m_FFTSize; j++)
			{
				double bandStart = j * (static_cast<double>(m_SamplingRate / 2.) / m_FFTSize);
				double bandStop = (j + 1) * (static_cast<double>(m_SamplingRate / 2.) / m_FFTSize);

				if (bandStop < bandStart)
				{
					bandStop = bandStart;
				}

				m_FrequencyBandDescription->getBuffer()[j * 2] = bandStart;
				m_FrequencyBandDescription->getBuffer()[j * 2 + 1] = bandStop;
			}

			// All spectra share the same header structure
			for (size_t encoderIndex = 0; encoderIndex < m_SpectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_IsSpectrumEncoderActive[encoderIndex])
				{
					// Spectrum matrix
					IMatrix * spectrum = m_SpectrumEncoders[encoderIndex]->getInputMatrix();
					spectrum->setDimensionCount(2);
					spectrum->setDimensionSize(0, m_ChannelCount);
					spectrum->setDimensionSize(1, m_FFTSize);

					// Spectrum channel names
					for (unsigned int j = 0; j < m_ChannelCount; j++)
					{
						spectrum->setDimensionLabel(0, j, matrix->getDimensionLabel(0, j));
					}

					// We also name the spectrum bands "Min-Max"
					for (unsigned int j = 0; j < m_FFTSize; j++)
					{
						char frequencyBandName[1024];
						sprintf(frequencyBandName, "%lg-%lg", m_FrequencyBandDescription->getBuffer()[j * 2], m_FrequencyBandDescription->getBuffer()[j * 2 + 1]);
						spectrum->setDimensionLabel(1, j, frequencyBandName);
					}

					m_SpectrumEncoders[encoderIndex]->encodeHeader();
					dynamicBoxContext->markOutputAsReadyToSend(encoderIndex, startTime, endTime);
				}
			}
		}

		if (m_Decoder.isBufferReceived())
		{
			// Compute the FFT
			Eigen::FFT<double> eigenFFT;
			eigenFFT.SetFlag(eigenFFT.HalfSpectrum); // REAL signal => spectrum with conjugate symmetry

			// This matrix will contain the channels spectra (COMPLEX values, RowMajor for copy into openvibe matrix)
			Eigen::MatrixXcd spectra = Eigen::MatrixXcd::Zero(m_ChannelCount, m_FFTSize);

			for (unsigned int j = 0; j < m_ChannelCount; j++)
			{
				Eigen::VectorXd samples = Eigen::VectorXd::Zero(m_SampleCount);

				for (unsigned int k = 0; k < m_SampleCount; k++)
				{
					samples(k) = matrix->getBuffer()[j * m_SampleCount + k];
				}

				Eigen::VectorXcd spectrum; // initialization useless: EigenFFT resizes spectrum in function .fwd()

				// EigenFFT
				eigenFFT.fwd(spectrum, samples);

				// return of a mirror spectrum of size 2*m_FFTSize: so we take only the first m_FFTSize values
				spectra.row(j) = spectrum;
			}

			// multiplication by sqrt(2), since half spectrum has been removed
			if (m_SampleCount % 2 == 0)
			{
				// even case : DC and Nyquist bins are not concerned
				spectra.block(0, 1, m_ChannelCount, m_FFTSize - 2) *= std::sqrt(2);
			}
			else
			{
				// odd case : DC bin is not concerned
				spectra.block(0, 1, m_ChannelCount, m_FFTSize - 1) *= std::sqrt(2);
			}

			for (size_t encoderIndex = 0; encoderIndex < m_SpectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_IsSpectrumEncoderActive[encoderIndex])
				{
					IMatrix * spectrum = m_SpectrumEncoders[encoderIndex]->getInputMatrix();

					switch (encoderIndex)
					{
						case 0:
							for (unsigned int j = 0; j < m_ChannelCount; j++)
							{
								for (unsigned int k = 0; k < m_FFTSize; k++)
								{
									double amplitude = sqrt(spectra(j, k).real() * spectra(j, k).real() + spectra(j, k).imag() * spectra(j, k).imag());
									spectrum->getBuffer()[j * m_FFTSize + k] = amplitude;
								}
							}
							break;

						case 1:
							for (unsigned int j = 0; j < m_ChannelCount; j++)
							{
								for (unsigned int k = 0; k < m_FFTSize; k++)
								{
									double phase = ::atan2(spectra(j, k).imag(), spectra(j, k).real());
									spectrum->getBuffer()[j * m_FFTSize + k] = phase;
								}
							}
							break;

						case 2:
							for (unsigned int j = 0; j < m_ChannelCount; j++)
							{
								for (unsigned int k = 0; k < m_FFTSize; k++)
								{
									double realPart = spectra(j, k).real();
									spectrum->getBuffer()[j * m_FFTSize + k] = realPart;
								}
							}
							break;

						case 3:
							for (unsigned int j = 0; j < m_ChannelCount; j++)
							{
								for (unsigned int k = 0; k < m_FFTSize; k++)
								{
									double imagPart = spectra(j, k).imag();
									spectrum->getBuffer()[j * m_FFTSize + k] = imagPart;
								}
							}
							break;

						default:
							OV_ERROR_KRF(
								"Invalid decoder output.\n",
								OpenViBE::Kernel::ErrorType::BadProcessing
								);
					}

					m_SpectrumEncoders[encoderIndex]->encodeBuffer();
					dynamicBoxContext->markOutputAsReadyToSend(encoderIndex, startTime, endTime);
				}
			}
		}

		if (m_Decoder.isEndReceived())
		{
			for (size_t encoderIndex = 0; encoderIndex < m_SpectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_IsSpectrumEncoderActive[encoderIndex])
				{
					m_SpectrumEncoders[encoderIndex]->encodeEnd();
					dynamicBoxContext->markOutputAsReadyToSend(encoderIndex, startTime, endTime);
				}
			}
		}
	}

	return true;
}
