#include "ovpCAlgorithmMatrixAverage.h"

#include <system/ovCMemory.h>
#include <cmath>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

// ________________________________________________________________________________________________________________
//

bool CAlgorithmMatrixAverage::initialize()
{
	ip_ui64AveragingMethod.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod));
	ip_ui64MatrixCount.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount));
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix));
	op_pAveragedMatrix.initialize(getOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix));

	m_nAverageSamples = 0;

	return true;
}

bool CAlgorithmMatrixAverage::uninitialize()
{
	for (auto it = m_history.begin(); it != m_history.end(); ++it) { delete *it; }
	m_history.clear();

	op_pAveragedMatrix.uninitialize();
	ip_pMatrix.uninitialize();
	ip_ui64MatrixCount.uninitialize();
	ip_ui64AveragingMethod.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CAlgorithmMatrixAverage::process()
{
	IMatrix* l_pInputMatrix  = ip_pMatrix;
	IMatrix* l_pOutputMatrix = op_pAveragedMatrix;

	bool l_bShouldPerformAverage = false;

	if (this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset))
	{
		for (auto it = m_history.begin(); it != m_history.end(); ++it) { delete*it; }

		m_history.clear();

		OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pOutputMatrix, *l_pInputMatrix);
	}

	if (this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix))
	{
		if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_MovingAverage.toUInteger())
		{
			IMatrix* l_pSwapMatrix = nullptr;

			if (m_history.size() >= ip_ui64MatrixCount)
			{
				l_pSwapMatrix = m_history.front();
				m_history.pop_front();
			}
			else
			{
				l_pSwapMatrix = new CMatrix();
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_history.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage = (m_history.size() == ip_ui64MatrixCount);
		}
		else if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_MovingAverageImmediate.toUInteger())
		{
			IMatrix* l_pSwapMatrix = nullptr;

			if (m_history.size() >= ip_ui64MatrixCount)
			{
				l_pSwapMatrix = m_history.front();
				m_history.pop_front();
			}
			else
			{
				l_pSwapMatrix = new CMatrix();
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_history.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage = (!m_history.empty());
		}
		else if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_BlockAverage.toUInteger())
		{
			IMatrix* l_pSwapMatrix = new CMatrix();

			if (m_history.size() >= ip_ui64MatrixCount)
			{
				for (auto it = m_history.begin(); it != m_history.end(); ++it) { delete *it; }
				m_history.clear();
			}

			OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *l_pInputMatrix);
			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *l_pInputMatrix);

			m_history.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage = (m_history.size() == ip_ui64MatrixCount);
		}
		else if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger())
		{
			m_history.push_back(l_pInputMatrix);
			l_bShouldPerformAverage = true;
		}
		else { l_bShouldPerformAverage = false; }
	}

	if (l_bShouldPerformAverage)
	{
		OpenViBEToolkit::Tools::Matrix::clearContent(*l_pOutputMatrix);

		if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger())
		{
			IMatrix* l_Matrix = m_history.at(0);

			m_nAverageSamples++;

			if (m_nAverageSamples == 1) // If it's the first matrix, the average is the first matrix
			{
				double* buffer          = l_Matrix->getBuffer();
				uint32_t bufferSize = l_Matrix->getBufferElementCount();

				m_averageMatrices.clear();
				m_averageMatrices.insert(m_averageMatrices.begin(), buffer, buffer + bufferSize);
			}
			else
			{
				if (l_Matrix->getBufferElementCount() != m_averageMatrices.size()) { return false; }

				const double n = double(m_nAverageSamples);

				double* l_pInputMatrixBuffer = l_Matrix->getBuffer();

				for (double& value : m_averageMatrices)
				{
					// calculate cumulative mean as
					// mean{k} = mean{k-1} + (new_value - mean{k-1}) / k
					// which is a recurrence equivalent to mean{k] = mean{k-1} * (k-1)/k + new_value/k
					// but more numerically stable
					value += (*l_pInputMatrixBuffer - value) / n;
					l_pInputMatrixBuffer++;
				}
			}

			double* l_pOutputMatrixBuffer = l_pOutputMatrix->getBuffer();

			for (const double& value : m_averageMatrices)
			{
				*l_pOutputMatrixBuffer = double(value);
				l_pOutputMatrixBuffer++;
			}

			m_history.clear();
		}
		else
		{
			uint32_t l_ui32Count = l_pOutputMatrix->getBufferElementCount();
			double l_f64Scale    = 1. / m_history.size();

			for (IMatrix* matrix : m_history)
			{
				double* l_pOutputMatrixBuffer = l_pOutputMatrix->getBuffer();
				double* l_pInputMatrixBuffer  = matrix->getBuffer();

				for (uint32_t i = 0; i < l_ui32Count; i++)
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
