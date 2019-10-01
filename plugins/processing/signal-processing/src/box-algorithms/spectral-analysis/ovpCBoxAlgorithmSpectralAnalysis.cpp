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
	double amplitude(const uint32_t channelIdx, const uint32_t FFTIdx, const MatrixXcd& matrix)
	{
		return sqrt(
			matrix(channelIdx, FFTIdx).real() * matrix(channelIdx, FFTIdx).real() + matrix(channelIdx, FFTIdx).imag() * matrix(
				channelIdx, FFTIdx).imag());
	}

	double phase(const uint32_t channelIdx, const uint32_t FFTIdx, const MatrixXcd& matrix)
	{
		return atan2(matrix(channelIdx, FFTIdx).imag(), matrix(channelIdx, FFTIdx).real());
	}

	double realPart(const uint32_t channelIdx, const uint32_t FFTIdx, const MatrixXcd& matrix) { return matrix(channelIdx, FFTIdx).real(); }

	double imaginaryPart(const uint32_t channelIdx, const uint32_t FFTIdx, const MatrixXcd& matrix) { return matrix(channelIdx, FFTIdx).imag(); }
} // namespace

bool CBoxAlgorithmSpectralAnalysis::initialize()
{
	m_decoder.initialize(*this, 0);

	m_frequencyAbscissa = new CMatrix();

	// Amplitude
	m_spectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 0));
	m_isSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));

	// Phase
	m_spectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 1));
	m_isSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));

	//Real Part
	m_spectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 2));
	m_isSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));

	// Imaginary part
	m_spectrumEncoders.push_back(new TSpectrumEncoder<CBoxAlgorithmSpectralAnalysis>(*this, 3));
	m_isSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));

	for (auto& curEncoder : m_spectrumEncoders)
	{
		curEncoder->getInputFrequencyAbscissa().setReferenceTarget(m_frequencyAbscissa);
		curEncoder->getInputSamplingRate().setReferenceTarget(m_decoder.getOutputSamplingRate());
	}

	this->getLogManager() << LogLevel_Trace << "Spectral components selected : [ "
			<< (m_isSpectrumEncoderActive[0] ? CString("AMP ") : "")
			<< (m_isSpectrumEncoderActive[1] ? CString("PHASE ") : "")
			<< (m_isSpectrumEncoderActive[2] ? CString("REAL ") : "")
			<< (m_isSpectrumEncoderActive[3] ? CString("IMG ") : "")
			<< "]\n";

	return true;
}

bool CBoxAlgorithmSpectralAnalysis::uninitialize()
{
	for (size_t i = 0; i < m_spectrumEncoders.size(); i++)
	{
		m_spectrumEncoders[i]->uninitialize();
		delete m_spectrumEncoders[i];
	}

	m_spectrumEncoders.clear();

	m_decoder.uninitialize();
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
	for (uint32_t i = 0; i < dynamicBoxContext->getInputChunkCount(0); i++)
	{
		const uint64_t startTime = dynamicBoxContext->getInputChunkStartTime(0, i);
		const uint64_t endTime   = dynamicBoxContext->getInputChunkEndTime(0, i);

		m_decoder.decode(i);
		IMatrix* matrix = m_decoder.getOutputMatrix();

		if (m_decoder.isHeaderReceived())
		{
			m_nChannel = matrix->getDimensionSize(0);
			m_nSample  = matrix->getDimensionSize(1);

			OV_ERROR_UNLESS_KRF(m_nSample > 1, "Input sample count lower or equal to 1 is not supported by the box.", OpenViBE::Kernel::ErrorType::BadInput);

			m_samplingRate = uint32_t(m_decoder.getOutputSamplingRate());

			OV_ERROR_UNLESS_KRF(m_samplingRate > 0, "Invalid sampling rate [" << m_samplingRate << "] (expected value > 0)", OpenViBE::Kernel::ErrorType::BadInput);

			// size of the spectrum
			m_sizeFFT = m_nSample / 2 + 1;

			// Constructing the frequency band description matrix, same for every possible output (and given through reference target mechanism)
			m_frequencyAbscissa->setDimensionCount(1);  // a list of frequencies
			m_frequencyAbscissa->setDimensionSize(0, m_sizeFFT); // FFTSize frquency abscissa

			// Frequency values
			for (uint32_t frequencyAbscissaIndex = 0; frequencyAbscissaIndex < m_sizeFFT; frequencyAbscissaIndex++)
			{
				m_frequencyAbscissa->getBuffer()[frequencyAbscissaIndex] = frequencyAbscissaIndex * (double(m_samplingRate) / m_nSample);
			}

			// All spectra share the same header structure
			for (size_t encoderIndex = 0; encoderIndex < m_spectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_isSpectrumEncoderActive[encoderIndex])
				{
					// Spectrum matrix
					IMatrix* spectrum = m_spectrumEncoders[encoderIndex]->getInputMatrix();
					spectrum->setDimensionCount(2);
					spectrum->setDimensionSize(0, m_nChannel);
					spectrum->setDimensionSize(1, m_sizeFFT);

					// Spectrum channel names
					for (uint32_t j = 0; j < m_nChannel; j++) { spectrum->setDimensionLabel(0, j, matrix->getDimensionLabel(0, j)); }

					// We also name the spectrum bands "Abscissa"
					for (uint32_t j = 0; j < m_sizeFFT; j++) { spectrum->setDimensionLabel(1, j, std::to_string(m_frequencyAbscissa->getBuffer()[j]).c_str()); }

					m_spectrumEncoders[encoderIndex]->encodeHeader();
					dynamicBoxContext->markOutputAsReadyToSend(uint32_t(encoderIndex), startTime, endTime);
				}
			}
		}

		if (m_decoder.isBufferReceived())
		{
			// Compute the FFT
			FFT<double> eigenFFT;
			eigenFFT.SetFlag(eigenFFT.HalfSpectrum); // REAL signal => spectrum with conjugate symmetry

			// This matrix will contain the channels spectra (COMPLEX values, RowMajor for copy into openvibe matrix)
			MatrixXcd spectra = MatrixXcd::Zero(m_nChannel, m_sizeFFT);

			for (uint32_t j = 0; j < m_nChannel; j++)
			{
				VectorXd samples = VectorXd::Zero(m_nSample);

				for (uint32_t k = 0; k < m_nSample; k++) { samples(k) = matrix->getBuffer()[j * m_nSample + k]; }

				VectorXcd spectrum; // initialization useless: EigenFFT resizes spectrum in function .fwd()

				// EigenFFT
				eigenFFT.fwd(spectrum, samples);

				// return of a mirror spectrum of size 2*m_sizeFFT: so we take only the first m_sizeFFT values
				spectra.row(j) = spectrum;
			}

			// multiplication by sqrt(2), since half spectrum has been removed
			if (m_nSample % 2 == 0)
			{
				// even case : DC and Nyquist bins are not concerned
				spectra.block(0, 1, m_nChannel, m_sizeFFT - 2) *= std::sqrt(2);
			}
			else
			{
				// odd case : DC bin is not concerned
				spectra.block(0, 1, m_nChannel, m_sizeFFT - 1) *= std::sqrt(2);
			}

			for (size_t encoderIndex = 0; encoderIndex < m_spectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_isSpectrumEncoderActive[encoderIndex])
				{
					std::function<double(uint32_t, uint32_t, const MatrixXcd&)> processResult;

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

					IMatrix* spectrum = m_spectrumEncoders[encoderIndex]->getInputMatrix();

					for (uint32_t j = 0; j < m_nChannel; j++)
					{
						for (uint32_t k = 0; k < m_sizeFFT; k++) { spectrum->getBuffer()[j * m_sizeFFT + k] = processResult(j, k, spectra); }
					}

					m_spectrumEncoders[encoderIndex]->encodeBuffer();
					dynamicBoxContext->markOutputAsReadyToSend(uint32_t(encoderIndex), startTime, endTime);
				}
			}
		}

		if (m_decoder.isEndReceived())
		{
			for (size_t encoderIndex = 0; encoderIndex < m_spectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_isSpectrumEncoderActive[encoderIndex])
				{
					m_spectrumEncoders[encoderIndex]->encodeEnd();
					dynamicBoxContext->markOutputAsReadyToSend(uint32_t(encoderIndex), startTime, endTime);
				}
			}
		}
	}

	return true;
}
