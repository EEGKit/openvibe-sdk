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
	for(int i=0;i<mat.rows();i++) {
		rMgr << LogLevel_Info << "Row " << i << ": ";
		for(int j=0;j<mat.cols();j++) {
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

		const uint32_t nRows = ip_pFeatureVectorSet->getDimensionSize(0);
		const uint32_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

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
		const uint32_t nRows = ip_pFeatureVectorSet->getDimensionSize(0);
		const uint32_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

		const double* buffer = ip_pFeatureVectorSet->getBuffer();

		OV_ERROR_UNLESS_KRF(buffer, "Input buffer is NULL", OpenViBE::Kernel::ErrorType::BadInput);

		// Cast our data into an Eigen matrix. As Eigen doesn't have const double* constructor, we cast away the const.
		const Map<MatrixXdRowMajor> l_oSampleChunk(const_cast<double*>(buffer), nRows, nCols);

		// Update the mean & cov estimates

		if (ip_ui64UpdateMethod == OVP_TypeId_OnlineCovariance_UpdateMethod_ChunkAverage.toUInteger())
		{
			// 'Average of per-chunk covariance matrices'. This might not be a proper cov over
			// the dataset, but seems occasionally produce nicely smoothed results when used for CSP.

			const MatrixXd l_oChunkMean     = l_oSampleChunk.colwise().mean();
			const MatrixXd l_oChunkCentered = l_oSampleChunk.rowwise() - l_oChunkMean.row(0);

			MatrixXd l_oChunkCov = (1.0 / double(nRows)) * l_oChunkCentered.transpose() * l_oChunkCentered;

			if (ip_bTraceNormalization)
			{
				// This normalization can be seen e.g. Muller-Gerkin & al., 1999. Presumably the idea is to normalize the
				// scale of each chunk in order to compensate for possible signal power drift over time during the EEG recording,
				// making each chunks' covariance contribute similarly to the average regardless of
				// the current average power. Such a normalization could also be implemented in its own
				// box and not done here.

				l_oChunkCov = l_oChunkCov / l_oChunkCov.trace();
			}

			m_oIncrementalMean += l_oChunkMean;
			m_oIncrementalCov += l_oChunkCov;

			m_n++;

			// dumpMatrix(this->getLogManager(), l_oSampleChunk, "SampleChunk");
			// dumpMatrix(this->getLogManager(), l_oSampleCenteredMean, "SampleCenteredMean");
		}
		else if (ip_ui64UpdateMethod == OVP_TypeId_OnlineCovariance_UpdateMethod_Incremental.toUInteger())
		{
			// Incremental sample-per-sample cov updating.
			// It should be implementing the Youngs & Cramer algorithm as described in
			// Chan, Golub, Leveq, "Updating formulae and a pairwise algorithm...", 1979

			uint32_t l_ui32Start = 0;
			if (m_n == 0)
			{
				m_oIncrementalMean = l_oSampleChunk.row(0);
				l_ui32Start        = 1;
				m_n        = 1;
			}

			MatrixXd l_oChunkContribution;
			l_oChunkContribution.resizeLike(m_oIncrementalCov);
			l_oChunkContribution.setZero();

			for (uint32_t i = l_ui32Start; i < nRows; i++)
			{
				m_oIncrementalMean += l_oSampleChunk.row(i);

				const MatrixXd l_oDiff      = (m_n + 1.0) * l_oSampleChunk.row(i) - m_oIncrementalMean;
				const MatrixXd l_oOuterProd = l_oDiff.transpose() * l_oDiff;

				l_oChunkContribution += 1.0 / (m_n * (m_n + 1.0)) * l_oOuterProd;

				m_n++;
			}

			if (ip_bTraceNormalization) { l_oChunkContribution = l_oChunkContribution / l_oChunkContribution.trace(); }

			m_oIncrementalCov += l_oChunkContribution;

			// dumpMatrix(this->getLogManager(), l_oSampleChunk, "Sample");
		}
#if 0
		else if(l_ui32Method == 2)
		{
			// Increment sample counts
			const uint64_t l_ui64CountBefore = m_n;
			const uint64_t l_ui64CountChunk = nRows;
			const uint64_t l_ui64CountAfter = l_ui64CountBefore + l_ui64CountChunk;
			const MatrixXd l_oSampleSum = l_oSampleChunk.colwise().sum();

			// Center the chunk
			const MatrixXd l_oSampleCentered = l_oSampleChunk.rowwise() - l_oSampleSum.row(0)*(1.0/(double)l_ui64CountChunk);

			const MatrixXd l_oSampleCoMoment = (l_oSampleCentered.transpose() * l_oSampleCentered);

			m_oIncrementalCov = m_oIncrementalCov + l_oSampleCoMoment;

			if(l_ui64CountBefore>0)
			{
				const MatrixXd l_oMeanDifference = (l_ui64CountChunk/(double)l_ui64CountBefore) * m_oIncrementalMean - l_oSampleSum;
				const MatrixXd l_oMeanDiffOuterProduct =  l_oMeanDifference.transpose()*l_oMeanDifference;

				m_oIncrementalCov += l_oMeanDiffOuterProduct*l_ui64CountBefore/(l_ui64CountChunk*l_ui64CountAfter);
			}

			m_oIncrementalMean = m_oIncrementalMean + l_oSampleSum;

			m_n = l_ui64CountAfter;
		}
		else
		{
			// Increment sample counts
			const uint64_t l_ui64CountBefore = m_n;
			const uint64_t l_ui64CountChunk = nRows;
			const uint64_t l_ui64CountAfter = l_ui64CountBefore + l_ui64CountChunk;

			// Insert our data into an Eigen matrix. As Eigen doesn't have const double* constructor, we cast away the const.
			const Map<MatrixXdRowMajor> l_oDataMatrix(const_cast<double*>(buffer),nRows,nCols);

			// Estimate the current sample means
			const MatrixXdRowMajor l_oSampleMean = l_oDataMatrix.colwise().mean();

			// Center the current data with the previous(!) mean
			const MatrixXdRowMajor l_oSampleCentered = l_oDataMatrix.rowwise() - m_oIncrementalMean.row(0);

			// Estimate the current covariance
			const MatrixXd l_oSampleCov = (l_oSampleCentered.transpose() * l_oSampleCentered) * (1.0/(double)nRows);

			// fixme: recheck the weights ...

			// Update the global mean and cov
			if(l_ui64CountBefore>0)
			{
				m_oIncrementalMean = ( m_oIncrementalMean*l_ui64CountBefore + l_oSampleMean*nRows) / (double)l_ui64CountAfter;
				m_oIncrementalCov = ( m_oIncrementalCov*l_ui64CountBefore + l_oSampleCov*(l_ui64CountBefore/(double)l_ui64CountAfter) ) / (double)l_ui64CountAfter;
			}
			else
			{
				m_oIncrementalMean = l_oSampleMean;
				m_oIncrementalCov = l_oSampleCov;
			}


			m_n = l_ui64CountAfter;
		}
#endif
		else { OV_ERROR_KRF("Unknown update method [" << CIdentifier(ip_ui64UpdateMethod).toString() << "]", OpenViBE::Kernel::ErrorType::BadSetting); }
	}

	// Give output with regularization (mix prior + cov)?
	if (isInputTriggerActive(OVP_Algorithm_OnlineCovariance_Process_GetCov))
	{
		const uint32_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

		OV_ERROR_UNLESS_KRF(m_n > 0, "No sample to compute covariance", OpenViBE::Kernel::ErrorType::BadConfig);

		// Converters to CMatrix
		Map<MatrixXdRowMajor> l_oOutputMean(op_pMean->getBuffer(), 1, nCols);
		Map<MatrixXdRowMajor> l_oOutputCov(op_pCovarianceMatrix->getBuffer(), nCols, nCols);

		// The shrinkage parameter pulls the covariance matrix towards diagonal covariance
		MatrixXd l_oPriorCov;
		l_oPriorCov.resizeLike(m_oIncrementalCov);
		l_oPriorCov.setIdentity();

		// Mix the prior and the sample estimates according to the shrinkage parameter. We scale by 1/n to normalize
		l_oOutputMean = m_oIncrementalMean / double(m_n);
		l_oOutputCov  = ip_f64Shrinkage * l_oPriorCov + (1.0 - ip_f64Shrinkage) * (m_oIncrementalCov / double(m_n));

		// Debug block
		dumpMatrix(this->getLogManager(), l_oOutputMean, "Data mean");
		dumpMatrix(this->getLogManager(), m_oIncrementalCov / double(m_n), "Data cov");
		dumpMatrix(this->getLogManager(), ip_f64Shrinkage * l_oPriorCov, "Prior cov");
		dumpMatrix(this->getLogManager(), l_oOutputCov, "Output cov");
	}

	// Give just the output with no shrinkage?
	if (isInputTriggerActive(OVP_Algorithm_OnlineCovariance_Process_GetCovRaw))
	{
		const uint32_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

		OV_ERROR_UNLESS_KRF(m_n > 0, "No sample to compute covariance", OpenViBE::Kernel::ErrorType::BadConfig);

		// Converters to CMatrix
		Map<MatrixXdRowMajor> l_oOutputMean(op_pMean->getBuffer(), 1, nCols);
		Map<MatrixXdRowMajor> l_oOutputCov(op_pCovarianceMatrix->getBuffer(), nCols, nCols);

		// We scale by 1/n to normalize
		l_oOutputMean = m_oIncrementalMean / double(m_n);
		l_oOutputCov  = m_oIncrementalCov / double(m_n);

		// Debug block
		dumpMatrix(this->getLogManager(), l_oOutputMean, "Data mean");
		dumpMatrix(this->getLogManager(), l_oOutputCov, "Data Cov");
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyEIGEN
