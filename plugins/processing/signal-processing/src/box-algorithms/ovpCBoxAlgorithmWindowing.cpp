#define _USE_MATH_DEFINES
#include <cmath>

#include "ovpCBoxAlgorithmWindowing.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace OpenViBEPlugins;
using namespace SignalProcessing;
using namespace OpenViBEToolkit;

bool CBoxAlgorithmWindowing::initialize()
{
	//reads the plugin settings
	m_windowMethod = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	if (m_windowMethod != None && m_windowMethod != Hamming && m_windowMethod != Hanning && m_windowMethod != Hann 
		&& m_windowMethod != Blackman && m_windowMethod != Triangular && m_windowMethod != SquareRoot)
	{
		OV_ERROR_KRF("No valid windowing method set.\n", OpenViBE::Kernel::ErrorType::BadSetting);
	}

	m_decoder.initialize(*this, 0);
	m_encoder.initialize(*this, 0);
	m_encoder.getInputMatrix().setReferenceTarget(m_decoder.getOutputMatrix());
	m_encoder.getInputSamplingRate().setReferenceTarget(m_decoder.getOutputSamplingRate());

	return true;
}

bool CBoxAlgorithmWindowing::uninitialize()
{
	m_decoder.uninitialize();
	m_encoder.uninitialize();

	return true;
}

bool CBoxAlgorithmWindowing::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmWindowing::process()
{
	IBoxIO* boxContext = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Process input data
	for (size_t i = 0; i < boxContext->getInputChunkCount(0); ++i)
	{
		const uint64_t startTime = boxContext->getInputChunkStartTime(0, i);
		const uint64_t endTime   = boxContext->getInputChunkEndTime(0, i);

		m_decoder.decode(i);
		IMatrix* matrix = m_decoder.getOutputMatrix();

		if (m_decoder.isHeaderReceived())
		{
			/*
			 * Depending on the Window method, we compute the coefficient vector
			 * To be applied on each channel.
			 */
			m_windowCoefs.resize(matrix->getDimensionSize(1));
			const size_t n = m_windowCoefs.size();

			if (m_windowMethod == Hamming)
			{
				for (size_t k = 0; k < n; ++k) { m_windowCoefs[k] = 0.54 - 0.46 * cos(2. * M_PI * double(k) / (double(n) - 1.)); }
			}
			else if (m_windowMethod == Hann || m_windowMethod == Hanning)
			{
				for (size_t k = 0; k < n; ++k) { m_windowCoefs[k] = 0.5 * (1. - cos(2. * M_PI * double(k) / (double(n) - 1.))); }
			}
			else if (m_windowMethod == Blackman)
			{
				for (size_t k = 0; k < n; ++k)
				{
					m_windowCoefs[k] = 0.42 - 0.5 * cos(2. * M_PI * double(k) / (double(n) - 1.)) + 0.08 * cos(4. * M_PI * double(k) / (double(n) - 1.));
				}
			}
			else if (m_windowMethod == Triangular)
			{
				/* from MATLAB implementation, as ITPP documentation seems to be flawed */
				for (size_t k = 1; k <= (n + 1) / 2; ++k)
				{
					if (n % 2 == 1) { m_windowCoefs[k - 1] = double((2. * double(k)) / (double(n) + 1.)); }
					else { m_windowCoefs[k - 1] = double((2. * double(k) - 1.) / double(n)); }
				}

				for (size_t k = n / 2 + 1; k <= n; ++k)
				{
					if (n % 2 == 1) { m_windowCoefs[k - 1] = double(2. - (2. * double(k)) / (double(n) + 1.)); }
					else { m_windowCoefs[k - 1] = double(2. - (2. * double(k) - 1.) / double(n)); }
				}
			}
			else if (m_windowMethod == SquareRoot)
			{
				for (size_t k = 1; k <= (n + 1) / 2; ++k)
				{
					if (n % 2 == 1) { m_windowCoefs[k - 1] = sqrt(2. * double(k) / (double(n) + 1.)); }
					else { m_windowCoefs[k - 1] = sqrt((2. * double(k) - 1.) / double(n)); }
				}

				for (size_t k = n / 2 + 1; k <= n; ++k)
				{
					if (n % 2 == 1) { m_windowCoefs[k - 1] = sqrt(2. - (2. * double(k)) / (double(n) + 1.)); }
					else { m_windowCoefs[k - 1] = sqrt(2. - (2. * double(k) - 1.) / double(n)); }
				}
			}
			else if (m_windowMethod == None) { for (size_t k = 0; k < n; ++k) { m_windowCoefs[k] = 1; } }
			else { OV_ERROR_KRF("The windows method chosen is not supported.\n", OpenViBE::Kernel::ErrorType::BadSetting); }

			m_encoder.encodeHeader();
		}

		if (m_decoder.isBufferReceived())
		{
			/* We filter each channel with the window function */
			for (size_t j = 0; j < matrix->getDimensionSize(0); ++j) // channels
			{
				for (size_t k = 0; k < matrix->getDimensionSize(1); ++k) // samples
				{
					matrix->getBuffer()[j * matrix->getDimensionSize(1) + k] *= m_windowCoefs[k];
				}
			}

			m_encoder.encodeBuffer();
		}

		if (m_decoder.isEndReceived()) { m_encoder.encodeEnd(); }

		boxContext->markOutputAsReadyToSend(0, startTime, endTime);
	}

	return true;
}
