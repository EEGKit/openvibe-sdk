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
	m_WindowMethod = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	if (m_WindowMethod != OVP_TypeId_WindowMethod_None
		&& m_WindowMethod != OVP_TypeId_WindowMethod_Hamming
		&& m_WindowMethod != OVP_TypeId_WindowMethod_Hanning
		&& m_WindowMethod != OVP_TypeId_WindowMethod_Hann
		&& m_WindowMethod != OVP_TypeId_WindowMethod_Blackman
		&& m_WindowMethod != OVP_TypeId_WindowMethod_Triangular
		&& m_WindowMethod != OVP_TypeId_WindowMethod_SquareRoot)
	{
		OV_ERROR_KRF(
			"No valid windowing method set.\n",
			OpenViBE::Kernel::ErrorType::BadSetting);
	}

	m_Decoder.initialize(*this, 0);
	m_Encoder.initialize(*this, 0);
	m_Encoder.getInputMatrix().setReferenceTarget(m_Decoder.getOutputMatrix());
	m_Encoder.getInputSamplingRate().setReferenceTarget(m_Decoder.getOutputSamplingRate());

	return true;
}

bool CBoxAlgorithmWindowing::uninitialize()
{
	m_Decoder.uninitialize();
	m_Encoder.uninitialize();

	return true;
}

bool CBoxAlgorithmWindowing::processInput(uint32_t ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmWindowing::process()
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
			/*
			 * Depending on the Window method, we compute the coefficient vector
			 * To be applied on each channel.
			 */
			m_WindowCoefficients.resize(matrix->getDimensionSize(1));
			size_t n = m_WindowCoefficients.size();

			if (m_WindowMethod == OVP_TypeId_WindowMethod_Hamming)
			{
				for (size_t k = 0; k < n; k++)
				{
					m_WindowCoefficients[k] = 0.54 - 0.46 * cos(2. * M_PI * static_cast<double>(k) / (static_cast<double>(n) - 1.));
				}
			}
			else if (m_WindowMethod == OVP_TypeId_WindowMethod_Hann || m_WindowMethod == OVP_TypeId_WindowMethod_Hanning)
			{
				for (size_t k = 0; k < n; k++)
				{
					m_WindowCoefficients[k] = 0.5 * (1. - cos(2. * M_PI * static_cast<double>(k) / (static_cast<double>(n) - 1.)));
				}
			}
			else if (m_WindowMethod == OVP_TypeId_WindowMethod_Blackman)
			{
				for (size_t k = 0; k < n; k++)
				{
					m_WindowCoefficients[k] = 0.42 - 0.5 * cos(2. * M_PI * static_cast<double>(k) / (static_cast<double>(n) - 1.)) + 0.08 * cos(4. * M_PI * static_cast<double>(k) / (static_cast<double>(n) - 1.));
				}
			}
			else if (m_WindowMethod == OVP_TypeId_WindowMethod_Triangular)
			{
				/* from MATLAB implementation, as ITPP documentation seems to be flawed */
				for (size_t k = 1; k <= (n + 1) / 2; k++)
				{
					if (n % 2 == 1)
					{
						m_WindowCoefficients[k - 1] = static_cast<double>((2. * static_cast<double>(k)) / (static_cast<double>(n) + 1.));
					}
					else
					{
						m_WindowCoefficients[k - 1] = static_cast<double>((2. * static_cast<double>(k) - 1.) / static_cast<double>(n));
					}
				}

				for (size_t k = n / 2 + 1; k <= n; k++)
				{
					if (n % 2 == 1)
					{
						m_WindowCoefficients[k - 1] = static_cast<double>(2. - (2. * static_cast<double>(k)) / (static_cast<double>(n) + 1.));
					}
					else
					{
						m_WindowCoefficients[k - 1] = static_cast<double>(2. - (2. * static_cast<double>(k) - 1.) / static_cast<double>(n));
					}
				}
			}
			else if (m_WindowMethod == OVP_TypeId_WindowMethod_SquareRoot)
			{
				for (size_t k = 1; k <= (n + 1) / 2; k++)
				{
					if (n % 2 == 1)
					{
						m_WindowCoefficients[k - 1] = sqrt(2. * static_cast<double>(k) / (static_cast<double>(n) + 1.));
					}
					else
					{
						m_WindowCoefficients[k - 1] = sqrt((2. * static_cast<double>(k) - 1.) / static_cast<double>(n));
					}
				}

				for (size_t k = n / 2 + 1; k <= n; k++)
				{
					if (n % 2 == 1) { m_WindowCoefficients[k - 1] = sqrt(2. - (2. * static_cast<double>(k)) / (static_cast<double>(n) + 1.)); }
					else { m_WindowCoefficients[k - 1] = sqrt(2. - (2. * static_cast<double>(k) - 1.) / static_cast<double>(n)); }
				}
			}
			else if (m_WindowMethod == OVP_TypeId_WindowMethod_None)
			{
				for (size_t k = 0; k < n; k++) { m_WindowCoefficients[k] = 1; }
			}
			else { OV_ERROR_KRF("The windows method chosen is not supported.\n", OpenViBE::Kernel::ErrorType::BadSetting); }

			m_Encoder.encodeHeader();
		}

		if (m_Decoder.isBufferReceived())
		{
			/* We filter each channel with the window function */
			for (unsigned int j = 0; j < matrix->getDimensionSize(0); j++) // channels
			{
				for (unsigned int k = 0; k < matrix->getDimensionSize(1); k++) // samples
				{
					matrix->getBuffer()[j * matrix->getDimensionSize(1) + k] *= m_WindowCoefficients[k];
				}
			}

			m_Encoder.encodeBuffer();
		}

		if (m_Decoder.isEndReceived()) { m_Encoder.encodeEnd(); }

		dynamicBoxContext->markOutputAsReadyToSend(0, startTime, endTime);
	}

	return true;
}
