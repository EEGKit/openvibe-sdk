#include "ovpCAlgorithmClassifierOneVsOne.h"
#include "ovpCAlgorithmPairwiseDecision.h"

#include <map>
#include <cmath>
#include <sstream>
#include <utility>
#include <iostream>
#include <system/ovCMemory.h>

namespace
{
	const char* const TYPE_NODE_NAME                      = "OneVsOne";
	const char* const SUB_CLASSIFIER_IDENTIFIER_NODE_NAME = "SubClassifierIdentifier";
	const char* const PAIRWISE_DECISION_NAME              = "PairwiseDecision";
	const char* const ALGORITHM_ID_ATTRIBUTE              = "algorithm-id";
	const char* const SUB_CLASSIFIER_COUNT_NODE_NAME      = "SubClassifierCount";
	const char* const SUB_CLASSIFIERS_NODE_NAME           = "SubClassifiers";
	const char* const SUB_CLASSIFIER_NODE_NAME            = "SubClassifier";
	const char* const FIRST_CLASS_ATRRIBUTE_NAME          = "first-class";
	const char* const SECOND_CLASS_ATTRIBUTE_NAME         = "second-class";

	//This map is used to record the decision strategies available for each algorithm
	//std::map<uint64_t, OpenViBE::CIdentifier> g_oDecisionMap;
} // namespace

extern const char* const CLASSIFIER_ROOT;

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace Classification;

using namespace /*OpenViBE::*/Toolkit;

bool CAlgorithmClassifierOneVsOne::initialize()
{
	TParameterHandler<XML::IXMLNode*> op_configuration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Config));
	op_configuration = nullptr;

	TParameterHandler<uint64_t> ip_pPairwise(this->getInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType));
	ip_pPairwise = OV_UndefinedIdentifier.toUInteger();

	m_decisionStrategyAlgorithm = nullptr;
	m_pairwiseDecisionID        = OV_UndefinedIdentifier;

	return CAlgorithmPairingStrategy::initialize();
}

bool CAlgorithmClassifierOneVsOne::uninitialize()
{
	if (m_decisionStrategyAlgorithm != nullptr)
	{
		m_decisionStrategyAlgorithm->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_decisionStrategyAlgorithm);
		m_decisionStrategyAlgorithm = nullptr;
	}

	for (auto& kv : m_subClassifiers)
	{
		IAlgorithmProxy* subClassifier = kv.second;
		subClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*subClassifier);
	}
	this->m_subClassifiers.clear();

	return CAlgorithmPairingStrategy::uninitialize();
}


bool CAlgorithmClassifierOneVsOne::train(const IFeatureVectorSet& dataset)
{
	TParameterHandler<uint64_t> ip_nClasses(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NClasses));
	m_nClasses = size_t(ip_nClasses);

	m_nSubClassifiers = m_nClasses * (m_nClasses - 1) / 2;

	createSubClassifiers();

	//Create the decision strategy
	OV_ERROR_UNLESS_KRF(this->initializeExtraParameterMechanism(), "Failed to initialize extra parameters", ErrorType::Internal);

	m_pairwiseDecisionID = this->getEnumerationParameter(
		OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType, OVP_TypeId_ClassificationPairwiseStrategy);

	OV_ERROR_UNLESS_KRF(m_pairwiseDecisionID != OV_UndefinedIdentifier,
						"Invalid pairwise decision strategy [" << OVP_TypeId_ClassificationPairwiseStrategy.str() << "]",
						ErrorType::BadConfig);

	if (m_decisionStrategyAlgorithm != nullptr)
	{
		m_decisionStrategyAlgorithm->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_decisionStrategyAlgorithm);
		m_decisionStrategyAlgorithm = nullptr;
	}
	m_decisionStrategyAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(m_pairwiseDecisionID));

	OV_ERROR_UNLESS_KRF(m_decisionStrategyAlgorithm->initialize(), "Failed to unitialize decision strategy algorithm", ErrorType::Internal);

	TParameterHandler<CIdentifier *> ip_classificationAlgorithm(
		m_decisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier));
	ip_classificationAlgorithm = &m_subClassifierAlgorithmID;
	TParameterHandler<uint64_t> ip_classCount(m_decisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	ip_classCount = m_nClasses;

	OV_ERROR_UNLESS_KRF(m_decisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize),
						"Failed to run decision strategy algorithm",
						ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(this->uninitializeExtraParameterMechanism(), "Failed to uninitialize extra parameters", ErrorType::Internal);

	//Calculate the amount of sample for each class
	std::map<double, size_t> classLabels;
	for (size_t i = 0; i < dataset.getFeatureVectorCount(); ++i)
	{
		if (!classLabels.count(dataset[i].getLabel())) { classLabels[dataset[i].getLabel()] = 0; }
		classLabels[dataset[i].getLabel()]++;
	}

	OV_ERROR_UNLESS_KRF(
		classLabels.size() == m_nClasses,
		"There are samples for " << classLabels.size() << " classes but expected samples for " << m_nClasses << " classes.",
		ErrorType::BadConfig);

	//Now we create the corresponding repartition set
	TParameterHandler<IMatrix*> ip_pRepartitionSet = m_decisionStrategyAlgorithm->getInputParameter(
		OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	ip_pRepartitionSet->setDimensionCount(1);
	ip_pRepartitionSet->setDimensionSize(0, m_nClasses);

	const size_t size = dataset[0].getSize();
	//Now let's train each classifier
	for (size_t i = 0; i < m_nClasses; ++i)
	{
		ip_pRepartitionSet->getBuffer()[i] = double(classLabels[double(i)]);

		for (size_t j = i + 1; j < m_nClasses; ++j)
		{
			const size_t nFeature          = classLabels[double(i)] + classLabels[double(j)];
			IAlgorithmProxy* subClassifier = m_subClassifiers[std::pair<size_t, size_t>(i, j)];

			TParameterHandler<IMatrix*> ip_dataset(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));
			ip_dataset->setDimensionCount(2);
			ip_dataset->setDimensionSize(0, nFeature);
			ip_dataset->setDimensionSize(1, size + 1);

			double* buffer = ip_dataset->getBuffer();
			for (size_t k = 0; k < dataset.getFeatureVectorCount(); ++k)
			{
				const double tmp = dataset[k].getLabel();
				if (tmp == double(i) || tmp == double(j))
				{
					System::Memory::copy(buffer, dataset[k].getBuffer(), size * sizeof(double));

					buffer[size] = size_t(tmp) == i ? 0 : 1;
					buffer += (size + 1);
				}
			}

			OV_ERROR_UNLESS_KRF(
				subClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Train),
				"Failed to train subclassifier [1st class = " << i << ", 2nd class = " << j << "]",
				ErrorType::Internal);
		}
	}
	return true;
}

bool CAlgorithmClassifierOneVsOne::classify(const IFeatureVector& sample, double& classId, IVector& distance, IVector& probability)
{
	OV_ERROR_UNLESS_KRF(m_decisionStrategyAlgorithm, "No decision strategy algorithm set", ErrorType::BadConfig);

	const size_t size = sample.getSize();
	std::vector<classification_info_t> classificationList;

	TParameterHandler<IMatrix*> ip_proba = m_decisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_InputParameter_ProbabilityMatrix);
	IMatrix* matrix                      = static_cast<IMatrix*>(ip_proba);

	matrix->setDimensionCount(2);
	matrix->setDimensionSize(0, m_nClasses);
	matrix->setDimensionSize(1, m_nClasses);

	for (size_t i = 0; i < matrix->getBufferElementCount(); ++i) { matrix->getBuffer()[i] = 0.0; }

	//Let's generate the matrix of confidence score
	for (size_t i = 0; i < m_nClasses; ++i)
	{
		for (size_t j = i + 1; j < m_nClasses; ++j)
		{
			IAlgorithmProxy* tmp = m_subClassifiers[std::pair<size_t, size_t>(i, j)];
			TParameterHandler<IMatrix*> ip_sample(tmp->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector));
			TParameterHandler<IMatrix*> op_values(tmp->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
			TParameterHandler<double> op_label(tmp->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));
			ip_sample->setDimensionCount(1);
			ip_sample->setDimensionSize(0, size);

			double* buffer = ip_sample->getBuffer();
			System::Memory::copy(buffer, sample.getBuffer(), size * sizeof(double));
			tmp->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify);

			classification_info_t classificationInfo = { double(i), double(j), op_label, op_values };
			classificationList.push_back(classificationInfo);
		}
	}

	//	for (size_t i =0 ; i < nClass ; ++i )
	//	{
	//		for (size_t j = 0; j < nClass ; ++j) { std::cout << matrix->getBuffer()[i*nClass + j] << " " ; }
	//		std::cout << std::endl;
	//	}
	//	std::cout << std::endl;

	TParameterHandler<std::vector<classification_info_t> *> ip_infos(
		m_decisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs));
	ip_infos = &classificationList;

	//Then ask to the strategy to make the decision
	OV_ERROR_UNLESS_KRF(m_decisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute), "Failed to compute decision strategy",
						ErrorType::Internal);

	TParameterHandler<IMatrix*> op_proba = m_decisionStrategyAlgorithm->getOutputParameter(
		OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector);
	double maxProb       = -1;
	int selectedClassIdx = -1;

	distance.setSize(0);
	probability.setSize(m_nClasses);

	//We just have to take the most relevant now.
	for (size_t i = 0; i < m_nClasses; ++i)
	{
		const double tmp = op_proba->getBuffer()[i];
		if (tmp > maxProb)
		{
			selectedClassIdx = i;
			maxProb          = tmp;
		}
		probability[i] = tmp;
	}

	classId = double(selectedClassIdx);
	return true;
}

bool CAlgorithmClassifierOneVsOne::createSubClassifiers()
{
	// Clear any previous ones
	for (auto& kv : m_subClassifiers)
	{
		IAlgorithmProxy* subClassifier = kv.second;
		subClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*subClassifier);
	}
	this->m_subClassifiers.clear();

	//Now let's instantiate all the sub classifiers
	for (size_t firstClass = 0; firstClass < m_nClasses; ++firstClass)
	{
		for (size_t secondClass = firstClass + 1; secondClass < m_nClasses; ++secondClass)
		{
			const CIdentifier subClassifierAlgorithm = this->getAlgorithmManager().createAlgorithm(this->m_subClassifierAlgorithmID);

			OV_ERROR_UNLESS_KRF(
				subClassifierAlgorithm != OV_UndefinedIdentifier,
				"Unable to instantiate classifier for class [" << this->m_subClassifierAlgorithmID.str() << "]",
				ErrorType::BadConfig);

			IAlgorithmProxy* subClassifier = &this->getAlgorithmManager().getAlgorithm(subClassifierAlgorithm);
			subClassifier->initialize();

			TParameterHandler<uint64_t> ip_pNClasses(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NClasses));
			ip_pNClasses = 2;

			//Set a references to the extra parameters input of the pairing strategy
			TParameterHandler<std::map<CString, CString>*> ip_params(
				subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));
			ip_params.setReferenceTarget(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));

			m_subClassifiers[std::pair<size_t, size_t>(firstClass, secondClass)] = subClassifier;
		}
	}
	return true;
}

bool CAlgorithmClassifierOneVsOne::designArchitecture(const CIdentifier& id, const size_t classCount)
{
	if (!setSubClassifierIdentifier(id)) { return false; }
	m_nClasses = classCount;
	return true;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::getClassifierConfig(const double firstClass, const double secondClass, IAlgorithmProxy* subClassifier)
{
	XML::IXMLNode* res = XML::createNode(SUB_CLASSIFIER_NODE_NAME);

	std::stringstream ssFirstClass, ssSecondClass;
	ssFirstClass << firstClass;
	ssSecondClass << secondClass;
	res->addAttribute(FIRST_CLASS_ATRRIBUTE_NAME, ssFirstClass.str().c_str());
	res->addAttribute(SECOND_CLASS_ATTRIBUTE_NAME, ssSecondClass.str().c_str());

	TParameterHandler<XML::IXMLNode*> op_config(subClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Config));
	subClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_SaveConfig);
	res->addChild(static_cast<XML::IXMLNode*>(op_config));

	return res;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::getPairwiseDecisionConfiguration() const
{
	if (!m_decisionStrategyAlgorithm) { return nullptr; }

	XML::IXMLNode* tmp = XML::createNode(PAIRWISE_DECISION_NAME);

	TParameterHandler<XML::IXMLNode*> op_config(m_decisionStrategyAlgorithm->getOutputParameter(OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Config));
	m_decisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfig);
	tmp->addChild(static_cast<XML::IXMLNode*>(op_config));

	tmp->addAttribute(ALGORITHM_ID_ATTRIBUTE, m_pairwiseDecisionID.toString());

	return tmp;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::saveConfig()
{
	std::stringstream nClassifier;
	nClassifier << m_nSubClassifiers;

	XML::IXMLNode* oneVsOneNode = XML::createNode(TYPE_NODE_NAME);

	XML::IXMLNode* tmp = XML::createNode(SUB_CLASSIFIER_IDENTIFIER_NODE_NAME);
	tmp->addAttribute(ALGORITHM_ID_ATTRIBUTE, this->m_subClassifierAlgorithmID.toString());
	tmp->setPCData(
		this->getTypeManager().getEnumerationEntryNameFromValue(OVTK_TypeId_ClassificationAlgorithm, m_subClassifierAlgorithmID.toUInteger()).
			  toASCIIString());
	oneVsOneNode->addChild(tmp);

	tmp = XML::createNode(SUB_CLASSIFIER_COUNT_NODE_NAME);
	tmp->setPCData(nClassifier.str().c_str());
	oneVsOneNode->addChild(tmp);

	oneVsOneNode->addChild(this->getPairwiseDecisionConfiguration());

	XML::IXMLNode* subClassifersNode = XML::createNode(SUB_CLASSIFIERS_NODE_NAME);

	for (auto& kv : m_subClassifiers) { subClassifersNode->addChild(getClassifierConfig(kv.first.first, kv.first.second, kv.second)); }
	oneVsOneNode->addChild(subClassifersNode);

	return oneVsOneNode;
}

bool CAlgorithmClassifierOneVsOne::loadConfig(XML::IXMLNode* configNode)
{
	XML::IXMLNode* tempNode = configNode->getChildByName(SUB_CLASSIFIER_IDENTIFIER_NODE_NAME);

	CIdentifier algorithmID;
	algorithmID.fromString(tempNode->getAttribute(ALGORITHM_ID_ATTRIBUTE));

	if (!this->setSubClassifierIdentifier(algorithmID))
	{
		//if the sub classifier doesn't have comparison function it is an error
		return false;
	}

	tempNode = configNode->getChildByName(PAIRWISE_DECISION_NAME);
	CIdentifier pairwiseID;
	pairwiseID.fromString(tempNode->getAttribute(ALGORITHM_ID_ATTRIBUTE));
	if (pairwiseID != m_pairwiseDecisionID)
	{
		if (m_decisionStrategyAlgorithm != nullptr)
		{
			m_decisionStrategyAlgorithm->uninitialize();
			this->getAlgorithmManager().releaseAlgorithm(*m_decisionStrategyAlgorithm);
			m_decisionStrategyAlgorithm = nullptr;
		}
		m_pairwiseDecisionID        = pairwiseID;
		m_decisionStrategyAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(m_pairwiseDecisionID));
		m_decisionStrategyAlgorithm->initialize();
	}
	TParameterHandler<XML::IXMLNode*> ip_config(m_decisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_Config));
	ip_config = tempNode->getChild(0);

	TParameterHandler<CIdentifier *> ip_algorithm(
		m_decisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier));
	ip_algorithm = &algorithmID;

	tempNode = configNode->getChildByName(SUB_CLASSIFIER_COUNT_NODE_NAME);
	std::stringstream ss(tempNode->getPCData());
	ss >> m_nSubClassifiers;

	// Invert the class count from subCls = numClass*(numClass-1)/2.
	const size_t deltaCarre = 1 + 8 * m_nSubClassifiers;
	m_nClasses              = size_t((1 + sqrt(double(deltaCarre))) / 2);

	TParameterHandler<uint64_t> ip_classCount(m_decisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	ip_classCount = m_nClasses;

	OV_ERROR_UNLESS_KRF(m_decisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfig),
						"Loading decision strategy configuration failed", ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(m_decisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize),
						"Parameterizing decision strategy failed", ErrorType::Internal);

	return loadSubClassifierConfig(configNode->getChildByName(SUB_CLASSIFIERS_NODE_NAME));
}

bool CAlgorithmClassifierOneVsOne::loadSubClassifierConfig(XML::IXMLNode* node)
{
	createSubClassifiers();

	for (size_t i = 0; i < node->getChildCount(); ++i)
	{
		double firstClass, secondClass;

		//Now we have to restore class indexes
		XML::IXMLNode* subClassifierNode = node->getChild(i);
		std::stringstream ss1(subClassifierNode->getAttribute(FIRST_CLASS_ATRRIBUTE_NAME));
		ss1 >> firstClass;
		std::stringstream ss2(subClassifierNode->getAttribute(SECOND_CLASS_ATTRIBUTE_NAME));
		ss2 >> secondClass;

		IAlgorithmProxy* subClassifier = m_subClassifiers[std::make_pair(size_t(firstClass), size_t(secondClass))];

		TParameterHandler<XML::IXMLNode*> ip_config(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Config));
		ip_config = subClassifierNode->getChild(0);

		OV_ERROR_UNLESS_KRF(subClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfig),
							"Unable to load the configuration for the sub-classifier " << i + 1, ErrorType::Internal);
	}

	OV_ERROR_UNLESS_KRF(m_subClassifiers.size() == m_nSubClassifiers,
						"Invalid number of loaded classifiers [" << m_subClassifiers.size() << "] (expected = " << m_nSubClassifiers << ")",
						ErrorType::Internal);

	return true;
}

bool CAlgorithmClassifierOneVsOne::setSubClassifierIdentifier(const CIdentifier& id)
{
	m_subClassifierAlgorithmID = id;
	m_algorithmComparison      = getClassificationComparisonFunction(id);

	OV_ERROR_UNLESS_KRF(m_algorithmComparison != nullptr, "No comparison function found for classifier " << m_subClassifierAlgorithmID.str(),
						ErrorType::ResourceNotFound);

	return true;
}
