#include "ovpCAlgorithmClassifierLDA.h"

#if defined TARGET_HAS_ThirdPartyEIGEN

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

int OpenViBEPlugins::Classification::LDAClassificationCompare(OpenViBE::IMatrix& first, OpenViBE::IMatrix& second)
{
	//We first need to find the best classification of each.
	double* buffer        = first.getBuffer();
	const double maxFirst = *(std::max_element(buffer, buffer + first.getBufferElementCount()));

	buffer                 = second.getBuffer();
	const double maxSecond = *(std::max_element(buffer, buffer + second.getBufferElementCount()));

	//Then we just compared them
	if (OVFloatEqual(maxFirst, maxSecond)) { return 0; }
	if (maxFirst > maxSecond) { return -1; }
	return 1;
}

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins::Classification;
using namespace OpenViBEToolkit;

using namespace Eigen;

#define LDA_DEBUG 0
#if LDA_DEBUG
void CAlgorithmClassifierLDA::dumpMatrix(OpenViBE::Kernel::ILogManager &rMgr, const MatrixXdRowMajor &mat, const CString &desc)
{
	rMgr << LogLevel_Info << desc << "\n";
	for (int i = 0 ; i < mat.rows() ; i++) 
	{
		rMgr << LogLevel_Info << "Row " << i << ": ";
		for (int j = 0 ; j < mat.cols() ; j++) { rMgr << mat(i,j) << " "; }
		rMgr << "\n";
	}
}
#else
void CAlgorithmClassifierLDA::dumpMatrix(ILogManager& /* rMgr */, const MatrixXdRowMajor& /*mat*/, const CString& /*desc*/) { }
#endif

bool CAlgorithmClassifierLDA::initialize()
{
	// Initialize the Conditioned Covariance Matrix algorithm
	m_covAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ConditionedCovariance));

	OV_ERROR_UNLESS_KRF(m_covAlgorithm->initialize(), "Failed to initialize covariance algorithm", OpenViBE::Kernel::ErrorType::Internal);

	// This is the weight parameter local to this module and automatically exposed to the GUI. Its redirected to the corresponding parameter of the cov alg.
	TParameterHandler<double> ip_shrinkage(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage));
	ip_shrinkage.setReferenceTarget(m_covAlgorithm->getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage));

	TParameterHandler<bool> ip_diagonalCov(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov));
	ip_diagonalCov = false;

	TParameterHandler<XML::IXMLNode*> op_configuration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Config));
	op_configuration = nullptr;

	return CAlgorithmClassifier::initialize();
}

bool CAlgorithmClassifierLDA::uninitialize()
{
	OV_ERROR_UNLESS_KRF(m_covAlgorithm->uninitialize(), "Failed to uninitialize covariance algorithm", OpenViBE::Kernel::ErrorType::Internal);

	this->getAlgorithmManager().releaseAlgorithm(*m_covAlgorithm);

	return CAlgorithmClassifier::uninitialize();
}

bool CAlgorithmClassifierLDA::train(const IFeatureVectorSet& dataset)
{
	OV_ERROR_UNLESS_KRF(this->initializeExtraParameterMechanism(), "Failed to unitialize extra parameters", OpenViBE::Kernel::ErrorType::Internal);

	//We need to clear list because a instance of this class should support more that one training.
	m_labels.clear();
	m_discriminantFunctions.clear();

	const bool useShrinkage = this->getBooleanParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage);

	bool diagonalCov;
	if (useShrinkage)
	{
		this->getDoubleParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage);
		diagonalCov = this->getBooleanParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov);
	}
	else
	{
		//If we don't use shrinkage we need to set lambda to 0.
		TParameterHandler<double> ip_shrinkage(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage));
		ip_shrinkage = 0.0;

		TParameterHandler<bool> ip_diagonalCov(this->getInputParameter(OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov));
		ip_diagonalCov = false;
		diagonalCov     = false;
	}

	OV_ERROR_UNLESS_KRF(this->uninitializeExtraParameterMechanism(), "Failed to ininitialize extra parameters", OpenViBE::Kernel::ErrorType::Internal);

	// IO to the covariance alg
	TParameterHandler<IMatrix*> op_mean(m_covAlgorithm->getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean));
	TParameterHandler<IMatrix*> op_covMatrix(m_covAlgorithm->getOutputParameter(OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix));
	TParameterHandler<IMatrix*> ip_dataset(m_covAlgorithm->getInputParameter(OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet));

	const size_t nRows = dataset.getFeatureVectorCount();
	const size_t nCols = (nRows > 0 ? dataset[0].getSize() : 0);
	this->getLogManager() << LogLevel_Debug << "Feature set input dims [" << dataset.getFeatureVectorCount() << "x" << nCols << "]\n";

	OV_ERROR_UNLESS_KRF(nRows != 0 && nCols != 0, "Input data has a zero-size dimension, dims = [" << nRows << "x" << nCols << "]",
						OpenViBE::Kernel::ErrorType::BadInput);

	// The max amount of classes to be expected
	TParameterHandler<uint64_t> ip_pNClasses(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NClasses));
	m_nClasses = size_t(ip_pNClasses);

	// Count the classes actually present
	std::vector<size_t> nClasses;
	nClasses.resize(m_nClasses);

	for (size_t i = 0; i < dataset.getFeatureVectorCount(); ++i)
	{
		size_t classIdx = size_t(dataset[i].getLabel());
		nClasses[classIdx]++;
	}

	// Get class labels
	for (size_t i = 0; i < m_nClasses; ++i)
	{
		m_labels.push_back(i);
		m_discriminantFunctions.push_back(CAlgorithmLDADiscriminantFunction());
	}

	// Per-class means and a global covariance are used to form the LDA model
	std::vector<MatrixXd> classMeans(m_nClasses);
	MatrixXd globalCov   = MatrixXd::Zero(nCols, nCols);

	// We need the means per class
	for (size_t classIdx = 0; classIdx < m_nClasses; classIdx++)
	{
		if (nClasses[classIdx] > 0)
		{
			// const double label = m_labels[l_classIdx];
			const size_t examplesInClass = nClasses[classIdx];

			// Copy all the data of the class to a matrix
			CMatrix classData;
			classData.setDimensionCount(2);
			classData.setDimensionSize(0, examplesInClass);
			classData.setDimensionSize(1, nCols);
			double* buffer = classData.getBuffer();
			for (size_t i = 0; i < nRows; ++i)
			{
				if (dataset[i].getLabel() == classIdx)
				{
					System::Memory::copy(buffer, dataset[i].getBuffer(), nCols * sizeof(double));
					buffer += nCols;
				}
			}

			// Get the mean out of it
			Map<MatrixXdRowMajor> dataMapper(classData.getBuffer(), examplesInClass, nCols);
			const MatrixXd classMean = dataMapper.colwise().mean().transpose();
			classMeans[classIdx]     = classMean;
		}
		else
		{
			MatrixXd tmp;
			tmp.resize(nCols, 1);
			tmp.setZero();
			classMeans[classIdx] = tmp;
		}
	}

	// We need a global covariance, use the regularized cov algorithm
	{
		ip_dataset->setDimensionCount(2);
		ip_dataset->setDimensionSize(0, nRows);
		ip_dataset->setDimensionSize(1, nCols);
		double* buffer = ip_dataset->getBuffer();

		// Insert all data as the input of the cov algorithm
		for (size_t i = 0; i < nRows; ++i)
		{
			System::Memory::copy(buffer, dataset[i].getBuffer(), nCols * sizeof(double));
			buffer += nCols;
		}

		// Compute cov
		if (!m_covAlgorithm->process()) { OV_ERROR_KRF("Global covariance computation failed", OpenViBE::Kernel::ErrorType::Internal); }

		// Get the results from the cov algorithm
		Map<MatrixXdRowMajor> covMapper(op_covMatrix->getBuffer(), nCols, nCols);
		globalCov = covMapper;
	}

	//dumpMatrix(this->getLogManager(), mean[l_classIdx], "Mean");
	//dumpMatrix(this->getLogManager(), globalCov, "Shrinked cov");

	if (diagonalCov)
	{
		for (size_t i = 0; i < nCols; ++i)
		{
			for (size_t j = i + 1; j < nCols; ++j)
			{
				globalCov(i, j) = 0.0;
				globalCov(j, i) = 0.0;
			}
		}
	}

	// Get the pseudoinverse of the global cov using eigen decomposition for self-adjoint matrices
	const double tolerance = 1e-10;
	SelfAdjointEigenSolver<MatrixXd> solver;
	solver.compute(globalCov);
	VectorXd eigenValues = solver.eigenvalues();
	for (size_t i = 0; i < nCols; ++i) { if (eigenValues(i) >= tolerance) { eigenValues(i) = 1.0 / eigenValues(i); } }
	const MatrixXd globalCovInv = solver.eigenvectors() * eigenValues.asDiagonal() * solver.eigenvectors().inverse();

	// const MatrixXd globalCovInv = globalCov.inverse();
	//We send the bias and the weight of each class to ComputationHelper
	for (size_t i = 0; i < getClassCount(); ++i)
	{
		const double examplesInClass = nClasses[i];
		if (examplesInClass > 0)
		{
			const size_t totalExamples = dataset.getFeatureVectorCount();

			// This formula e.g. in Hastie, Tibshirani & Friedman: "Elements...", 2nd ed., p. 109
			const VectorXd weigth = (globalCovInv * classMeans[i]);
			const MatrixXd inter  = -0.5 * classMeans[i].transpose() * globalCovInv * classMeans[i];
			const double bias     = inter(0, 0) + std::log(examplesInClass / totalExamples);

			this->getLogManager() << LogLevel_Debug << "Bias for " << i << " is " << bias << ", from " << examplesInClass / totalExamples
					<< ", " << examplesInClass << "/" << totalExamples << ", int=" << inter(0, 0) << "\n";
			// dumpMatrix(this->getLogManager(), perClassMeans[i], "Means");

			m_discriminantFunctions[i].setWeight(weigth);
			m_discriminantFunctions[i].setBias(bias);
		}
		else { this->getLogManager() << LogLevel_Debug << "Class " << static_cast<const uint64_t>(i) << " has no examples\n"; }
	}

	// Hack for classes with zero examples, give them valid models but such that will always lose
	size_t nonZeroClassIdx = 0;
	for (size_t i = 0; i < getClassCount(); ++i)
	{
		if (nClasses[i] > 0)
		{
			nonZeroClassIdx = i;
			break;
		}
	}
	for (size_t i = 0; i < getClassCount(); ++i)
	{
		if (nClasses[i] == 0)
		{
			m_discriminantFunctions[i].setWeight(m_discriminantFunctions[nonZeroClassIdx].getWeight());
			m_discriminantFunctions[i].setBias(m_discriminantFunctions[nonZeroClassIdx].getBias() - 1.0); // Will always lose to the orig
		}
	}

	m_nCols = nCols;

	// Debug output
	//dumpMatrix(this->getLogManager(), globalCov, "Global cov");
	//dumpMatrix(this->getLogManager(), eigenValues, "Eigenvalues");
	//dumpMatrix(this->getLogManager(), eigenSolver.eigenvectors(), "Eigenvectors");
	//dumpMatrix(this->getLogManager(), globalCovInv, "Global cov inverse");
	//dumpMatrix(this->getLogManager(), m_coefficients, "Hyperplane weights");

	return true;
}

bool CAlgorithmClassifierLDA::classify(const IFeatureVector& sample, double& classId, IVector& distance, IVector& probability)
{
	OV_ERROR_UNLESS_KRF(!m_discriminantFunctions.empty(), "LDA discriminant function list is empty", OpenViBE::Kernel::ErrorType::BadConfig);

	OV_ERROR_UNLESS_KRF(sample.getSize() == m_discriminantFunctions[0].getNWeight(),
						"Classifier expected " << m_discriminantFunctions[0].getNWeight() << " features, got " << sample.getSize(),
						OpenViBE::Kernel::ErrorType::BadInput);

	const Map<VectorXd> featureVec(const_cast<double*>(sample.getBuffer()), sample.getSize());
	const VectorXd weights = featureVec;
	const size_t nClass    = getClassCount();

	std::vector<double> buffer(nClass);
	std::vector<double> probabBuffer(nClass);
	//We ask for all computation helper to give the corresponding class value
	for (size_t i = 0; i < nClass; ++i) { buffer[i] = m_discriminantFunctions[i].getValue(weights); }

	//p(Ck | x) = exp(ak) / sum[j](exp (aj))
	// with aj = (Weight for class j).transpose() * x + (Bias for class j)

	//Exponential can lead to nan results, so we reduce the computation and instead compute
	// p(Ck | x) = 1 / sum[j](exp(aj - ak))

	//All ak are given by computation helper
	errno = 0;
	for (size_t i = 0; i < nClass; ++i)
	{
		double expSum = 0.;
		for (size_t j = 0; j < nClass; ++j) { expSum += exp(buffer[j] - buffer[i]); }
		probabBuffer[i] = 1 / expSum;
		// std::cout << "p " << i << " = " << probabilityValue[i] << ", v=" << valueArray[i] << ", " << errno << "\n";
	}

	//Then we just find the highest probability and take it as a result
	const size_t classIdx = size_t(std::distance(buffer.begin(), std::max_element(buffer.begin(), buffer.end())));

	distance.setSize(nClass);
	probability.setSize(nClass);

	for (size_t i = 0; i < nClass; ++i)
	{
		distance[i]    = buffer[i];
		probability[i] = probabBuffer[i];
	}

	classId = m_labels[classIdx];

	return true;
}

XML::IXMLNode* CAlgorithmClassifierLDA::saveConfig()
{
	XML::IXMLNode* algorithmNode = XML::createNode(TYPE_NODE_NAME);
	algorithmNode->addAttribute(LDA_CONFIG_FILE_VERSION_ATTRIBUTE_NAME, "1");

	// Write the classifier to an .xml
	std::stringstream classes;

	for (size_t i = 0; i < getClassCount(); ++i) { classes << m_labels[i] << " "; }

	//Only new version should be recorded so we don't need to test
	XML::IXMLNode* helpersConfig = XML::createNode(COMPUTATION_HELPERS_CONFIGURATION_NODE);
	for (size_t i = 0; i < m_discriminantFunctions.size(); ++i) { helpersConfig->addChild(m_discriminantFunctions[i].getConfiguration()); }

	XML::IXMLNode* tmpNode = XML::createNode(CLASSES_NODE_NAME);
	tmpNode->setPCData(classes.str().c_str());
	algorithmNode->addChild(tmpNode);
	algorithmNode->addChild(helpersConfig);

	return algorithmNode;
}


//Extract a double from the PCDATA of a node
double getFloatFromNode(XML::IXMLNode* pNode)
{
	std::stringstream ss(pNode->getPCData());
	double res;
	ss >> res;

	return res;
}

bool CAlgorithmClassifierLDA::loadConfig(XML::IXMLNode* configNode)
{
	OV_ERROR_UNLESS_KRF(configNode->hasAttribute(LDA_CONFIG_FILE_VERSION_ATTRIBUTE_NAME),
						"Invalid model: model trained with an obsolete version of LDA", OpenViBE::Kernel::ErrorType::BadConfig);

	m_labels.clear();
	m_discriminantFunctions.clear();

	XML::IXMLNode* tmpNode = configNode->getChildByName(CLASSES_NODE_NAME);

	OV_ERROR_UNLESS_KRF(tmpNode != nullptr, "Failed to retrieve xml node", OpenViBE::Kernel::ErrorType::BadParsing);

	loadClassesFromNode(tmpNode);


	//We send corresponding data to the computation helper
	XML::IXMLNode* configsNode = configNode->getChildByName(COMPUTATION_HELPERS_CONFIGURATION_NODE);

	for (size_t i = 0; i < configsNode->getChildCount(); ++i)
	{
		m_discriminantFunctions.push_back(CAlgorithmLDADiscriminantFunction());
		m_discriminantFunctions[i].loadConfig(configsNode->getChild(i));
	}

	return true;
}

void CAlgorithmClassifierLDA::loadClassesFromNode(XML::IXMLNode* node)
{
	std::stringstream ss(node->getPCData());
	double value;
	while (ss >> value) { m_labels.push_back(value); }
	m_nClasses = m_labels.size();
}

//Load the weight vector
void CAlgorithmClassifierLDA::loadCoefsFromNode(XML::IXMLNode* node)
{
	std::stringstream ss(node->getPCData());

	std::vector<double> coefs;
	double value;
	while (ss >> value) { coefs.push_back(value); }

	m_weights.resize(1, coefs.size());
	m_nCols = coefs.size();
	for (size_t i = 0; i < coefs.size(); ++i) { m_weights(0, i) = coefs[i]; }
}

#endif // TARGET_HAS_ThirdPartyEIGEN
