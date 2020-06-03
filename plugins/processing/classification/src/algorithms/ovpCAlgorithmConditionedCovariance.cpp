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
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace Classification;

using namespace /*OpenViBE::*/Toolkit;

using namespace Eigen;

#define COV_DEBUG 0
#if COV_DEBUG
void CAlgorithmConditionedCovariance::dumpMatrix(Kernel::ILogManager &mgr, const MatrixXdRowMajor &mat, const CString &desc)
{
	mgr << LogLevel_Info << desc << "\n";
	for (int i = 0 ; i < mat.rows() ; i++)
	{
		mgr << LogLevel_Info << "Row " << i << ": ";
		for (int j = 0 ; j < mat.cols() ; j++) { mgr << mat(i,j) << " "; }
		mgr << "\n";
	}
}
#else
void CAlgorithmConditionedCovariance::dumpMatrix(ILogManager& /* mgr */, const MatrixXdRowMajor& /*mat*/, const CString& /*desc*/) { }
#endif

bool CAlgorithmConditionedCovariance::initialize()
{
	// Default value setting
	TParameterHandler<double> ip_shrinkage(getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage));
	ip_shrinkage = -1.0;

	return true;
}

bool CAlgorithmConditionedCovariance::process()
{
	// Set up the IO
	const TParameterHandler<double> ip_shrinkage(getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage));
	const TParameterHandler<IMatrix*> ip_sample(getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet));
	TParameterHandler<IMatrix*> op_mean(getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean));
	TParameterHandler<IMatrix*> op_covMatrix(getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix));
	double shrinkage = ip_shrinkage;

	OV_ERROR_UNLESS_KRF(shrinkage <= 1.0, "Invalid shrinkage value " << shrinkage << "(expected value <= 1.0)",
						Kernel::ErrorType::BadConfig);


	OV_ERROR_UNLESS_KRF(ip_sample->getDimensionCount() == 2,
						"Invalid dimension count for vector set " << ip_sample->getDimensionCount() << "(expected value = 2)",
						Kernel::ErrorType::BadInput);

	const size_t nRows = ip_sample->getDimensionSize(0);
	const size_t nCols = ip_sample->getDimensionSize(1);

	OV_ERROR_UNLESS_KRF(nRows >= 1 && nCols >= 1,
						"Invalid input matrix [" << nRows << "x" << nCols << "] (expected at least 1x1 size)",
						Kernel::ErrorType::BadInput);

	const double* buffer = ip_sample->getBuffer();


	OV_ERROR_UNLESS_KRF(buffer, "Invalid NULL feature set buffer", Kernel::ErrorType::BadInput);

	// Set the output buffers so we can write the results to them without copy
	op_mean->setDimensionCount(2);
	op_mean->setDimensionSize(0, 1);
	op_mean->setDimensionSize(1, nCols);
	op_covMatrix->setDimensionCount(2);
	op_covMatrix->setDimensionSize(0, nCols);
	op_covMatrix->setDimensionSize(1, nCols);

	// Insert our data into an Eigen matrix. As Eigen doesn't have const double* constructor, we cast away the const.
	const Map<MatrixXdRowMajor> dataMatrix(const_cast<double*>(buffer), nRows, nCols);

	// Estimate the data center and center the data
	Map<MatrixXdRowMajor> dataMean(op_mean->getBuffer(), 1, nCols);
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

		getLogManager() << LogLevel_Debug << "Phi " << phi << " Gamma " << gamma << " kappa " << kappa << "\n";
		getLogManager() << LogLevel_Debug << "Estimated shrinkage weight to be " << shrinkage << "\n";

		dumpMatrix(getLogManager(), phiMat, "PhiMat");
	}
	else { getLogManager() << LogLevel_Debug << "Using user-provided shrinkage weight " << shrinkage << "\n"; }

	// Use the output as a buffer to avoid copying
	Map<MatrixXdRowMajor> oCov(op_covMatrix->getBuffer(), nCols, nCols);

	// Mix the prior and the sample estimates according to the shrinkage parameter
	oCov = shrinkage * priorCov + (1.0 - shrinkage) * sampleCov;

	// Debug block
	dumpMatrix(getLogManager(), dataMean, "DataMean");
	dumpMatrix(getLogManager(), sampleCov, "Sample cov");
	dumpMatrix(getLogManager(), priorCov, "Prior cov");
	dumpMatrix(getLogManager(), oCov, "Output cov");

	return true;
}

#endif // TARGET_HAS_ThirdPartyEIGEN
