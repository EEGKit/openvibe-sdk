#include "ovpCAlgorithmMatrixAverage.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;

// ________________________________________________________________________________________________________________
//

bool CAlgorithmMatrixAverage::initialize()
{
	ip_averagingMethod.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod));
	ip_matrixCount.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount));
	ip_matrix.initialize(getInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix));
	op_averagedMatrix.initialize(getOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix));

	m_nAverageSamples = 0;

	return true;
}

bool CAlgorithmMatrixAverage::uninitialize()
{
	m_history.clear();

	op_averagedMatrix.uninitialize();
	ip_matrix.uninitialize();
	ip_matrixCount.uninitialize();
	ip_averagingMethod.uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CAlgorithmMatrixAverage::process()
{
	CMatrix* iMatrix = ip_matrix;
	CMatrix* oMatrix = op_averagedMatrix;

	bool shouldPerformAverage = false;

	if (this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset))
	{
		m_history.clear();
		oMatrix->copyDescription(*iMatrix);
		oMatrix->reset();
	}

	if (this->isInputTriggerActive(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix))
	{
		if (ip_averagingMethod == uint64_t(EEpochAverageMethod::Moving))
		{
			if (m_history.size() >= ip_matrixCount) { m_history.pop_front(); }
			m_history.push_back(CMatrix(*iMatrix));	// call constructor when pushback
			shouldPerformAverage = (m_history.size() == ip_matrixCount);
		}
		else if (ip_averagingMethod == uint64_t(EEpochAverageMethod::MovingImmediate))
		{
			if (m_history.size() >= ip_matrixCount) { m_history.pop_front(); }
			m_history.push_back(CMatrix(*iMatrix));	// call constructor when pushback
			shouldPerformAverage = (!m_history.empty());
		}
		else if (ip_averagingMethod == uint64_t(EEpochAverageMethod::Block))
		{
			if (m_history.size() >= ip_matrixCount) { m_history.clear(); }
			m_history.push_back(CMatrix(*iMatrix));	// call constructor when pushback
			shouldPerformAverage = (m_history.size() == ip_matrixCount);
		}
		else if (ip_averagingMethod == uint64_t(EEpochAverageMethod::Cumulative))
		{
			m_history.push_back(CMatrix(*iMatrix));
			shouldPerformAverage = true;
		}
		else { shouldPerformAverage = false; }
	}

	if (shouldPerformAverage)
	{
		oMatrix->reset();

		if (ip_averagingMethod == uint64_t(EEpochAverageMethod::Cumulative))
		{
			CMatrix& matrix = m_history.at(0);

			m_nAverageSamples++;

			if (m_nAverageSamples == 1) // If it's the first matrix, the average is the first matrix
			{
				double* buffer    = matrix.getBuffer();
				const size_t size = matrix.getSize();

				m_averageMatrices.clear();
				m_averageMatrices.insert(m_averageMatrices.begin(), buffer, buffer + size);
			}
			else
			{
				if (matrix.getSize() != m_averageMatrices.size()) { return false; }

				const double n = double(m_nAverageSamples);

				double* iBuffer = matrix.getBuffer();

				for (double& value : m_averageMatrices)
				{
					// calculate cumulative mean as
					// mean{k} = mean{k-1} + (new_value - mean{k-1}) / k
					// which is a recurrence equivalent to mean{k] = mean{k-1} * (k-1)/k + new_value/k
					// but more numerically stable
					value += (*iBuffer - value) / n;
					iBuffer++;
				}
			}

			double* oBuffer = oMatrix->getBuffer();

			for (const double& value : m_averageMatrices)
			{
				*oBuffer = double(value);
				oBuffer++;
			}

			m_history.clear();
		}
		else
		{
			const size_t n     = oMatrix->getSize();
			const double scale = 1. / m_history.size();

			for (const CMatrix& matrix : m_history)
			{
				double* oBuffer       = oMatrix->getBuffer();
				const double* iBuffer = matrix.getBuffer();

				for (size_t i = 0; i < n; ++i)
				{
					*oBuffer += *iBuffer * scale;
					oBuffer++;
					iBuffer++;
				}
			}
		}

		this->activateOutputTrigger(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed, true);
	}

	return true;
}
