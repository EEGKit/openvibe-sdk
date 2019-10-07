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
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

bool CAlgorithmClassifierOneVsOne::initialize()
{
	TParameterHandler<XML::IXMLNode*> op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	op_pConfiguration = nullptr;

	TParameterHandler<uint64_t> ip_pPairwise(this->getInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType));
	ip_pPairwise = OV_UndefinedIdentifier.toUInteger();

	m_pDecisionStrategyAlgorithm  = nullptr;
	m_oPairwiseDecisionID = OV_UndefinedIdentifier;

	return CAlgorithmPairingStrategy::initialize();
}

bool CAlgorithmClassifierOneVsOne::uninitialize()
{
	if (m_pDecisionStrategyAlgorithm != nullptr)
	{
		m_pDecisionStrategyAlgorithm->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pDecisionStrategyAlgorithm);
		m_pDecisionStrategyAlgorithm = nullptr;
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


bool CAlgorithmClassifierOneVsOne::train(const IFeatureVectorSet& rFeatureVectorSet)
{
	TParameterHandler<uint64_t> ip_pNumberOfClasses(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));
	m_nClasses = uint32_t(ip_pNumberOfClasses);

	m_nSubClassifiers = m_nClasses * (m_nClasses - 1) / 2;

	createSubClassifiers();

	//Create the decision strategy
	OV_ERROR_UNLESS_KRF(this->initializeExtraParameterMechanism(), "Failed to initialize extra parameters", OpenViBE::Kernel::ErrorType::Internal);

	m_oPairwiseDecisionID = this->getEnumerationParameter(
		OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType, OVP_TypeId_ClassificationPairwiseStrategy);

	OV_ERROR_UNLESS_KRF(m_oPairwiseDecisionID != OV_UndefinedIdentifier,
						"Invalid pairwise decision strategy [" << OVP_TypeId_ClassificationPairwiseStrategy.toString() << "]",
						OpenViBE::Kernel::ErrorType::BadConfig);

	if (m_pDecisionStrategyAlgorithm != nullptr)
	{
		m_pDecisionStrategyAlgorithm->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pDecisionStrategyAlgorithm);
		m_pDecisionStrategyAlgorithm = nullptr;
	}
	m_pDecisionStrategyAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(m_oPairwiseDecisionID));

	OV_ERROR_UNLESS_KRF(m_pDecisionStrategyAlgorithm->initialize(), "Failed to unitialize decision strategy algorithm", OpenViBE::Kernel::ErrorType::Internal);

	TParameterHandler<CIdentifier *> ip_pClassificationAlgorithm(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier));
	ip_pClassificationAlgorithm = &m_subClassifierAlgorithmID;
	TParameterHandler<uint64_t> ip_pClassCount(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	ip_pClassCount = m_nClasses;

	OV_ERROR_UNLESS_KRF(m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize),
						"Failed to run decision strategy algorithm",
						OpenViBE::Kernel::ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(this->uninitializeExtraParameterMechanism(), "Failed to uninitialize extra parameters", OpenViBE::Kernel::ErrorType::Internal);

	//Calculate the amount of sample for each class
	std::map<double, uint32_t> l_vClassLabels;
	for (uint32_t i = 0; i < rFeatureVectorSet.getFeatureVectorCount(); ++i)
	{
		if (!l_vClassLabels.count(rFeatureVectorSet[i].getLabel())) { l_vClassLabels[rFeatureVectorSet[i].getLabel()] = 0; }
		l_vClassLabels[rFeatureVectorSet[i].getLabel()]++;
	}

	OV_ERROR_UNLESS_KRF(
		l_vClassLabels.size() == m_nClasses,
		"There are samples for " << uint32_t(l_vClassLabels.size()) << " classes but expected samples for " << m_nClasses << " classes.",
		OpenViBE::Kernel::ErrorType::BadConfig);

	//Now we create the corresponding repartition set
	TParameterHandler<IMatrix*> ip_pRepartitionSet = m_pDecisionStrategyAlgorithm->getInputParameter(
		OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	ip_pRepartitionSet->setDimensionCount(1);
	ip_pRepartitionSet->setDimensionSize(0, m_nClasses);

	const uint32_t l_iFeatureVectorSize = rFeatureVectorSet[0].getSize();
	//Now let's train each classifier
	for (uint32_t firstClass = 0; firstClass < m_nClasses; ++firstClass)
	{
		ip_pRepartitionSet->getBuffer()[firstClass] = double(l_vClassLabels[double(firstClass)]);

		for (uint32_t secondClass = firstClass + 1; secondClass < m_nClasses; ++secondClass)
		{
			const uint32_t nFeature = l_vClassLabels[double(firstClass)] + l_vClassLabels[double(secondClass)];

			IAlgorithmProxy* subClassifier = m_subClassifiers[std::pair<uint32_t, uint32_t>(firstClass, secondClass)];

			TParameterHandler<IMatrix*> ip_pFeatureVectorSet(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));
			ip_pFeatureVectorSet->setDimensionCount(2);
			ip_pFeatureVectorSet->setDimensionSize(0, nFeature);
			ip_pFeatureVectorSet->setDimensionSize(1, l_iFeatureVectorSize + 1);

			double* l_pFeatureVectorSetBuffer = ip_pFeatureVectorSet->getBuffer();
			for (uint32_t j = 0; j < rFeatureVectorSet.getFeatureVectorCount(); ++j)
			{
				const double l_f64TempClass = rFeatureVectorSet[j].getLabel();
				if (l_f64TempClass == double(firstClass) || l_f64TempClass == double(secondClass))
				{
					System::Memory::copy(l_pFeatureVectorSetBuffer, rFeatureVectorSet[j].getBuffer(), l_iFeatureVectorSize * sizeof(double));

					l_pFeatureVectorSetBuffer[l_iFeatureVectorSize] = size_t(l_f64TempClass) == firstClass ? 0 : 1;
					l_pFeatureVectorSetBuffer += (l_iFeatureVectorSize + 1);
				}
			}

			OV_ERROR_UNLESS_KRF(
				subClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Train),
				"Failed to train subclassifier [1st class = " << uint64_t(firstClass) << ", 2nd class = " << uint64_t(secondClass) << "]",
				OpenViBE::Kernel::ErrorType::Internal);
		}
	}
	return true;
}

bool CAlgorithmClassifierOneVsOne::classify(const IFeatureVector& featureVector, double& classId, IVector& distance, IVector& probability)
{
	OV_ERROR_UNLESS_KRF(m_pDecisionStrategyAlgorithm, "No decision strategy algorithm set", OpenViBE::Kernel::ErrorType::BadConfig);

	const uint32_t featureVectorSize = featureVector.getSize();
	std::vector<SClassificationInfo> l_oClassificationList;

	TParameterHandler<IMatrix*> probabilityMatrix = m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_InputParameter_ProbabilityMatrix);
	IMatrix* l_pProbabilityMatrix                 = static_cast<IMatrix*>(probabilityMatrix);

	l_pProbabilityMatrix->setDimensionCount(2);
	l_pProbabilityMatrix->setDimensionSize(0, m_nClasses);
	l_pProbabilityMatrix->setDimensionSize(1, m_nClasses);

	for (uint32_t i = 0; i < l_pProbabilityMatrix->getBufferElementCount(); ++i) { l_pProbabilityMatrix->getBuffer()[i] = 0.0; }

	//Let's generate the matrix of confidence score
	for (uint32_t i = 0; i < m_nClasses; ++i)
	{
		for (uint32_t j = i + 1; j < m_nClasses; ++j)
		{
			IAlgorithmProxy* tmp = m_subClassifiers[std::pair<uint32_t, uint32_t>(i, j)];
			TParameterHandler<IMatrix*> ip_pFeatureVector(tmp->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector));
			TParameterHandler<IMatrix*> op_pClassificationValues(
				tmp->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
			TParameterHandler<double> op_pClassificationLabel(tmp->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));
			ip_pFeatureVector->setDimensionCount(1);
			ip_pFeatureVector->setDimensionSize(0, featureVectorSize);

			double* l_pFeatureVectorBuffer = ip_pFeatureVector->getBuffer();
			System::Memory::copy(l_pFeatureVectorBuffer, featureVector.getBuffer(), featureVectorSize * sizeof(double));
			tmp->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify);

			SClassificationInfo classificationInfo = { double(i), double(j), op_pClassificationLabel, op_pClassificationValues };
			l_oClassificationList.push_back(classificationInfo);
		}
	}

	//	for (size_t i =0 ; i < l_ui32ClassCount ; ++i )
	//	{
	//		for (size_t j = 0; j < l_ui32ClassCount ; ++j) { std::cout << l_pProbabilityMatrix->getBuffer()[i*l_ui32ClassCount + j] << " " ; }
	//		std::cout << std::endl;
	//	}
	//	std::cout << std::endl;

	TParameterHandler<std::vector<SClassificationInfo> *> ip_pClassificationInfos(
		m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs));
	ip_pClassificationInfos = &l_oClassificationList;

	//Then ask to the strategy to make the decision
	OV_ERROR_UNLESS_KRF(m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute), "Failed to compute decision strategy", OpenViBE::Kernel::ErrorType::Internal);

	TParameterHandler<IMatrix*> op_pProbabilityVector = m_pDecisionStrategyAlgorithm->getOutputParameter(
		OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector);
	double maxProb       = -1;
	int selectedClassIdx = -1;

	distance.setSize(0);
	probability.setSize(m_nClasses);

	//We just have to take the most relevant now.
	for (uint32_t i = 0; i < m_nClasses; ++i)
	{
		const double tmp = op_pProbabilityVector->getBuffer()[i];
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
		IAlgorithmProxy* l_pSubClassifier = kv.second;
		l_pSubClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*l_pSubClassifier);
	}
	this->m_subClassifiers.clear();

	//Now let's instantiate all the sub classifiers
	for (uint32_t firstClass = 0; firstClass < m_nClasses; ++firstClass)
	{
		for (uint32_t secondClass = firstClass + 1; secondClass < m_nClasses; ++secondClass)
		{
			const CIdentifier l_oSubClassifierAlgorithm = this->getAlgorithmManager().createAlgorithm(this->m_subClassifierAlgorithmID);

			OV_ERROR_UNLESS_KRF(
				l_oSubClassifierAlgorithm != OV_UndefinedIdentifier,
				"Unable to instantiate classifier for class [" << this->m_subClassifierAlgorithmID.toString() << "]",
				OpenViBE::Kernel::ErrorType::BadConfig);

			IAlgorithmProxy* l_pSubClassifier = &this->getAlgorithmManager().getAlgorithm(l_oSubClassifierAlgorithm);
			l_pSubClassifier->initialize();

			TParameterHandler<uint64_t> ip_pNumberOfClasses(l_pSubClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));
			ip_pNumberOfClasses = 2;

			//Set a references to the extra parameters input of the pairing strategy
			TParameterHandler<std::map<CString, CString>*> ip_pExtraParameters(
				l_pSubClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));
			ip_pExtraParameters.setReferenceTarget(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));

			m_subClassifiers[std::pair<uint32_t, uint32_t>(firstClass, secondClass)] = l_pSubClassifier;
		}
	}
	return true;
}

bool CAlgorithmClassifierOneVsOne::designArchitecture(const CIdentifier& id, const uint32_t classCount)
{
	if (!setSubClassifierIdentifier(id)) { return false; }
	m_nClasses = classCount;
	return true;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::getClassifierConfiguration(const double firstClass, const double secondClass, IAlgorithmProxy* subClassifier)
{
	XML::IXMLNode* res = XML::createNode(SUB_CLASSIFIER_NODE_NAME);

	std::stringstream ssFirstClass, ssSecondClass;
	ssFirstClass << firstClass;
	ssSecondClass << secondClass;
	res->addAttribute(FIRST_CLASS_ATRRIBUTE_NAME, ssFirstClass.str().c_str());
	res->addAttribute(SECOND_CLASS_ATTRIBUTE_NAME, ssSecondClass.str().c_str());

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(subClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	subClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration);
	res->addChild(static_cast<XML::IXMLNode*>(op_pConfiguration));

	return res;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::getPairwiseDecisionConfiguration() const
{
	if (!m_pDecisionStrategyAlgorithm) { return nullptr; }

	XML::IXMLNode* tmp = XML::createNode(PAIRWISE_DECISION_NAME);

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(m_pDecisionStrategyAlgorithm->getOutputParameter(OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Configuration));
	m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfiguration);
	tmp->addChild(static_cast<XML::IXMLNode*>(op_pConfiguration));

	tmp->addAttribute(ALGORITHM_ID_ATTRIBUTE, m_oPairwiseDecisionID.toString());

	return tmp;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::saveConfiguration()
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

	for (auto& kv : m_subClassifiers) { subClassifersNode->addChild(getClassifierConfiguration(kv.first.first, kv.first.second, kv.second)); }
	oneVsOneNode->addChild(subClassifersNode);

	return oneVsOneNode;
}

bool CAlgorithmClassifierOneVsOne::loadConfiguration(XML::IXMLNode* configNode)
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
	if (pairwiseID != m_oPairwiseDecisionID)
	{
		if (m_pDecisionStrategyAlgorithm != nullptr)
		{
			m_pDecisionStrategyAlgorithm->uninitialize();
			this->getAlgorithmManager().releaseAlgorithm(*m_pDecisionStrategyAlgorithm);
			m_pDecisionStrategyAlgorithm = nullptr;
		}
		m_oPairwiseDecisionID = pairwiseID;
		m_pDecisionStrategyAlgorithm  = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(m_oPairwiseDecisionID));
		m_pDecisionStrategyAlgorithm->initialize();
	}
	TParameterHandler<XML::IXMLNode*> ip_pConfiguration(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_Configuration));
	ip_pConfiguration = tempNode->getChild(0);

	TParameterHandler<CIdentifier *> ip_pClassificationAlgorithm(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier));
	ip_pClassificationAlgorithm = &algorithmID;

	tempNode = configNode->getChildByName(SUB_CLASSIFIER_COUNT_NODE_NAME);
	std::stringstream l_sCountData(tempNode->getPCData());
	l_sCountData >> m_nSubClassifiers;

	// Invert the class count from subCls = numClass*(numClass-1)/2.
	const uint32_t deltaCarre = 1 + 8 * m_nSubClassifiers;
	m_nClasses                = uint32_t((1 + sqrt(double(deltaCarre))) / 2);

	TParameterHandler<uint64_t> ip_pClassCount(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	ip_pClassCount = m_nClasses;

	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfiguration),
		"Loading decision strategy configuration failed",
		OpenViBE::Kernel::ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize),
		"Parameterizing decision strategy failed",
		OpenViBE::Kernel::ErrorType::Internal);

	return loadSubClassifierConfiguration(configNode->getChildByName(SUB_CLASSIFIERS_NODE_NAME));
}

uint32_t CAlgorithmClassifierOneVsOne::getOutputProbabilityVectorLength() { return uint32_t(m_nClasses); }

uint32_t CAlgorithmClassifierOneVsOne::getOutputDistanceVectorLength() { return 0; }

bool CAlgorithmClassifierOneVsOne::loadSubClassifierConfiguration(XML::IXMLNode* subClassifiersNode)
{
	createSubClassifiers();

	for (uint32_t i = 0; i < subClassifiersNode->getChildCount(); ++i)
	{
		double firstClass, secondClass;

		//Now we have to restore class indexes
		XML::IXMLNode* subClassifierNode = subClassifiersNode->getChild(i);
		std::stringstream ssFirstClass(subClassifierNode->getAttribute(FIRST_CLASS_ATRRIBUTE_NAME));
		ssFirstClass >> firstClass;
		std::stringstream ssSecondClass(subClassifierNode->getAttribute(SECOND_CLASS_ATTRIBUTE_NAME));
		ssSecondClass >> secondClass;

		IAlgorithmProxy* subClassifier = m_subClassifiers[std::make_pair(uint32_t(firstClass), uint32_t(secondClass))];

		TParameterHandler<XML::IXMLNode*> ip_pConfiguration(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
		ip_pConfiguration = subClassifierNode->getChild(0);

		OV_ERROR_UNLESS_KRF(subClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration),
							"Unable to load the configuration for the sub-classifier " << uint64_t(i+1), OpenViBE::Kernel::ErrorType::Internal);
	}

	OV_ERROR_UNLESS_KRF(m_subClassifiers.size() == m_nSubClassifiers,
						"Invalid number of loaded classifiers [" << uint64_t(m_subClassifiers.size()) << "] (expected = " << m_nSubClassifiers << ")",
						OpenViBE::Kernel::ErrorType::Internal);

	return true;
}

bool CAlgorithmClassifierOneVsOne::setSubClassifierIdentifier(const CIdentifier& id)
{
	m_subClassifierAlgorithmID = id;
	m_algorithmComparison      = getClassificationComparisonFunction(id);

	OV_ERROR_UNLESS_KRF(m_algorithmComparison != nullptr, "No comparison function found for classifier " << m_subClassifierAlgorithmID.toString(), OpenViBE::Kernel::ErrorType::ResourceNotFound);

	return true;
}
