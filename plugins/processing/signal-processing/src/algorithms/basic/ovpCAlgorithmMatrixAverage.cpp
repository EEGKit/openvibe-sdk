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
	IMatrix* iMatrix  = ip_pMatrix;
	IMatrix* oMatrix = op_pAveragedMatrix;

	bool l_bShouldPerformAverage = false;

	if (this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset))
	{
		for (auto it = m_history.begin(); it != m_history.end(); ++it) { delete*it; }

		m_history.clear();

		OpenViBEToolkit::Tools::Matrix::copyDescription(*oMatrix, *iMatrix);
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
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *iMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *iMatrix);

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
				OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *iMatrix);
			}

			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *iMatrix);

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

			OpenViBEToolkit::Tools::Matrix::copyDescription(*l_pSwapMatrix, *iMatrix);
			OpenViBEToolkit::Tools::Matrix::copyContent(*l_pSwapMatrix, *iMatrix);

			m_history.push_back(l_pSwapMatrix);
			l_bShouldPerformAverage = (m_history.size() == ip_ui64MatrixCount);
		}
		else if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger())
		{
			m_history.push_back(iMatrix);
			l_bShouldPerformAverage = true;
		}
		else { l_bShouldPerformAverage = false; }
	}

	if (l_bShouldPerformAverage)
	{
		OpenViBEToolkit::Tools::Matrix::clearContent(*oMatrix);

		if (ip_ui64AveragingMethod == OVP_TypeId_EpochAverageMethod_CumulativeAverage.toUInteger())
		{
			IMatrix* matrix = m_history.at(0);

			m_nAverageSamples++;

			if (m_nAverageSamples == 1) // If it's the first matrix, the average is the first matrix
			{
				double* buffer          = matrix->getBuffer();
				const uint32_t size = matrix->getBufferElementCount();

				m_averageMatrices.clear();
				m_averageMatrices.insert(m_averageMatrices.begin(), buffer, buffer + size);
			}
			else
			{
				if (matrix->getBufferElementCount() != m_averageMatrices.size()) { return false; }

				const double n = double(m_nAverageSamples);

				double* iMatrixBuffer = matrix->getBuffer();

				for (double& value : m_averageMatrices)
				{
					// calculate cumulative mean as
					// mean{k} = mean{k-1} + (new_value - mean{k-1}) / k
					// which is a recurrence equivalent to mean{k] = mean{k-1} * (k-1)/k + new_value/k
					// but more numerically stable
					value += (*iMatrixBuffer - value) / n;
					iMatrixBuffer++;
				}
			}

			double* oMatrixBuffer = oMatrix->getBuffer();

			for (const double& value : m_averageMatrices)
			{
				*oMatrixBuffer = double(value);
				oMatrixBuffer++;
			}

			m_history.clear();
		}
		else
		{
			const uint32_t n = oMatrix->getBufferElementCount();
			const double scale    = 1. / m_history.size();

			for (IMatrix* matrix : m_history)
			{
				double* oMatrixBuffer = oMatrix->getBuffer();
				double* iMatrixBuffer  = matrix->getBuffer();

				for (uint32_t i = 0; i < n; i++)
				{
					*oMatrixBuffer += *iMatrixBuffer * scale;
					oMatrixBuffer++;
					iMatrixBuffer++;
				}
			}
		}

		this->activateOutputTrigger(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed, true);
	}

	return true;
}
