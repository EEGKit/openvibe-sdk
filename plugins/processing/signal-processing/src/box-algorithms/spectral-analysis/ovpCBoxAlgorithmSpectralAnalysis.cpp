#include "ovpCBoxAlgorithmSpectralAnalysis.h"

#include <Eigen/Eigen>
// additional Eigen module
#include <unsupported/Eigen/FFT>

#include <cmath>
#include <sstream>
#include <cstdio>
#include <iostream>
#include <functional>

using namespace Eigen;

using namespace OpenViBE;
using namespace Kernel;
using namespace OpenViBEPlugins;
using namespace SignalProcessing;
using namespace OpenViBEToolkit;

namespace
{
	double amplitude(unsigned int channelIndex, unsigned int FFTIndex, const MatrixXcd& matrix)
	{
		return sqrt(matrix(channelIndex, FFTIndex).real() * matrix(channelIndex, FFTIndex).real() + matrix(channelIndex, FFTIndex).imag() * matrix(channelIndex, FFTIndex).imag());
	}

	double phase(unsigned int channelIndex, unsigned int FFTIndex, const MatrixXcd& matrix)
	{
		return atan2(matrix(channelIndex, FFTIndex).imag(), matrix(channelIndex, FFTIndex).real());
	}

	double realPart(unsigned int channelIndex, unsigned int FFTIndex, const MatrixXcd& matrix)
	{
		return matrix(channelIndex, FFTIndex).real();
	}

	double imaginaryPart(unsigned int channelIndex, unsigned int FFTIndex, const MatrixXcd& matrix)
	{
		return matrix(channelIndex, FFTIndex).imag();
	}
} // namespace

bool CBoxAlgorithmSpectralAnalysis::initialize()
{
	m_Decoder.initialize(*this, 0);

	m_FrequencyAbscissa = new CMatrix();

	// Amplitude
	m_SpectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 0));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));

	// Phase
	m_SpectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 1));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));

	//Real Part
	m_SpectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 2));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));

	// Imaginary part
	m_SpectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 3));
	m_IsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));

	for (auto& curEncoder : m_SpectrumEncoders)
	{
		curEncoder->getInputFrequencyAbscissa().setReferenceTarget(m_FrequencyAbscissa);
		curEncoder->getInputSamplingRate().setReferenceTarget(m_Decoder.getOutputSamplingRate());
	}

	this->getLogManager() << LogLevel_Trace << "Spectral components selected : [ "
			<< (m_IsSpectrumEncoderActive[0] ? CString("AMP ") : "")
			<< (m_IsSpectrumEncoderActive[1] ? CString("PHASE ") : "")
			<< (m_IsSpectrumEncoderActive[2] ? CString("REAL ") : "")
			<< (m_IsSpectrumEncoderActive[3] ? CString("IMG ") : "")
			<< "]\n";

	return true;
}

bool CBoxAlgorithmSpectralAnalysis::uninitialize()
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

bool CBoxAlgorithmSpectralAnalysis::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSpectralAnalysis::process()
{
	IBoxIO* dynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for (unsigned int i = 0; i < dynamicBoxContext->getInputChunkCount(0); i++)
	{
		const uint64_t startTime = dynamicBoxContext->getInputChunkStartTime(0, i);
		const uint64_t endTime   = dynamicBoxContext->getInputChunkEndTime(0, i);

		m_Decoder.decode(i);
		IMatrix* matrix = m_Decoder.getOutputMatrix();

		if (m_Decoder.isHeaderReceived())
		{
			m_ChannelCount = matrix->getDimensionSize(0);
			m_SampleCount  = matrix->getDimensionSize(1);

			OV_ERROR_UNLESS_KRF(m_SampleCount > 1, "Input sample count lower or equal to 1 is not supported by the box.", OpenViBE::Kernel::ErrorType::BadInput);

			m_SamplingRate = (unsigned int)m_Decoder.getOutputSamplingRate();

			OV_ERROR_UNLESS_KRF(m_SamplingRate > 0, "Invalid sampling rate [" << m_SamplingRate << "] (expected value > 0)", OpenViBE::Kernel::ErrorType::BadInput);

			// size of the spectrum
			m_FFTSize = m_SampleCount / 2 + 1;

			// Constructing the frequency band description matrix, same for every possible output (and given through reference target mechanism)
			m_FrequencyAbscissa->setDimensionCount(1);  // a list of frequencies
			m_FrequencyAbscissa->setDimensionSize(0, m_FFTSize); // FFTSize frquency abscissa

			// Frequency values
			for (unsigned int frequencyAbscissaIndex = 0; frequencyAbscissaIndex < m_FFTSize; frequencyAbscissaIndex++)
			{
				m_FrequencyAbscissa->getBuffer()[frequencyAbscissaIndex] = frequencyAbscissaIndex * (double(m_SamplingRate) / m_SampleCount);
			}

			// All spectra share the same header structure
			for (size_t encoderIndex = 0; encoderIndex < m_SpectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_IsSpectrumEncoderActive[encoderIndex])
				{
					// Spectrum matrix
					IMatrix* spectrum = m_SpectrumEncoders[encoderIndex]->getInputMatrix();
					spectrum->setDimensionCount(2);
					spectrum->setDimensionSize(0, m_ChannelCount);
					spectrum->setDimensionSize(1, m_FFTSize);

					// Spectrum channel names
					for (size_t j = 0; j < m_ChannelCount; j++)
					{
						spectrum->setDimensionLabel(0, uint32_t(j), matrix->getDimensionLabel(0, j));
					}

					// We also name the spectrum bands "Abscissa"
					for (unsigned int j = 0; j < m_FFTSize; j++)
					{
						char frequencyBandName[1024];
						sprintf(frequencyBandName, "%lg", m_FrequencyAbscissa->getBuffer()[j]);
						spectrum->setDimensionLabel(1, uint32_t(j), frequencyBandName);
					}

					m_SpectrumEncoders[encoderIndex]->encodeHeader();
					dynamicBoxContext->markOutputAsReadyToSend(uint32_t(encoderIndex), startTime, endTime);
				}
			}
		}

		if (m_Decoder.isBufferReceived())
		{
			// Compute the FFT
			FFT<double> eigenFFT;
			eigenFFT.SetFlag(eigenFFT.HalfSpectrum); // REAL signal => spectrum with conjugate symmetry

			// This matrix will contain the channels spectra (COMPLEX values, RowMajor for copy into openvibe matrix)
			MatrixXcd spectra = MatrixXcd::Zero(m_ChannelCount, m_FFTSize);

			for (unsigned int j = 0; j < m_ChannelCount; j++)
			{
				VectorXd samples = VectorXd::Zero(m_SampleCount);

				for (unsigned int k = 0; k < m_SampleCount; k++)
				{
					samples(k) = matrix->getBuffer()[j * m_SampleCount + k];
				}

				VectorXcd spectrum; // initialization useless: EigenFFT resizes spectrum in function .fwd()

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
					std::function<double(unsigned int, unsigned int, const MatrixXcd&)> processResult;

					switch (encoderIndex)
					{
						case 0:
							processResult = amplitude;
							break;

						case 1:
							processResult = phase;
							break;

						case 2:
							processResult = realPart;
							break;

						case 3:
							processResult = imaginaryPart;
							break;

						default:
							OV_ERROR_KRF("Invalid decoder output.\n", OpenViBE::Kernel::ErrorType::BadProcessing);
					}

					IMatrix* spectrum = m_SpectrumEncoders[encoderIndex]->getInputMatrix();

					for (unsigned int j = 0; j < m_ChannelCount; j++)
					{
						for (unsigned int k = 0; k < m_FFTSize; k++)
						{
							spectrum->getBuffer()[j * m_FFTSize + k] = processResult(j, k, spectra);
						}
					}

					m_SpectrumEncoders[encoderIndex]->encodeBuffer();
					dynamicBoxContext->markOutputAsReadyToSend(uint32_t(encoderIndex), startTime, endTime);
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
					dynamicBoxContext->markOutputAsReadyToSend(uint32_t(encoderIndex), startTime, endTime);
				}
			}
		}
	}

	return true;
}
