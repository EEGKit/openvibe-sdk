#include "ovpCAlgorithmConditionedCovariance.h"

/*
 * This implementation is based on the matlab code corresponding to
 *
 * Ledoit & Wolf: "A Well-Conditioned Estimator for Large-Dimensional Covariance Matrices", 2004.
 *
 */
#if defined TARGET_HAS_ThirdPartyEIGEN
#include <iostream>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

using namespace Eigen;

#define COV_DEBUG 0
#if COV_DEBUG
void CAlgorithmConditionedCovariance::dumpMatrix(OpenViBE::Kernel::ILogManager &rMgr, const MatrixXdRowMajor &mat, const CString &desc)
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
void CAlgorithmConditionedCovariance::dumpMatrix(ILogManager& /* rMgr */, const MatrixXdRowMajor& /*mat*/, const CString& /*desc*/) { }
#endif

bool CAlgorithmConditionedCovariance::initialize()
{
	// Default value setting
	TParameterHandler<double> ip_f64Shrinkage(getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage));
	ip_f64Shrinkage = -1.0;

	return true;
}

bool CAlgorithmConditionedCovariance::uninitialize() { return true; }

bool CAlgorithmConditionedCovariance::process()
{
	// Set up the IO
	const TParameterHandler<double> ip_f64Shrinkage(getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage));
	const TParameterHandler<IMatrix*> ip_pFeatureVectorSet(getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet));
	TParameterHandler<IMatrix*> op_pMean(getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean));
	TParameterHandler<IMatrix*> op_pCovarianceMatrix(getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix));
	double shrinkage = ip_f64Shrinkage;

	OV_ERROR_UNLESS_KRF(shrinkage <= 1.0, "Invalid shrinkage value " << shrinkage << "(expected value <= 1.0)",
						OpenViBE::Kernel::ErrorType::BadConfig);


	OV_ERROR_UNLESS_KRF(ip_pFeatureVectorSet->getDimensionCount() == 2,
						"Invalid dimension count for vector set " << ip_pFeatureVectorSet->getDimensionCount() << "(expected value = 2)",
						OpenViBE::Kernel::ErrorType::BadInput);

	const uint32_t nRows = ip_pFeatureVectorSet->getDimensionSize(0);
	const uint32_t nCols = ip_pFeatureVectorSet->getDimensionSize(1);

	OV_ERROR_UNLESS_KRF(nRows >= 1 && nCols >= 1,
						"Invalid input matrix [" << nRows << "x" << nCols << "] (expected at least 1x1 size)",
						OpenViBE::Kernel::ErrorType::BadInput);

	const double* buffer = ip_pFeatureVectorSet->getBuffer();


	OV_ERROR_UNLESS_KRF(buffer, "Invalid NULL feature set buffer", OpenViBE::Kernel::ErrorType::BadInput);

	// Set the output buffers so we can write the results to them without copy
	op_pMean->setDimensionCount(2);
	op_pMean->setDimensionSize(0, 1);
	op_pMean->setDimensionSize(1, nCols);
	op_pCovarianceMatrix->setDimensionCount(2);
	op_pCovarianceMatrix->setDimensionSize(0, nCols);
	op_pCovarianceMatrix->setDimensionSize(1, nCols);

	// Insert our data into an Eigen matrix. As Eigen doesn't have const double* constructor, we cast away the const.
	const Map<MatrixXdRowMajor> dataMatrix(const_cast<double*>(buffer), nRows, nCols);

	// Estimate the data center and center the data
	Map<MatrixXdRowMajor> dataMean(op_pMean->getBuffer(), 1, nCols);
	dataMean                            = dataMatrix.colwise().mean();
	const MatrixXdRowMajor dataCentered = dataMatrix.rowwise() - dataMean.row(0);

	// Compute the sample cov matrix
	const MatrixXd sampleCov = (dataCentered.transpose() * dataCentered) * (1 / double(nRows));

	// Compute the prior cov matrix
	MatrixXd priorCov = MatrixXd::Zero(nCols, nCols);
	priorCov.diagonal().setConstant(sampleCov.diagonal().mean());

	// Compute shrinkage coefficient if its not given
	if (shrinkage < 0)
	{
		const MatrixXd dataSquared = dataCentered.cwiseProduct(dataCentered);

		const MatrixXd phiMat = (dataSquared.transpose() * dataSquared) / double(nRows) - sampleCov.cwiseAbs2();

		const double phi   = phiMat.sum();
		const double gamma = (sampleCov - priorCov).squaredNorm();	// Frobenius norm
		const double kappa = phi / gamma;

		shrinkage = std::max<double>(0, std::min<double>(1, kappa / double(nRows)));

		this->getLogManager() << LogLevel_Debug << "Phi " << phi << " Gamma " << gamma << " kappa " << kappa << "\n";
		this->getLogManager() << LogLevel_Debug << "Estimated shrinkage weight to be " << shrinkage << "\n";

		dumpMatrix(this->getLogManager(), phiMat, "PhiMat");
	}
	else { this->getLogManager() << LogLevel_Debug << "Using user-provided shrinkage weight " << shrinkage << "\n"; }

	// Use the output as a buffer to avoid copying
	Map<MatrixXdRowMajor> l_oOutputCov(op_pCovarianceMatrix->getBuffer(), nCols, nCols);

	// Mix the prior and the sample estimates according to the shrinkage parameter
	l_oOutputCov = shrinkage * priorCov + (1.0 - shrinkage) * sampleCov;

	// Debug block
	dumpMatrix(this->getLogManager(), dataMean, "DataMean");
	dumpMatrix(this->getLogManager(), sampleCov, "Sample cov");
	dumpMatrix(this->getLogManager(), priorCov, "Prior cov");
	dumpMatrix(this->getLogManager(), l_oOutputCov, "Output cov");

	return true;
}

#endif // TARGET_HAS_ThirdPartyEIGEN
