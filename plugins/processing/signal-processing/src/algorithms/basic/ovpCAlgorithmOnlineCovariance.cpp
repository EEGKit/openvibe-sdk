#if defined TARGET_HAS_ThirdPartyEIGEN

#include "ovpCAlgorithmOnlineCovariance.h"

#include <iostream>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace OpenViBEToolkit;

using namespace Eigen;

#define COV_DEBUG 0
#if COV_DEBUG
void CAlgorithmOnlineCovariance::dumpMatrix(OpenViBE::Kernel::ILogManager &rMgr, const MatrixXdRowMajor &mat, const CString &desc)
{
	rMgr << LogLevel_Info << desc << "\n";
	for (int i=0;i<mat.rows();i++) {
		rMgr << LogLevel_Info << "Row " << i << ": ";
		for (int j=0;j<mat.cols();j++) {
			rMgr << mat(i,j) << " ";
		}
		rMgr << "\n";
	}
}
#else
void CAlgorithmOnlineCovariance::dumpMatrix(ILogManager& /* rMgr */, const MatrixXdRowMajor& /*mat*/, const CString& /*desc*/) { }
#endif

bool CAlgorithmOnlineCovariance::initialize()
{
	m_n = 0;

	return true;
}

bool CAlgorithmOnlineCovariance::uninitialize() { return true; }

bool CAlgorithmOnlineCovariance::process()
{
	// Note: The input parameters must have been set by the caller by now
	const TParameterHandler<double> ip_f64Shrinkage(getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_Shrinkage));
	const TParameterHandler<bool> ip_bTraceNormalization(getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_TraceNormalization));
	const TParameterHandler<uint64_t> ip_ui64UpdateMethod(getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_UpdateMethod));
	const TParameterHandler<IMatrix*> ip_pFeatureVectorSet(getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors));
	TParameterHandler<IMatrix*> op_pMean(getOutputParameter(OVP_Algorithm_OnlineCovariance_OutputParameterId_Mean));
	TParameterHandler<IMatrix*> op_pCovarianceMatrix(getOutputParameter(OVP_Algorithm_OnlineCovariance_OutputParameterId_CovarianceMatrix));

	if (isInputTriggerActive(OVP_Algorithm_OnlineCovariance_Process_Reset))
	{
		OV_ERROR_UNLESS_KRF(ip_f64Shrinkage >= 0.0 && ip_f64Shrinkage <= 1.0,
							"Invalid shrinkage parameter (expected value between 0 and 1)",
							OpenViBE::Kernel::ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(ip_pFeatureVectorSet->getDimensionCount() == 2,
							"Invalid feature vector with " << ip_pFeatureVectorSet->getDimensionCount() << " dimensions (expected dim = 2)",
							OpenViBE::Kernel::ErrorType::BadInput);

		const size_t nRows = ip_pFeatureVectorSet->getDimensionSize(0);
		const size_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

		OV_ERROR_UNLESS_KRF(nRows >= 1 && nCols >= 1,
							"Invalid input matrix [" << nRows << "x" << nCols << "(minimum expected = 1x1)",
							OpenViBE::Kernel::ErrorType::BadInput);

		this->getLogManager() << LogLevel_Debug << "Using shrinkage coeff " << ip_f64Shrinkage << " ...\n";
		this->getLogManager() << LogLevel_Debug << "Trace normalization is " << (ip_bTraceNormalization ? "[on]" : "[off]") << "\n";
		this->getLogManager() << LogLevel_Debug << "Using update method " << getTypeManager().getEnumerationEntryNameFromValue(
			OVP_TypeId_OnlineCovariance_UpdateMethod, ip_ui64UpdateMethod) << "\n";

		// Set the output buffers
		op_pMean->setDimensionCount(2);
		op_pMean->setDimensionSize(0, 1);
		op_pMean->setDimensionSize(1, nCols);
		op_pCovarianceMatrix->setDimensionCount(2);
		op_pCovarianceMatrix->setDimensionSize(0, nCols);
		op_pCovarianceMatrix->setDimensionSize(1, nCols);

		// These keep track of the non-normalized incremental estimates
		m_oIncrementalMean.resize(1, nCols);
		m_oIncrementalMean.setZero();
		m_oIncrementalCov.resize(nCols, nCols);
		m_oIncrementalCov.setZero();

		m_n = 0;
	}

	if (isInputTriggerActive(OVP_Algorithm_OnlineCovariance_Process_Update))
	{
		const size_t nRows = ip_pFeatureVectorSet->getDimensionSize(0);
		const size_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

		const double* buffer = ip_pFeatureVectorSet->getBuffer();

		OV_ERROR_UNLESS_KRF(buffer, "Input buffer is NULL", OpenViBE::Kernel::ErrorType::BadInput);

		// Cast our data into an Eigen matrix. As Eigen doesn't have const double* constructor, we cast away the const.
		const Map<MatrixXdRowMajor> sampleChunk(const_cast<double*>(buffer), nRows, nCols);

		// Update the mean & cov estimates

		if (ip_ui64UpdateMethod == OVP_TypeId_OnlineCovariance_UpdateMethod_ChunkAverage.toUInteger())
		{
			// 'Average of per-chunk covariance matrices'. This might not be a proper cov over
			// the dataset, but seems occasionally produce nicely smoothed results when used for CSP.

			const MatrixXd chunkMean     = sampleChunk.colwise().mean();
			const MatrixXd chunkCentered = sampleChunk.rowwise() - chunkMean.row(0);

			MatrixXd chunkCov = (1.0 / double(nRows)) * chunkCentered.transpose() * chunkCentered;

			if (ip_bTraceNormalization)
			{
				// This normalization can be seen e.g. Muller-Gerkin & al., 1999. Presumably the idea is to normalize the
				// scale of each chunk in order to compensate for possible signal power drift over time during the EEG recording,
				// making each chunks' covariance contribute similarly to the average regardless of
				// the current average power. Such a normalization could also be implemented in its own
				// box and not done here.

				chunkCov = chunkCov / chunkCov.trace();
			}

			m_oIncrementalMean += chunkMean;
			m_oIncrementalCov += chunkCov;

			m_n++;

			// dumpMatrix(this->getLogManager(), sampleChunk, "SampleChunk");
			// dumpMatrix(this->getLogManager(), sampleCenteredMean, "SampleCenteredMean");
		}
		else if (ip_ui64UpdateMethod == OVP_TypeId_OnlineCovariance_UpdateMethod_Incremental.toUInteger())
		{
			// Incremental sample-per-sample cov updating.
			// It should be implementing the Youngs & Cramer algorithm as described in
			// Chan, Golub, Leveq, "Updating formulae and a pairwise algorithm...", 1979

			size_t start = 0;
			if (m_n == 0)
			{
				m_oIncrementalMean = sampleChunk.row(0);
				start              = 1;
				m_n                = 1;
			}

			MatrixXd chunkContribution;
			chunkContribution.resizeLike(m_oIncrementalCov);
			chunkContribution.setZero();

			for (size_t i = start; i < nRows; ++i)
			{
				m_oIncrementalMean += sampleChunk.row(i);

				const MatrixXd diff      = (m_n + 1.0) * sampleChunk.row(i) - m_oIncrementalMean;
				const MatrixXd outerProd = diff.transpose() * diff;

				chunkContribution += 1.0 / (m_n * (m_n + 1.0)) * outerProd;

				m_n++;
			}

			if (ip_bTraceNormalization) { chunkContribution = chunkContribution / chunkContribution.trace(); }

			m_oIncrementalCov += chunkContribution;

			// dumpMatrix(this->getLogManager(), sampleChunk, "Sample");
		}
#if 0
		else if(method == 2)
		{
			// Increment sample counts
			const uint64_t countBefore = m_n;
			const uint64_t countChunk = nRows;
			const uint64_t countAfter = countBefore + countChunk;
			const MatrixXd sampleSum = sampleChunk.colwise().sum();

			// Center the chunk
			const MatrixXd sampleCentered = sampleChunk.rowwise() - sampleSum.row(0)*(1.0/(double)countChunk);

			const MatrixXd sampleCoMoment = (sampleCentered.transpose() * sampleCentered);

			m_oIncrementalCov = m_oIncrementalCov + sampleCoMoment;

			if(countBefore>0)
			{
				const MatrixXd meanDifference = (countChunk/(double)countBefore) * m_oIncrementalMean - sampleSum;
				const MatrixXd meanDiffOuterProduct =  meanDifference.transpose()*meanDifference;

				m_oIncrementalCov += meanDiffOuterProduct*countBefore/(countChunk*countAfter);
			}

			m_oIncrementalMean = m_oIncrementalMean + sampleSum;

			m_n = countAfter;
		}
		else
		{
			// Increment sample counts
			const uint64_t countBefore = m_n;
			const uint64_t countChunk = nRows;
			const uint64_t countAfter = countBefore + countChunk;

			// Insert our data into an Eigen matrix. As Eigen doesn't have const double* constructor, we cast away the const.
			const Map<MatrixXdRowMajor> dataMatrix(const_cast<double*>(buffer),nRows,nCols);

			// Estimate the current sample means
			const MatrixXdRowMajor sampleMean = dataMatrix.colwise().mean();

			// Center the current data with the previous(!) mean
			const MatrixXdRowMajor sampleCentered = dataMatrix.rowwise() - m_oIncrementalMean.row(0);

			// Estimate the current covariance
			const MatrixXd sampleCov = (sampleCentered.transpose() * sampleCentered) * (1.0/(double)nRows);

			// fixme: recheck the weights ...

			// Update the global mean and cov
			if(countBefore>0)
			{
				m_oIncrementalMean = ( m_oIncrementalMean*countBefore + sampleMean*nRows) / (double)countAfter;
				m_oIncrementalCov = ( m_oIncrementalCov*countBefore + sampleCov*(countBefore/(double)countAfter) ) / (double)countAfter;
			}
			else
			{
				m_oIncrementalMean = sampleMean;
				m_oIncrementalCov = sampleCov;
			}


			m_n = countAfter;
		}
#endif
		else { OV_ERROR_KRF("Unknown update method [" << CIdentifier(ip_ui64UpdateMethod).toString() << "]", OpenViBE::Kernel::ErrorType::BadSetting); }
	}

	// Give output with regularization (mix prior + cov)?
	if (isInputTriggerActive(OVP_Algorithm_OnlineCovariance_Process_GetCov))
	{
		const size_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

		OV_ERROR_UNLESS_KRF(m_n > 0, "No sample to compute covariance", OpenViBE::Kernel::ErrorType::BadConfig);

		// Converters to CMatrix
		Map<MatrixXdRowMajor> outputMean(op_pMean->getBuffer(), 1, nCols);
		Map<MatrixXdRowMajor> outputCov(op_pCovarianceMatrix->getBuffer(), nCols, nCols);

		// The shrinkage parameter pulls the covariance matrix towards diagonal covariance
		MatrixXd priorCov;
		priorCov.resizeLike(m_oIncrementalCov);
		priorCov.setIdentity();

		// Mix the prior and the sample estimates according to the shrinkage parameter. We scale by 1/n to normalize
		outputMean = m_oIncrementalMean / double(m_n);
		outputCov  = ip_f64Shrinkage * priorCov + (1.0 - ip_f64Shrinkage) * (m_oIncrementalCov / double(m_n));

		// Debug block
		dumpMatrix(this->getLogManager(), outputMean, "Data mean");
		dumpMatrix(this->getLogManager(), m_oIncrementalCov / double(m_n), "Data cov");
		dumpMatrix(this->getLogManager(), ip_f64Shrinkage * priorCov, "Prior cov");
		dumpMatrix(this->getLogManager(), outputCov, "Output cov");
	}

	// Give just the output with no shrinkage?
	if (isInputTriggerActive(OVP_Algorithm_OnlineCovariance_Process_GetCovRaw))
	{
		const size_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

		OV_ERROR_UNLESS_KRF(m_n > 0, "No sample to compute covariance", OpenViBE::Kernel::ErrorType::BadConfig);

		// Converters to CMatrix
		Map<MatrixXdRowMajor> outputMean(op_pMean->getBuffer(), 1, nCols);
		Map<MatrixXdRowMajor> outputCov(op_pCovarianceMatrix->getBuffer(), nCols, nCols);

		// We scale by 1/n to normalize
		outputMean = m_oIncrementalMean / double(m_n);
		outputCov  = m_oIncrementalCov / double(m_n);

		// Debug block
		dumpMatrix(this->getLogManager(), outputMean, "Data mean");
		dumpMatrix(this->getLogManager(), outputCov, "Data Cov");
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyEIGEN
