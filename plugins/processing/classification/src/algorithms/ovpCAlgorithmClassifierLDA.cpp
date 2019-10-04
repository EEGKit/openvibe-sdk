#include "ovpCAlgorithmClassifierLDA.h"
#if defined TARGET_HAS_ThirdPartyEIGEN

#include <map>
#include <sstream>
#include <iostream>
#include <algorithm>

#include <system/ovCMemory.h>
#include <xml/IXMLHandler.h>

#include <Eigen/Eigenvalues>

#include "../algorithms/ovpCAlgorithmConditionedCovariance.h"

namespace
{
	const char* const TYPE_NODE_NAME    = "LDA";
	const char* const CLASSES_NODE_NAME = "Classes";
	//const char* const COEFFICIENTS_NODE_NAME = "Weights";
	//const char* const BIAS_DISTANCE_NODE_NAME = "Bias-distance";
	//const char* const COEFFICIENT_PROBABILITY_NODE_NAME = "Coefficient-probability";
	const char* const COMPUTATION_HELPERS_CONFIGURATION_NODE = "Class-config-list";
	const char* const LDA_CONFIG_FILE_VERSION_ATTRIBUTE_NAME = "version";
}

extern const char* const CLASSIFIER_ROOT;

int OpenViBEPlugins::Classification::LDAClassificationCompare(OpenViBE::IMatrix& firstClassificationValue, OpenViBE::IMatrix& secondClassificationValue)
{
	//We first need to find the best classification of each.
	double* valueBuffer   = firstClassificationValue.getBuffer();
	const double maxFirst = *(std::max_element(valueBuffer, valueBuffer + firstClassificationValue.getBufferElementCount()));

	valueBuffer            = secondClassificationValue.getBuffer();
	const double maxSecond = *(std::max_element(valueBuffer, valueBuffer + secondClassificationValue.getBufferElementCount()));

	//Then we just compared them
	if (OVFloatEqual(maxFirst, maxSecond)) { return 0; }
	if (maxFirst > maxSecond) { return -1; }
	return 1;
}

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins::Classification;

using namespace OpenViBEToolkit;

using namespace Eigen;

#define LDA_DEBUG 0
#if LDA_DEBUG
void CAlgorithmClassifierLDA::dumpMatrix(OpenViBE::Kernel::ILogManager &rMgr, const MatrixXdRowMajor &mat, const CString &desc)
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
void CAlgorithmClassifierLDA::dumpMatrix(ILogManager& /* rMgr */, const MatrixXdRowMajor& /*mat*/, const CString& /*desc*/) { }
#endif

bool CAlgorithmClassifierLDA::initialize()
{
	// Initialize the Conditioned Covariance Matrix algorithm
	m_covarianceAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ConditionedCovariance));

	OV_ERROR_UNLESS_KRF(m_covarianceAlgorithm->initialize(), "Failed to initialize covariance algorithm", OpenViBE::Kernel::ErrorType::Internal);

	// This is the weight parameter local to this module and automatically exposed to the GUI. Its redirected to the corresponding parameter of the cov alg.
	TParameterHandler<double> ip_f64Shrinkage(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage));
	ip_f64Shrinkage.setReferenceTarget(m_covarianceAlgorithm->getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage));

	TParameterHandler<bool> ip_bDiagonalCov(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov));
	ip_bDiagonalCov = false;

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	op_pConfiguration = nullptr;

	return CAlgorithmClassifier::initialize();
}

bool CAlgorithmClassifierLDA::uninitialize()
{
	OV_ERROR_UNLESS_KRF(m_covarianceAlgorithm->uninitialize(), "Failed to uninitialize covariance algorithm", OpenViBE::Kernel::ErrorType::Internal);

	this->getAlgorithmManager().releaseAlgorithm(*m_covarianceAlgorithm);

	return CAlgorithmClassifier::uninitialize();
}

bool CAlgorithmClassifierLDA::train(const IFeatureVectorSet& featureVectorSet)
{
	OV_ERROR_UNLESS_KRF(this->initializeExtraParameterMechanism(), "Failed to unitialize extra parameters", OpenViBE::Kernel::ErrorType::Internal);

	//We need to clear list because a instance of this class should support more that one training.
	m_labels.clear();
	m_discriminantFunctions.clear();

	const bool useShrinkage = this->getBooleanParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage);

	bool diagonalCov;
	if (useShrinkage)
	{
		this->getFloat64Parameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage);
		diagonalCov = this->getBooleanParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov);
	}
	else
	{
		//If we don't use shrinkage we need to set lambda to 0.
		TParameterHandler<double> ip_f64Shrinkage(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage));
		ip_f64Shrinkage = 0.0;

		TParameterHandler<bool> ip_bDiagonalCov(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov));
		ip_bDiagonalCov = false;
		diagonalCov     = false;
	}

	OV_ERROR_UNLESS_KRF(this->uninitializeExtraParameterMechanism(), "Failed to ininitialize extra parameters", OpenViBE::Kernel::ErrorType::Internal);

	// IO to the covariance alg
	TParameterHandler<IMatrix*> op_pMean(m_covarianceAlgorithm->getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean));
	TParameterHandler<IMatrix*> op_pCovarianceMatrix(m_covarianceAlgorithm->getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix));
	TParameterHandler<IMatrix*> ip_pFeatureVectorSet(m_covarianceAlgorithm->getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet));

	const uint32_t nRows = featureVectorSet.getFeatureVectorCount();
	const uint32_t nCols = (nRows > 0 ? featureVectorSet[0].getSize() : 0);
	this->getLogManager() << LogLevel_Debug << "Feature set input dims ["
			<< featureVectorSet.getFeatureVectorCount() << "x" << nCols << "]\n";

	OV_ERROR_UNLESS_KRF(nRows != 0 && nCols != 0, "Input data has a zero-size dimension, dims = [" << nRows << "x" << nCols << "]",
						OpenViBE::Kernel::ErrorType::BadInput);

	// The max amount of classes to be expected
	TParameterHandler<uint64_t> ip_pNumberOfClasses(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));
	m_nClasses = uint32_t(ip_pNumberOfClasses);

	// Count the classes actually present
	std::vector<uint32_t> l_vClassCounts;
	l_vClassCounts.resize(m_nClasses);

	for (uint32_t i = 0; i < featureVectorSet.getFeatureVectorCount(); i++)
	{
		uint32_t classIdx = uint32_t(featureVectorSet[i].getLabel());
		l_vClassCounts[classIdx]++;
	}

	// Get class labels
	for (uint32_t i = 0; i < m_nClasses; i++)
	{
		m_labels.push_back(i);
		m_discriminantFunctions.push_back(CAlgorithmLDADiscriminantFunction());
	}

	// Per-class means and a global covariance are used to form the LDA model
	MatrixXd* l_oPerClassMeans = new MatrixXd[m_nClasses];
	MatrixXd l_oGlobalCov      = MatrixXd::Zero(nCols, nCols);

	// We need the means per class
	for (uint32_t l_ui32classIdx = 0; l_ui32classIdx < m_nClasses; l_ui32classIdx++)
	{
		if (l_vClassCounts[l_ui32classIdx] > 0)
		{
			// const double l_f64Label = m_labels[l_ui32classIdx];
			const uint32_t l_ui32nExamplesInClass = l_vClassCounts[l_ui32classIdx];

			// Copy all the data of the class to a matrix
			CMatrix l_oClassData;
			l_oClassData.setDimensionCount(2);
			l_oClassData.setDimensionSize(0, l_ui32nExamplesInClass);
			l_oClassData.setDimensionSize(1, nCols);
			double* buffer = l_oClassData.getBuffer();
			for (uint32_t i = 0; i < nRows; i++)
			{
				if (featureVectorSet[i].getLabel() == l_ui32classIdx)
				{
					System::Memory::copy(buffer, featureVectorSet[i].getBuffer(), nCols * sizeof(double));
					buffer += nCols;
				}
			}

			// Get the mean out of it
			Map<MatrixXdRowMajor> l_oDataMapper(l_oClassData.getBuffer(), l_ui32nExamplesInClass, nCols);
			const MatrixXd l_oClassMean      = l_oDataMapper.colwise().mean().transpose();
			l_oPerClassMeans[l_ui32classIdx] = l_oClassMean;
		}
		else
		{
			MatrixXd l_oTmp;
			l_oTmp.resize(nCols, 1);
			l_oTmp.setZero();
			l_oPerClassMeans[l_ui32classIdx] = l_oTmp;
		}
	}

	// We need a global covariance, use the regularized cov algorithm
	{
		ip_pFeatureVectorSet->setDimensionCount(2);
		ip_pFeatureVectorSet->setDimensionSize(0, nRows);
		ip_pFeatureVectorSet->setDimensionSize(1, nCols);
		double* buffer = ip_pFeatureVectorSet->getBuffer();

		// Insert all data as the input of the cov algorithm
		for (uint32_t i = 0; i < nRows; i++)
		{
			System::Memory::copy(buffer, featureVectorSet[i].getBuffer(), nCols * sizeof(double));
			buffer += nCols;
		}

		// Compute cov
		if (!m_covarianceAlgorithm->process())
		{

			//Free memory before leaving
			delete[] l_oPerClassMeans;

			OV_ERROR_KRF("Global covariance computation failed", OpenViBE::Kernel::ErrorType::Internal);
		}

		// Get the results from the cov algorithm
		Map<MatrixXdRowMajor> l_oCovMapper(op_pCovarianceMatrix->getBuffer(), nCols, nCols);
		l_oGlobalCov = l_oCovMapper;
	}

	//dumpMatrix(this->getLogManager(), l_aMean[l_ui32classIdx], "Mean");
	//dumpMatrix(this->getLogManager(), l_oGlobalCov, "Shrinked cov");

	if (diagonalCov)
	{
		for (uint32_t i = 0; i < nCols; i++)
		{
			for (uint32_t j = i + 1; j < nCols; j++)
			{
				l_oGlobalCov(i, j) = 0.0;
				l_oGlobalCov(j, i) = 0.0;
			}
		}
	}

	// Get the pseudoinverse of the global cov using eigen decomposition for self-adjoint matrices
	const double l_f64Tolerance = 1e-10;
	SelfAdjointEigenSolver<MatrixXd> l_oEigenSolver;
	l_oEigenSolver.compute(l_oGlobalCov);
	VectorXd l_oEigenValues = l_oEigenSolver.eigenvalues();
	for (uint32_t i = 0; i < nCols; i++) { if (l_oEigenValues(i) >= l_f64Tolerance) { l_oEigenValues(i) = 1.0 / l_oEigenValues(i); } }
	const MatrixXd l_oGlobalCovInv = l_oEigenSolver.eigenvectors() * l_oEigenValues.asDiagonal() * l_oEigenSolver.eigenvectors().inverse();

	// const MatrixXd l_oGlobalCovInv = l_oGlobalCov.inverse();
	//We send the bias and the weight of each class to ComputationHelper
	for (size_t i = 0; i < getClassCount(); ++i)
	{
		const double l_f64ExamplesInClass = l_vClassCounts[i];
		if (l_f64ExamplesInClass > 0)
		{
			const uint32_t l_ui32TotalExamples = featureVectorSet.getFeatureVectorCount();

			// This formula e.g. in Hastie, Tibshirani & Friedman: "Elements...", 2nd ed., p. 109
			const VectorXd l_oWeight = (l_oGlobalCovInv * l_oPerClassMeans[i]);
			const MatrixXd l_oInter  = -0.5 * l_oPerClassMeans[i].transpose() * l_oGlobalCovInv * l_oPerClassMeans[i];
			const double l_f64Bias   = l_oInter(0, 0) + std::log(l_f64ExamplesInClass / l_ui32TotalExamples);

			this->getLogManager() << LogLevel_Debug << "Bias for " << static_cast<const uint64_t>(i) << " is " << l_f64Bias << ", from " << l_f64ExamplesInClass
					/ l_ui32TotalExamples
					<< ", " << l_f64ExamplesInClass << "/" << l_ui32TotalExamples << ", int=" << l_oInter(0, 0)
					<< "\n";
			// dumpMatrix(this->getLogManager(), l_oPerClassMeans[i], "Means");

			m_discriminantFunctions[i].setWeight(l_oWeight);
			m_discriminantFunctions[i].setBias(l_f64Bias);
		}
		else { this->getLogManager() << LogLevel_Debug << "Class " << static_cast<const uint64_t>(i) << " has no examples\n"; }
	}

	// Hack for classes with zero examples, give them valid models but such that will always lose
	size_t l_ui32NonZeroClassIdx = 0;
	for (size_t i = 0; i < getClassCount(); i++)
	{
		if (l_vClassCounts[i] > 0)
		{
			l_ui32NonZeroClassIdx = i;
			break;
		}
	}
	for (size_t i = 0; i < getClassCount(); i++)
	{
		if (l_vClassCounts[i] == 0)
		{
			m_discriminantFunctions[i].setWeight(m_discriminantFunctions[l_ui32NonZeroClassIdx].getWeight());
			m_discriminantFunctions[i].setBias(m_discriminantFunctions[l_ui32NonZeroClassIdx].getBias() - 1.0); // Will always lose to the orig
		}
	}

	m_nCols = nCols;

	// Debug output
	//dumpMatrix(this->getLogManager(), l_oGlobalCov, "Global cov");
	//dumpMatrix(this->getLogManager(), l_oEigenValues, "Eigenvalues");
	//dumpMatrix(this->getLogManager(), l_oEigenSolver.eigenvectors(), "Eigenvectors");
	//dumpMatrix(this->getLogManager(), l_oGlobalCovInv, "Global cov inverse");
	//dumpMatrix(this->getLogManager(), m_coefficients, "Hyperplane weights");

	delete[] l_oPerClassMeans;
	return true;
}

bool CAlgorithmClassifierLDA::classify(const IFeatureVector& featureVector, double& classId, IVector& distance, IVector& probability)
{
	OV_ERROR_UNLESS_KRF(!m_discriminantFunctions.empty(), "LDA discriminant function list is empty", OpenViBE::Kernel::ErrorType::BadConfig);

	OV_ERROR_UNLESS_KRF(featureVector.getSize() == m_discriminantFunctions[0].getWeightVectorSize(),
						"Classifier expected " << m_discriminantFunctions[0].getWeightVectorSize() << " features, got " << featureVector.getSize(),
						OpenViBE::Kernel::ErrorType::BadInput);

	const Map<VectorXd> featureVec(const_cast<double*>(featureVector.getBuffer()), featureVector.getSize());
	const VectorXd weights = featureVec;
	const uint32_t nClass  = getClassCount();

	double* valueArray       = new double[nClass];
	double* probabilityValue = new double[nClass];
	//We ask for all computation helper to give the corresponding class value
	for (size_t i = 0; i < nClass; ++i) { valueArray[i] = m_discriminantFunctions[i].getValue(weights); }

	//p(Ck | x) = exp(ak) / sum[j](exp (aj))
	// with aj = (Weight for class j).transpose() * x + (Bias for class j)

	//Exponential can lead to nan results, so we reduce the computation and instead compute
	// p(Ck | x) = 1 / sum[j](exp(aj - ak))

	//All ak are given by computation helper
	errno = 0;
	for (size_t i = 0; i < nClass; ++i)
	{
		double expSum = 0.;
		for (size_t j = 0; j < nClass; ++j) { expSum += exp(valueArray[j] - valueArray[i]); }
		probabilityValue[i] = 1 / expSum;
		// std::cout << "p " << i << " = " << l_pProbabilityValue[i] << ", v=" << l_pValueArray[i] << ", " << errno << "\n";
	}

	//Then we just find the highest probability and take it as a result
	const size_t classIdx = size_t(std::distance(valueArray, std::max_element(valueArray, valueArray + nClass)));

	distance.setSize(nClass);
	probability.setSize(nClass);

	for (size_t i = 0; i < nClass; ++i)
	{
		distance[i]    = valueArray[i];
		probability[i] = probabilityValue[i];
	}

	classId = m_labels[classIdx];

	delete[] valueArray;
	delete[] probabilityValue;

	return true;
}

uint32_t CAlgorithmClassifierLDA::getClassCount() { return m_nClasses; }

XML::IXMLNode* CAlgorithmClassifierLDA::saveConfiguration()
{
	XML::IXMLNode* l_pAlgorithmNode = XML::createNode(TYPE_NODE_NAME);
	l_pAlgorithmNode->addAttribute(LDA_CONFIG_FILE_VERSION_ATTRIBUTE_NAME, "1");

	// Write the classifier to an .xml
	std::stringstream l_sClasses;

	for (size_t i = 0; i < getClassCount(); ++i) { l_sClasses << m_labels[i] << " "; }

	//Only new version should be recorded so we don't need to test
	XML::IXMLNode* l_pHelpersConfiguration = XML::createNode(COMPUTATION_HELPERS_CONFIGURATION_NODE);
	for (size_t i = 0; i < m_discriminantFunctions.size(); ++i) { l_pHelpersConfiguration->addChild(m_discriminantFunctions[i].getConfiguration()); }

	XML::IXMLNode* l_pTempNode = XML::createNode(CLASSES_NODE_NAME);
	l_pTempNode->setPCData(l_sClasses.str().c_str());
	l_pAlgorithmNode->addChild(l_pTempNode);
	l_pAlgorithmNode->addChild(l_pHelpersConfiguration);

	return l_pAlgorithmNode;
}


//Extract a double from the PCDATA of a node
double getFloatFromNode(XML::IXMLNode* pNode)
{
	std::stringstream l_sData(pNode->getPCData());
	double res;
	l_sData >> res;

	return res;
}

bool CAlgorithmClassifierLDA::loadConfiguration(XML::IXMLNode* pConfigurationNode)
{
	OV_ERROR_UNLESS_KRF(pConfigurationNode->hasAttribute(LDA_CONFIG_FILE_VERSION_ATTRIBUTE_NAME),
						"Invalid model: model trained with an obsolete version of LDA",
						OpenViBE::Kernel::ErrorType::BadConfig);

	m_labels.clear();
	m_discriminantFunctions.clear();

	XML::IXMLNode* l_pTempNode = pConfigurationNode->getChildByName(CLASSES_NODE_NAME);

	OV_ERROR_UNLESS_KRF(l_pTempNode != nullptr, "Failed to retrieve xml node", OpenViBE::Kernel::ErrorType::BadParsing);

	loadClassesFromNode(l_pTempNode);


	//We send corresponding data to the computation helper
	XML::IXMLNode* l_pConfigsNode = pConfigurationNode->getChildByName(COMPUTATION_HELPERS_CONFIGURATION_NODE);

	for (size_t i = 0; i < l_pConfigsNode->getChildCount(); ++i)
	{
		m_discriminantFunctions.push_back(CAlgorithmLDADiscriminantFunction());
		m_discriminantFunctions[i].loadConfiguration(l_pConfigsNode->getChild(i));
	}

	return true;
}

void CAlgorithmClassifierLDA::loadClassesFromNode(XML::IXMLNode* pNode)
{
	std::stringstream l_sData(pNode->getPCData());
	double l_f64Temp;
	while (l_sData >> l_f64Temp) { m_labels.push_back(l_f64Temp); }
	m_nClasses = uint32_t(m_labels.size());
}

//Load the weight vector
void CAlgorithmClassifierLDA::loadCoefficientsFromNode(XML::IXMLNode* pNode)
{
	std::stringstream l_sData(pNode->getPCData());

	std::vector<double> l_vCoefficients;
	double l_f64Value;
	while (l_sData >> l_f64Value) { l_vCoefficients.push_back(l_f64Value); }

	m_weights.resize(1, l_vCoefficients.size());
	m_nCols = uint32_t(l_vCoefficients.size());
	for (size_t i = 0; i < l_vCoefficients.size(); i++) { m_weights(0, i) = l_vCoefficients[i]; }
}

#endif // TARGET_HAS_ThirdPartyEIGEN
