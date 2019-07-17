#include "ovpCAlgorithmMatrixAverage.h"

#include <system/ovCMemory.h>
#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

// ________________________________________________________________________________________________________________
//

boolean CAlgorithmMatrixAverage::initialize(void)
{
	ip_ui64AveragingMethod.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod));
	ip_ui64MatrixCount.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount));
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix));
	op_pAveragedMatrix.initialize(getOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix));

	m_CumulativeAverageSampleCount = 0;

	return true;
}

boolean CAlgorithmMatrixAverage::uninitialize(void)
{
	std::deque<OpenViBE::IMatrix*>::iterator it;
	for (it = m_vHistory.begin(); it != m_vHistory.end(); ++it)
	{
		delete *it;
	}
	m_vHistory.clear();

	op_pAveragedMatrix.uninitialize();
	ip_pMatrix.uninitialize();
	ip_ui64MatrixCount.uninitialize();
	ip_ui64AveragingMethod.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CAlgorithmMatrixAverage::process(void)
{
	IMatrix* l_pInputMatrix  = ip_pMatrix;
	IMatrix* l_pOutputMatrix = op_pAveragedMatrix;

	boolean l_bShouldPerformAverage = false;

	if (this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset))
	{
		std::deque<IMatrix*>::iterator it;

		for (it = m_vHistory.begin(); it != m_vHistory.end(); it++)
		{
			delete *it;
		}

		m_vHistory.clear();

		OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pOutputMatrix, *l_pInputMatrix);
	}

	if (this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix))
	{
		if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_MovingAverage.toUInteger())
		{
			IMatrix* l_pSwapMatrix = nullptr;

			if (m_vHistory.size() >= ip_ui64MatrixCount)
			{
				l_pSwapMatrix = m_vHistory.front();
				m_vHistory.pop_front();
			}
			else
			{
				l_pSwapMatrix = new CMatrix();
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_vHistory.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage = (m_vHistory.size() == ip_ui64MatrixCount);
		}
		else if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_MovingAverageImmediate.toUInteger())
		{
			IMatrix* l_pSwapMatrix = nullptr;

			if (m_vHistory.size() >= ip_ui64MatrixCount)
			{
				l_pSwapMatrix = m_vHistory.front();
				m_vHistory.pop_front();
			}
			else
			{
				l_pSwapMatrix = new CMatrix();
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_vHistory.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage = (m_vHistory.size() > 0);
		}
		else if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_BlockAverage.toUInteger())
		{
			IMatrix* l_pSwapMatrix = new CMatrix();

			if (m_vHistory.size() >= ip_ui64MatrixCount)
			{
				std::deque<IMatrix*>::iterator it;
				for (it = m_vHistory.begin(); it != m_vHistory.end(); ++it)
				{
					delete *it;
				}
				m_vHistory.clear();
			}

			OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_vHistory.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage = (m_vHistory.size() == ip_ui64MatrixCount);
		}
		else if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger())
		{
			m_vHistory.push_back(l_pInputMatrix);
			l_bShouldPerformAverage = true;
		}
		else
		{
			l_bShouldPerformAverage = false;
		}
	}

	if (l_bShouldPerformAverage)
	{
		OpenViBEToolkit::Tools::Matrix::clearContent(*l_pOutputMatrix);

		if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger())
		{
			OpenViBE::IMatrix* l_Matrix = m_vHistory.at(0);

			m_CumulativeAverageSampleCount++;

			if (m_CumulativeAverageSampleCount == 1) // If it's the first matrix, the average is the first matrix
			{
				float64* buffer         = l_Matrix->getBuffer();
				unsigned int bufferSize = l_Matrix->getBufferElementCount();

				m_CumulativeAverageMatrix.clear();
				m_CumulativeAverageMatrix.insert(m_CumulativeAverageMatrix.begin(), buffer, buffer + bufferSize);
			}
			else
			{
				if (l_Matrix->getBufferElementCount() != m_CumulativeAverageMatrix.size()) { return false; }

				const double n = static_cast<double>(m_CumulativeAverageSampleCount);

				OpenViBE::float64* l_pInputMatrixBuffer = l_Matrix->getBuffer();

				for (double& value : m_CumulativeAverageMatrix)
				{
					// calculate cumulative mean as
					// mean{k} = mean{k-1} + (new_value - mean{k-1}) / k
					// which is a recurrence equivalent to mean{k] = mean{k-1} * (k-1)/k + new_value/k
					// but more numerically stable
					value += (*l_pInputMatrixBuffer - value) / n;
					l_pInputMatrixBuffer++;
				}
			}

			float64* l_pOutputMatrixBuffer = l_pOutputMatrix->getBuffer();

			for (const double& value : m_CumulativeAverageMatrix)
			{
				*l_pOutputMatrixBuffer = static_cast<float64>(value);
				l_pOutputMatrixBuffer++;
			}

			m_vHistory.clear();
		}
		else
		{
			uint32 l_ui32Count = l_pOutputMatrix->getBufferElementCount();
			float64 l_f64Scale = 1. / m_vHistory.size();

			for (OpenViBE::IMatrix* matrix : m_vHistory)
			{
				float64* l_pOutputMatrixBuffer = l_pOutputMatrix->getBuffer();
				float64* l_pInputMatrixBuffer  = matrix->getBuffer();

				for (uint32 i = 0; i < l_ui32Count; i++)
				{
					*l_pOutputMatrixBuffer += *l_pInputMatrixBuffer * l_f64Scale;
					l_pOutputMatrixBuffer++;
					l_pInputMatrixBuffer++;
				}
			}
		}

		this->activateOutputTrigger(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed, true);
	}

	return true;
}
