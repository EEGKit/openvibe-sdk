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
	m_oDecoder.initialize(*this, 0);

	m_pFrequencyBandDescription = new CMatrix();

	// Amplitude
	m_vSpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 0));
	m_vIsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));

	// Phase
	m_vSpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 1));
	m_vIsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1));

	//Real Part
	m_vSpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 2));
	m_vIsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));

	// Imaginary part
	m_vSpectrumEncoders.push_back(new TSpectrumEncoder < CBoxAlgorithmSpectralAnalysis >(*this, 3));
	m_vIsSpectrumEncoderActive.push_back(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3));

	//All encoders share the same frequency band description
	m_vSpectrumEncoders[0]->getInputMinMaxFrequencyBands().setReferenceTarget(m_pFrequencyBandDescription);
	m_vSpectrumEncoders[1]->getInputMinMaxFrequencyBands().setReferenceTarget(m_pFrequencyBandDescription);
	m_vSpectrumEncoders[2]->getInputMinMaxFrequencyBands().setReferenceTarget(m_pFrequencyBandDescription);
	m_vSpectrumEncoders[3]->getInputMinMaxFrequencyBands().setReferenceTarget(m_pFrequencyBandDescription);

	this->getLogManager() << LogLevel_Trace << "Spectral components selected : [ "
		<< (m_vIsSpectrumEncoderActive[0] ? CString("AMP ") : "")
		<< (m_vIsSpectrumEncoderActive[1] ? CString("PHASE ") : "")
		<< (m_vIsSpectrumEncoderActive[2] ? CString("REAL ") : "")
		<< (m_vIsSpectrumEncoderActive[3] ? CString("IMG ") : "")
		<< "]\n";

	return true;
}

boolean CBoxAlgorithmSpectralAnalysis::uninitialize()
{
	for (size_t i = 0; i < m_vSpectrumEncoders.size(); i++)
	{
		m_vSpectrumEncoders[i]->uninitialize();
		delete m_vSpectrumEncoders[i];
	}

	m_vSpectrumEncoders.clear();

	m_oDecoder.uninitialize();
	return true;
}

boolean CBoxAlgorithmSpectralAnalysis::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmSpectralAnalysis::process()
{
	IBoxIO* l_pDynamicBoxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for (uint32 i = 0; i < l_pDynamicBoxContext->getInputChunkCount(0); i++)
	{
		uint64 l_ui64StartTime = l_pDynamicBoxContext->getInputChunkStartTime(0, i);
		uint64 l_ui64EndTime = l_pDynamicBoxContext->getInputChunkEndTime(0, i);

		m_oDecoder.decode(i);
		IMatrix * l_pMatrix = m_oDecoder.getOutputMatrix();

		if (m_oDecoder.isHeaderReceived())
		{
			m_ui32ChannelCount = l_pMatrix->getDimensionSize(0);
			m_ui32SampleCount = l_pMatrix->getDimensionSize(1);
			m_ui32SamplingRate = (uint32)m_oDecoder.getOutputSamplingRate();

			OV_ERROR_UNLESS_KRF(
				m_ui32SamplingRate > 0,
				"Invalid sampling rate [" << m_ui32SamplingRate << "] (expected value > 0)",
				OpenViBE::Kernel::ErrorType::BadInput
				);

			// size of the spectrum
			m_ui32FFTSize = m_ui32SampleCount / 2 + 1;

			// Constructing the frequency band description matrix, same for every possible output (and given through reference target mechanism)
			m_pFrequencyBandDescription->setDimensionCount(2);  // a list of (min,max) pairs
			m_pFrequencyBandDescription->setDimensionSize(0, 2); // Min and Max
			m_pFrequencyBandDescription->setDimensionSize(1, m_ui32FFTSize); // FFTSize bands 

			// Min Max frequency band values
			for (uint32 j = 0; j < m_ui32FFTSize; j++)
			{
				float64 l_float64BandStart = j * (static_cast<double>(m_ui32SamplingRate / 2.) / m_ui32FFTSize);
				float64 l_float64BandStop = (j + 1) * (static_cast<double>(m_ui32SamplingRate / 2.) / m_ui32FFTSize);

				if (l_float64BandStop < l_float64BandStart)
				{
					l_float64BandStop = l_float64BandStart;
				}

				m_pFrequencyBandDescription->getBuffer()[j * 2] = l_float64BandStart;
				m_pFrequencyBandDescription->getBuffer()[j * 2 + 1] = l_float64BandStop;
			}

			// All spectra share the same header structure
			for (size_t encoderIndex = 0; encoderIndex < m_vSpectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_vIsSpectrumEncoderActive[encoderIndex])
				{
					// Spectrum matrix
					IMatrix * l_pSpectrum = m_vSpectrumEncoders[encoderIndex]->getInputMatrix();
					l_pSpectrum->setDimensionCount(2);
					l_pSpectrum->setDimensionSize(0, m_ui32ChannelCount);
					l_pSpectrum->setDimensionSize(1, m_ui32FFTSize);

					// Spectrum channel names
					for (uint32 j = 0; j < m_ui32ChannelCount; j++)
					{
						l_pSpectrum->setDimensionLabel(0, j, l_pMatrix->getDimensionLabel(0, j));
					}

					// We also name the spectrum bands "Min-Max"
					for (uint32 j = 0; j < m_ui32FFTSize; j++)
					{
						char l_sFrequencyBandName[1024];
						sprintf(l_sFrequencyBandName, "%lg-%lg", m_pFrequencyBandDescription->getBuffer()[j * 2], m_pFrequencyBandDescription->getBuffer()[j * 2 + 1]);
						l_pSpectrum->setDimensionLabel(1, j, l_sFrequencyBandName);
					}

					m_vSpectrumEncoders[encoderIndex]->encodeHeader();
					l_pDynamicBoxContext->markOutputAsReadyToSend(encoderIndex, l_ui64StartTime, l_ui64EndTime);
				}
			}
		}

		if (m_oDecoder.isBufferReceived())
		{
			// Compute the FFT
			Eigen::FFT<double> l_oEigenFFT;
			l_oEigenFFT.SetFlag(l_oEigenFFT.HalfSpectrum); // REAL signal => spectrum with conjugate symmetry

			// This matrix will contain the channels spectra (COMPLEX values, RowMajor for copy into openvibe matrix)
			Eigen::MatrixXcd l_oSpectra = Eigen::MatrixXcd::Zero(m_ui32ChannelCount, m_ui32FFTSize);

			for (uint32 j = 0; j < m_ui32ChannelCount; j++)
			{
				Eigen::VectorXd l_oSamples = Eigen::VectorXd::Zero(m_ui32SampleCount);

				for (uint32 k = 0; k < m_ui32SampleCount; k++)
				{
					l_oSamples(k) = l_pMatrix->getBuffer()[j * m_ui32SampleCount + k];
				}

				Eigen::VectorXcd l_oSpectrum; // initialization useless: EigenFFT resizes l_oSpectrum in function .fwd()

				// EigenFFT
				l_oEigenFFT.fwd(l_oSpectrum, l_oSamples);

				// return of a mirror spectrum of size 2*m_ui32FFTSize: so we take only the first m_ui32FFTSize values
				l_oSpectra.row(j) = l_oSpectrum;
			}

			// multiplication by sqrt(2), since half spectrum has been removed
			if (m_ui32SampleCount % 2 == 0)
			{
				// even case : DC and Nyquist bins are not concerned
				l_oSpectra.block(0, 1, m_ui32ChannelCount, m_ui32FFTSize - 2) *= std::sqrt(2);
			}
			else
			{
				// odd case : DC bin is not concerned
				l_oSpectra.block(0, 1, m_ui32ChannelCount, m_ui32FFTSize - 1) *= std::sqrt(2);
			}

			for (size_t encoderIndex = 0; encoderIndex < m_vSpectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_vIsSpectrumEncoderActive[encoderIndex])
				{
					IMatrix * l_pSpectrum = m_vSpectrumEncoders[encoderIndex]->getInputMatrix();

					switch (encoderIndex)
					{
						case 0:
							for (uint32 j = 0; j < m_ui32ChannelCount; j++)
							{
								for (uint32 k = 0; k < m_ui32FFTSize; k++)
								{
									float64 l_f64Amplitude = sqrt(l_oSpectra(j, k).real() * l_oSpectra(j, k).real() + l_oSpectra(j, k).imag() * l_oSpectra(j, k).imag());
									l_pSpectrum->getBuffer()[j * m_ui32FFTSize + k] = l_f64Amplitude;
								}
							}
							break;

						case 1:
							for (uint32 j = 0; j < m_ui32ChannelCount; j++)
							{
								for (uint32 k = 0; k < m_ui32FFTSize; k++)
								{
									float64 l_f64Phase = ::atan2(l_oSpectra(j, k).imag(), l_oSpectra(j, k).real());
									l_pSpectrum->getBuffer()[j * m_ui32FFTSize + k] = l_f64Phase;
								}
							}
							break;

						case 2:
							for (uint32 j = 0; j < m_ui32ChannelCount; j++)
							{
								for (uint32 k = 0; k < m_ui32FFTSize; k++)
								{
									float64 l_f64RealPart = l_oSpectra(j, k).real();
									l_pSpectrum->getBuffer()[j * m_ui32FFTSize + k] = l_f64RealPart;
								}
							}
							break;

						case 3:
							for (uint32 j = 0; j < m_ui32ChannelCount; j++)
							{
								for (uint32 k = 0; k < m_ui32FFTSize; k++)
								{
									float64 l_f64ImagPart = l_oSpectra(j, k).imag();
									l_pSpectrum->getBuffer()[j * m_ui32FFTSize + k] = l_f64ImagPart;
								}
							}
							break;

						default:
							OV_ERROR_KRF(
								"Invalid decoder output.\n",
								OpenViBE::Kernel::ErrorType::BadProcessing
								);
					}

					m_vSpectrumEncoders[encoderIndex]->encodeBuffer();
					l_pDynamicBoxContext->markOutputAsReadyToSend(encoderIndex, l_ui64StartTime, l_ui64EndTime);
				}
			}
		}

		if (m_oDecoder.isEndReceived())
		{
			for (size_t encoderIndex = 0; encoderIndex < m_vSpectrumEncoders.size(); encoderIndex++)
			{
				// We build the chunk only if the encoder is activated
				if (m_vIsSpectrumEncoderActive[encoderIndex])
				{
					m_vSpectrumEncoders[encoderIndex]->encodeEnd();
					l_pDynamicBoxContext->markOutputAsReadyToSend(encoderIndex, l_ui64StartTime, l_ui64EndTime);
				}
			}
		}
	}

	return true;
}
