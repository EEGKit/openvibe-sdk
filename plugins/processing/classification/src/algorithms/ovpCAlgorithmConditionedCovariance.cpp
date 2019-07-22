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
	for(int i=0;i<mat.rows();i++) {
		rMgr << LogLevel_Info << "Row " << i << ": ";
		for(int j=0;j<mat.cols();j++) {
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
	double l_f64Shrinkage = ip_f64Shrinkage;

	OV_ERROR_UNLESS_KRF(
		l_f64Shrinkage <= 1.0,
		"Invalid shrinkage value " << l_f64Shrinkage << "(expected value <= 1.0)",
		OpenViBE::Kernel::ErrorType::BadConfig);


	OV_ERROR_UNLESS_KRF(
		ip_pFeatureVectorSet->getDimensionCount() == 2,
		"Invalid dimension count for vector set " << ip_pFeatureVectorSet->getDimensionCount() << "(expected value = 2)",
		OpenViBE::Kernel::ErrorType::BadInput);

	const uint32_t l_ui32nRows = ip_pFeatureVectorSet->getDimensionSize(0);
	const uint32_t l_ui32nCols = ip_pFeatureVectorSet->getDimensionSize(1);

	OV_ERROR_UNLESS_KRF(
		l_ui32nRows >= 1 && l_ui32nCols >= 1,
		"Invalid input matrix [" << l_ui32nRows << "x" << l_ui32nCols << "] (expected at least 1x1 size)",
		OpenViBE::Kernel::ErrorType::BadInput);

	const double* l_pBuffer = ip_pFeatureVectorSet->getBuffer();


	OV_ERROR_UNLESS_KRF(
		l_pBuffer,
		"Invalid NULL feature set buffer",
		OpenViBE::Kernel::ErrorType::BadInput);

	// Set the output buffers so we can write the results to them without copy
	op_pMean->setDimensionCount(2);
	op_pMean->setDimensionSize(0, 1);
	op_pMean->setDimensionSize(1, l_ui32nCols);
	op_pCovarianceMatrix->setDimensionCount(2);
	op_pCovarianceMatrix->setDimensionSize(0, l_ui32nCols);
	op_pCovarianceMatrix->setDimensionSize(1, l_ui32nCols);

	// Insert our data into an Eigen matrix. As Eigen doesn't have const double* constructor, we cast away the const.
	const Map<MatrixXdRowMajor> l_oDataMatrix(const_cast<double*>(l_pBuffer), l_ui32nRows, l_ui32nCols);

	// Estimate the data center and center the data
	Map<MatrixXdRowMajor> l_oDataMean(op_pMean->getBuffer(), 1, l_ui32nCols);
	l_oDataMean                            = l_oDataMatrix.colwise().mean();
	const MatrixXdRowMajor l_oDataCentered = l_oDataMatrix.rowwise() - l_oDataMean.row(0);

	// Compute the sample cov matrix
	const MatrixXd l_oSampleCov = (l_oDataCentered.transpose() * l_oDataCentered) * (1 / (double)l_ui32nRows);

	// Compute the prior cov matrix
	MatrixXd l_oPriorCov = MatrixXd::Zero(l_ui32nCols, l_ui32nCols);
	l_oPriorCov.diagonal().setConstant(l_oSampleCov.diagonal().mean());

	// Compute shrinkage coefficient if its not given
	if (l_f64Shrinkage < 0)
	{
		const MatrixXd l_oDataSquared = l_oDataCentered.cwiseProduct(l_oDataCentered);

		MatrixXd l_oPhiMat = (l_oDataSquared.transpose() * l_oDataSquared) / (double)l_ui32nRows - l_oSampleCov.cwiseAbs2();

		const double l_f64phi   = l_oPhiMat.sum();
		const double l_f64gamma = (l_oSampleCov - l_oPriorCov).squaredNorm();	// Frobenius norm
		const double l_f64kappa = l_f64phi / l_f64gamma;

		l_f64Shrinkage = std::max<double>(0, std::min<double>(1, l_f64kappa / (double)l_ui32nRows));

		this->getLogManager() << LogLevel_Debug << "Phi " << l_f64phi << " Gamma " << l_f64gamma << " kappa " << l_f64kappa << "\n";
		this->getLogManager() << LogLevel_Debug << "Estimated shrinkage weight to be " << l_f64Shrinkage << "\n";

		dumpMatrix(this->getLogManager(), l_oPhiMat, "PhiMat");
	}
	else
	{
		this->getLogManager() << LogLevel_Debug << "Using user-provided shrinkage weight " << l_f64Shrinkage << "\n";
	}

	// Use the output as a buffer to avoid copying
	Map<MatrixXdRowMajor> l_oOutputCov(op_pCovarianceMatrix->getBuffer(), l_ui32nCols, l_ui32nCols);

	// Mix the prior and the sample estimates according to the shrinkage parameter
	l_oOutputCov = l_f64Shrinkage * l_oPriorCov + (1.0 - l_f64Shrinkage) * l_oSampleCov;

	// Debug block
	dumpMatrix(this->getLogManager(), l_oDataMean, "DataMean");
	dumpMatrix(this->getLogManager(), l_oSampleCov, "Sample cov");
	dumpMatrix(this->getLogManager(), l_oPriorCov, "Prior cov");
	dumpMatrix(this->getLogManager(), l_oOutputCov, "Output cov");

	return true;
}

#endif // TARGET_HAS_ThirdPartyEIGEN
