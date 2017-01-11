#define _USE_MATH_DEFINES
#include <cmath>

#include "ovpCBoxAlgorithmWindowing.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;
using namespace OpenViBEToolkit;

boolean CBoxAlgorithmWindowing::initialize()
{
	//reads the plugin settings
	m_ui64WindowMethod = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	m_oDecoder.initialize(*this, 0);
	m_oEncoder.initialize(*this, 0);
	m_oEncoder.getInputMatrix().setReferenceTarget(m_oDecoder.getOutputMatrix());
	m_oEncoder.getInputSamplingRate().setReferenceTarget(m_oDecoder.getOutputSamplingRate());

	return true;
}

boolean CBoxAlgorithmWindowing::uninitialize()
{
	m_oDecoder.uninitialize();
	m_oEncoder.uninitialize();

	return true;
}

boolean CBoxAlgorithmWindowing::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmWindowing::process()
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
			/*
			 * Depending on the Window method, we compute the coefficient vector
			 * To be applied on each channel.
			 */
			m_vWindowCoefficients.resize(l_pMatrix->getDimensionSize(1));
			uint32 n = m_vWindowCoefficients.size();

			if (m_ui64WindowMethod == OVP_TypeId_WindowMethod_Hamming)
			{
				for (uint32 k = 0; k < n; k++)
				{
					m_vWindowCoefficients[k] = 0.54 - 0.46 * cos(2. * M_PI * k / (n - 1.));
				}
			}
			else if (m_ui64WindowMethod == OVP_TypeId_WindowMethod_Hanning)
			{
				for (uint32 k = 0; k < n; k++)
				{
					m_vWindowCoefficients[k] = 0.5 * (1. - cos(2. * M_PI * (k + 1.) / (n + 1.)));
				}
			}
			else if (m_ui64WindowMethod == OVP_TypeId_WindowMethod_Hann)
			{
				for (uint32 k = 0; k < n; k++)
				{
					m_vWindowCoefficients[k] = 0.5 * (1. - cos(2. * M_PI * k / (n - 1.)));
				}
			}
			else if (m_ui64WindowMethod == OVP_TypeId_WindowMethod_Blackman)
			{
				for (uint32 k = 0; k < n; k++)
				{
					m_vWindowCoefficients[k] = 0.42 - 0.5 * cos(2. * M_PI * k / (n - 1.)) + 0.08 * cos(4. * M_PI * k / (n - 1.));
				}
			}
			else if (m_ui64WindowMethod == OVP_TypeId_WindowMethod_Triangular)
			{
				/* from MATLAB implementation, as ITPP documentation seems to be flawed */
				for (uint32 k = 1; k <= (n + 1) / 2; k++)
				{
					if (n % 2 == 1) 
					{
						m_vWindowCoefficients[k - 1] = static_cast<double>((2. * k) / (n + 1.)); 
					}
					else
					{ 
						m_vWindowCoefficients[k - 1] = static_cast<double>((2. * k - 1.) / n);
					}
				}

				for (uint32 k = n / 2 + 1; k <= n; k++)
				{
					if (n % 2 == 1) 
					{ 
						m_vWindowCoefficients[k - 1] = static_cast<double>(2. - (2. * k) / (n + 1.));
					}
					else {
						m_vWindowCoefficients[k - 1] = static_cast<double>(2. - (2. * k - 1.) / n);
					}
				}
			}
			else if (m_ui64WindowMethod == OVP_TypeId_WindowMethod_SquareRoot)
			{
				for (uint32 k = 1; k <= (n + 1) / 2; k++)
				{
					if (n % 2 == 1)
					{
						m_vWindowCoefficients[k - 1] = sqrt(static_cast<double>((2. * k) / (n + 1.)));
					}
					else
					{
						m_vWindowCoefficients[k - 1] = sqrt(static_cast<double>((2. * k - 1.) / n));
					}
				}

				for (uint32 k = n / 2 + 1; k <= n; k++)
				{
					if (n % 2 == 1)
					{
						m_vWindowCoefficients[k - 1] = sqrt(static_cast<double>(2. - (2. * k) / (n + 1.)));
					}
					else
					{
						m_vWindowCoefficients[k - 1] = sqrt(static_cast<double>(2. - (2. * k - 1.) / n));
					}
				}
			}
			else // identity
			{
				for (uint32 k = 0; k < n; k++)
				{
					m_vWindowCoefficients[k] = 1;
				}
			}

			m_oEncoder.encodeHeader();
		}

		if (m_oDecoder.isBufferReceived())
		{
			/* We filter each channel with the window function */
			for (uint32 j = 0; j < l_pMatrix->getDimensionSize(0); j++) // channels
			{
				for (uint32 k = 0; k < l_pMatrix->getDimensionSize(1); k++) // samples
				{
					l_pMatrix->getBuffer()[j * l_pMatrix->getDimensionSize(1) + k] *= m_vWindowCoefficients[k];
				}
			}

			m_oEncoder.encodeBuffer();
		}

		if (m_oDecoder.isEndReceived())
		{
			m_oEncoder.encodeEnd();
		}

		l_pDynamicBoxContext->markOutputAsReadyToSend(0, l_ui64StartTime, l_ui64EndTime);
	}

	return true;
}
