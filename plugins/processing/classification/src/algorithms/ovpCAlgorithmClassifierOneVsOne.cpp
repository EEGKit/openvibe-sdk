#include "ovpCAlgorithmClassifierOneVsOne.h"
#include "ovpCAlgorithmPairwiseDecision.h"

#include <map>
#include <cmath>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <string>
#include <utility>
#include <iostream>
#include <system/ovCMemory.h>

namespace
{
	const char* const c_sTypeNodeName                    = "OneVsOne";
	const char* const c_sSubClassifierIdentifierNodeName = "SubClassifierIdentifier";
	const char* const c_sPairwiseDecisionName            = "PairwiseDecision";
	const char* const c_sAlgorithmIdAttribute            = "algorithm-id";
	const char* const c_sSubClassifierCountNodeName      = "SubClassifierCount";
	const char* const c_sSubClassifiersNodeName          = "SubClassifiers";
	const char* const c_sSubClassifierNodeName           = "SubClassifier";

	const char* const c_sFirstClassAtrributeName  = "first-class";
	const char* const c_sSecondClassAttributeName = "second-class";

	//This map is used to record the decision strategies available for each algorithm
	std::map<uint64_t, OpenViBE::CIdentifier> g_oDecisionMap;
}

extern const char* const c_sClassifierRoot;

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
	m_oPairwiseDecisionIdentifier = OV_UndefinedIdentifier;

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

	for (auto& kv : m_oSubClassifiers)
	{
		IAlgorithmProxy* l_pSubClassifier = kv.second;
		l_pSubClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*l_pSubClassifier);
	}
	this->m_oSubClassifiers.clear();

	return CAlgorithmPairingStrategy::uninitialize();
}


bool CAlgorithmClassifierOneVsOne::train(const IFeatureVectorSet& rFeatureVectorSet)
{
	TParameterHandler<uint64_t> ip_pNumberOfClasses(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));
	m_ui32NumberOfClasses = static_cast<uint32_t>(ip_pNumberOfClasses);

	m_ui32NumberOfSubClassifiers = m_ui32NumberOfClasses * (m_ui32NumberOfClasses - 1) / 2;

	createSubClassifiers();

	//Create the decision strategy
	OV_ERROR_UNLESS_KRF(
		this->initializeExtraParameterMechanism(),
		"Failed to initialize extra parameters",
		OpenViBE::Kernel::ErrorType::Internal);

	m_oPairwiseDecisionIdentifier = this->getEnumerationParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType, OVP_TypeId_ClassificationPairwiseStrategy);

	OV_ERROR_UNLESS_KRF(
		m_oPairwiseDecisionIdentifier != OV_UndefinedIdentifier,
		"Invalid pairwise decision strategy [" << OVP_TypeId_ClassificationPairwiseStrategy.toString() << "]",
		OpenViBE::Kernel::ErrorType::BadConfig);

	if (m_pDecisionStrategyAlgorithm != nullptr)
	{
		m_pDecisionStrategyAlgorithm->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pDecisionStrategyAlgorithm);
		m_pDecisionStrategyAlgorithm = nullptr;
	}
	m_pDecisionStrategyAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(m_oPairwiseDecisionIdentifier));

	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm->initialize(),
		"Failed to unitialize decision strategy algorithm",
		OpenViBE::Kernel::ErrorType::Internal);

	TParameterHandler<CIdentifier *> ip_pClassificationAlgorithm(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier));
	ip_pClassificationAlgorithm = &m_oSubClassifierAlgorithmIdentifier;
	TParameterHandler<uint64_t> ip_pClassCount(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	ip_pClassCount = m_ui32NumberOfClasses;

	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize),
		"Failed to run decision strategy algorithm",
		OpenViBE::Kernel::ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(
		this->uninitializeExtraParameterMechanism(),
		"Failed to uninitialize extra parameters",
		OpenViBE::Kernel::ErrorType::Internal);

	//Calculate the amount of sample for each class
	std::map<double, uint32_t> l_vClassLabels;
	for (uint32_t i = 0; i < rFeatureVectorSet.getFeatureVectorCount(); i++)
	{
		if (!l_vClassLabels.count(rFeatureVectorSet[i].getLabel()))
		{
			l_vClassLabels[rFeatureVectorSet[i].getLabel()] = 0;
		}
		l_vClassLabels[rFeatureVectorSet[i].getLabel()]++;
	}

	OV_ERROR_UNLESS_KRF(
		l_vClassLabels.size() == m_ui32NumberOfClasses,
		"There are samples for " << static_cast<uint32_t>(l_vClassLabels.size()) << " classes but expected samples for " << m_ui32NumberOfClasses << " classes.",
		OpenViBE::Kernel::ErrorType::BadConfig);

	//Now we create the corresponding repartition set
	TParameterHandler<IMatrix*> ip_pRepartitionSet = m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	ip_pRepartitionSet->setDimensionCount(1);
	ip_pRepartitionSet->setDimensionSize(0, m_ui32NumberOfClasses);

	const uint32_t l_iFeatureVectorSize = rFeatureVectorSet[0].getSize();
	//Now let's train each classifier
	for (uint32_t l_iFirstClass = 0; l_iFirstClass < m_ui32NumberOfClasses; ++l_iFirstClass)
	{
		ip_pRepartitionSet->getBuffer()[l_iFirstClass] = static_cast<double>(l_vClassLabels[static_cast<double>(l_iFirstClass)]);

		for (uint32_t l_iSecondClass = l_iFirstClass + 1; l_iSecondClass < m_ui32NumberOfClasses; ++l_iSecondClass)
		{
			uint32_t l_iFeatureCount = l_vClassLabels[static_cast<double>(l_iFirstClass)] + l_vClassLabels[static_cast<double>(l_iSecondClass)];

			IAlgorithmProxy* l_pSubClassifier = m_oSubClassifiers[std::pair<uint32_t, uint32_t>(l_iFirstClass, l_iSecondClass)];

			TParameterHandler<IMatrix*> ip_pFeatureVectorSet(l_pSubClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));
			ip_pFeatureVectorSet->setDimensionCount(2);
			ip_pFeatureVectorSet->setDimensionSize(0, l_iFeatureCount);
			ip_pFeatureVectorSet->setDimensionSize(1, l_iFeatureVectorSize + 1);

			double* l_pFeatureVectorSetBuffer = ip_pFeatureVectorSet->getBuffer();
			for (uint32_t j = 0; j < rFeatureVectorSet.getFeatureVectorCount(); j++)
			{
				const double l_f64TempClass = rFeatureVectorSet[j].getLabel();
				if (l_f64TempClass == static_cast<double>(l_iFirstClass) || l_f64TempClass == static_cast<double>(l_iSecondClass))
				{
					System::Memory::copy(l_pFeatureVectorSetBuffer, rFeatureVectorSet[j].getBuffer(), l_iFeatureVectorSize * sizeof(double));

					l_pFeatureVectorSetBuffer[l_iFeatureVectorSize] = static_cast<size_t>(l_f64TempClass) == l_iFirstClass ? 0 : 1;
					l_pFeatureVectorSetBuffer += (l_iFeatureVectorSize + 1);
				}
			}

			OV_ERROR_UNLESS_KRF(
				l_pSubClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Train),
				"Failed to train subclassifier [1st class = " << static_cast<uint64_t>(l_iFirstClass) << ", 2nd class = " << static_cast<uint64_t>(l_iSecondClass) << "]",
				OpenViBE::Kernel::ErrorType::Internal);
		}
	}
	return true;
}

bool CAlgorithmClassifierOneVsOne::classify(const IFeatureVector& rFeatureVector, double& rf64Class, IVector& rClassificationValues, IVector& rProbabilityValue)
{
	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm,
		"No decision strategy algorithm set",
		OpenViBE::Kernel::ErrorType::BadConfig);

	const uint32_t l_ui32FeatureVectorSize = rFeatureVector.getSize();
	std::vector<SClassificationInfo> l_oClassificationList;

	TParameterHandler<IMatrix*> ip_pProbabilityMatrix = m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_InputParameter_ProbabilityMatrix);
	IMatrix* l_pProbabilityMatrix                     = (IMatrix*)ip_pProbabilityMatrix;

	l_pProbabilityMatrix->setDimensionCount(2);
	l_pProbabilityMatrix->setDimensionSize(0, m_ui32NumberOfClasses);
	l_pProbabilityMatrix->setDimensionSize(1, m_ui32NumberOfClasses);

	for (uint32_t i = 0; i < l_pProbabilityMatrix->getBufferElementCount(); ++i)
	{
		l_pProbabilityMatrix->getBuffer()[i] = 0.0;
	}

	//Let's generate the matrix of confidence score
	for (uint32_t i = 0; i < m_ui32NumberOfClasses; ++i)
	{
		for (uint32_t j = i + 1; j < m_ui32NumberOfClasses; ++j)
		{
			IAlgorithmProxy* l_pTempProxy = m_oSubClassifiers[std::pair<uint32_t, uint32_t>(i, j)];
			TParameterHandler<IMatrix*> ip_pFeatureVector(l_pTempProxy->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector));
			TParameterHandler<IMatrix*> op_pClassificationValues(l_pTempProxy->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
			TParameterHandler<double> op_pClassificationLabel(l_pTempProxy->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));
			ip_pFeatureVector->setDimensionCount(1);
			ip_pFeatureVector->setDimensionSize(0, l_ui32FeatureVectorSize);

			double* l_pFeatureVectorBuffer = ip_pFeatureVector->getBuffer();
			System::Memory::copy(l_pFeatureVectorBuffer, rFeatureVector.getBuffer(), l_ui32FeatureVectorSize * sizeof(double));
			l_pTempProxy->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify);

			SClassificationInfo l_oClassificationInfo = { (double)i, (double)j, op_pClassificationLabel, op_pClassificationValues };
			l_oClassificationList.push_back(l_oClassificationInfo);
		}
	}

	//	for(size_t i =0 ; i < l_ui32ClassCount ; ++i )
	//	{
	//		for(size_t j = 0; j < l_ui32ClassCount ; ++j)
	//		{
	//			std::cout << l_pProbabilityMatrix->getBuffer()[i*l_ui32ClassCount + j] << " " ;
	//		}
	//		std::cout << std::endl;
	//	}
	//	std::cout << std::endl;

	TParameterHandler<std::vector<SClassificationInfo> *> ip_pClassificationInfos(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs));
	ip_pClassificationInfos = &l_oClassificationList;

	//Then ask to the strategy to make the decision
	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute),
		"Failed to compute decision strategy",
		OpenViBE::Kernel::ErrorType::Internal);

	TParameterHandler<IMatrix*> op_pProbabilityVector = m_pDecisionStrategyAlgorithm->getOutputParameter(OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector);
	double l_f64MaxProb                               = -1;
	int32_t l_i32IndexSelectedClass                   = -1;

	rClassificationValues.setSize(0);
	rProbabilityValue.setSize(m_ui32NumberOfClasses);

	//We just have to take the most relevant now.
	for (uint32_t i = 0; i < m_ui32NumberOfClasses; ++i)
	{
		const double l_f64TempProb = op_pProbabilityVector->getBuffer()[i];
		if (l_f64TempProb > l_f64MaxProb)
		{
			l_i32IndexSelectedClass = i;
			l_f64MaxProb            = l_f64TempProb;
		}
		rProbabilityValue[i] = l_f64TempProb;
	}

	rf64Class = static_cast<double>(l_i32IndexSelectedClass);
	return true;
}

bool CAlgorithmClassifierOneVsOne::createSubClassifiers()
{
	// Clear any previous ones
	for (auto& kv : m_oSubClassifiers)
	{
		IAlgorithmProxy* l_pSubClassifier = kv.second;
		l_pSubClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*l_pSubClassifier);
	}
	this->m_oSubClassifiers.clear();

	//Now let's instantiate all the sub classifiers
	for (uint32_t l_iFirstClass = 0; l_iFirstClass < m_ui32NumberOfClasses; ++l_iFirstClass)
	{
		for (uint32_t l_iSecondClass = l_iFirstClass + 1; l_iSecondClass < m_ui32NumberOfClasses; ++l_iSecondClass)
		{
			const CIdentifier l_oSubClassifierAlgorithm = this->getAlgorithmManager().createAlgorithm(this->m_oSubClassifierAlgorithmIdentifier);

			OV_ERROR_UNLESS_KRF(
				l_oSubClassifierAlgorithm != OV_UndefinedIdentifier,
				"Unable to instantiate classifier for class [" << this->m_oSubClassifierAlgorithmIdentifier.toString() << "]",
				OpenViBE::Kernel::ErrorType::BadConfig);

			IAlgorithmProxy* l_pSubClassifier = &this->getAlgorithmManager().getAlgorithm(l_oSubClassifierAlgorithm);
			l_pSubClassifier->initialize();

			TParameterHandler<uint64_t> ip_pNumberOfClasses(l_pSubClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));
			ip_pNumberOfClasses = 2;

			//Set a references to the extra parameters input of the pairing strategy
			TParameterHandler<std::map<CString, CString>*> ip_pExtraParameters(l_pSubClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));
			ip_pExtraParameters.setReferenceTarget(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));

			m_oSubClassifiers[std::pair<uint32_t, uint32_t>(l_iFirstClass, l_iSecondClass)] = l_pSubClassifier;
		}
	}
	return true;
}

bool CAlgorithmClassifierOneVsOne::designArchitecture(const CIdentifier& rId, uint32_t rClassCount)
{
	if (!setSubClassifierIdentifier(rId)) { return false; }

	m_ui32NumberOfClasses = rClassCount;


	return true;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::getClassifierConfiguration(double f64FirstClass, double f64SecondClass, IAlgorithmProxy* pSubClassifier)
{
	XML::IXMLNode* l_pRes = XML::createNode(c_sSubClassifierNodeName);

	std::stringstream l_sFirstClass, l_sSecondClass;
	l_sFirstClass << f64FirstClass;
	l_sSecondClass << f64SecondClass;
	l_pRes->addAttribute(c_sFirstClassAtrributeName, l_sFirstClass.str().c_str());
	l_pRes->addAttribute(c_sSecondClassAttributeName, l_sSecondClass.str().c_str());

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(pSubClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	pSubClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration);
	l_pRes->addChild((XML::IXMLNode*)op_pConfiguration);

	return l_pRes;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::getPairwiseDecisionConfiguration()
{
	if (!m_pDecisionStrategyAlgorithm) { return nullptr; }

	XML::IXMLNode* l_pTempNode = XML::createNode(c_sPairwiseDecisionName);

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(m_pDecisionStrategyAlgorithm->getOutputParameter(OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Configuration));
	m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfiguration);
	l_pTempNode->addChild((XML::IXMLNode*)op_pConfiguration);

	l_pTempNode->addAttribute(c_sAlgorithmIdAttribute, m_oPairwiseDecisionIdentifier.toString());

	return l_pTempNode;
}

XML::IXMLNode* CAlgorithmClassifierOneVsOne::saveConfiguration()
{
	std::stringstream l_sClassifierCount;
	l_sClassifierCount << m_ui32NumberOfSubClassifiers;

	XML::IXMLNode* l_pOneVsOneNode = XML::createNode(c_sTypeNodeName);

	XML::IXMLNode* l_pTempNode = XML::createNode(c_sSubClassifierIdentifierNodeName);
	l_pTempNode->addAttribute(c_sAlgorithmIdAttribute, this->m_oSubClassifierAlgorithmIdentifier.toString());
	l_pTempNode->setPCData(this->getTypeManager().getEnumerationEntryNameFromValue(OVTK_TypeId_ClassificationAlgorithm, m_oSubClassifierAlgorithmIdentifier.toUInteger()).toASCIIString());
	l_pOneVsOneNode->addChild(l_pTempNode);

	l_pTempNode = XML::createNode(c_sSubClassifierCountNodeName);
	l_pTempNode->setPCData(l_sClassifierCount.str().c_str());
	l_pOneVsOneNode->addChild(l_pTempNode);

	l_pOneVsOneNode->addChild(this->getPairwiseDecisionConfiguration());

	XML::IXMLNode* l_pSubClassifersNode = XML::createNode(c_sSubClassifiersNodeName);

	for (auto& kv : m_oSubClassifiers)
	{
		l_pSubClassifersNode->addChild(getClassifierConfiguration(kv.first.first, kv.first.second, kv.second));
	}
	l_pOneVsOneNode->addChild(l_pSubClassifersNode);

	return l_pOneVsOneNode;
}

bool CAlgorithmClassifierOneVsOne::loadConfiguration(XML::IXMLNode* pConfigurationNode)
{
	XML::IXMLNode* l_pTempNode = pConfigurationNode->getChildByName(c_sSubClassifierIdentifierNodeName);

	CIdentifier l_pAlgorithmIdentifier;
	l_pAlgorithmIdentifier.fromString(l_pTempNode->getAttribute(c_sAlgorithmIdAttribute));

	if (!this->setSubClassifierIdentifier(l_pAlgorithmIdentifier))
	{
		//if the sub classifier doesn't have comparison function it is an error
		return false;
	}

	l_pTempNode = pConfigurationNode->getChildByName(c_sPairwiseDecisionName);
	CIdentifier l_pPairwiseIdentifier;
	l_pPairwiseIdentifier.fromString(l_pTempNode->getAttribute(c_sAlgorithmIdAttribute));
	if (l_pPairwiseIdentifier != m_oPairwiseDecisionIdentifier)
	{
		if (m_pDecisionStrategyAlgorithm != nullptr)
		{
			m_pDecisionStrategyAlgorithm->uninitialize();
			this->getAlgorithmManager().releaseAlgorithm(*m_pDecisionStrategyAlgorithm);
			m_pDecisionStrategyAlgorithm = nullptr;
		}
		m_oPairwiseDecisionIdentifier = l_pPairwiseIdentifier;
		m_pDecisionStrategyAlgorithm  = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(m_oPairwiseDecisionIdentifier));
		m_pDecisionStrategyAlgorithm->initialize();
	}
	TParameterHandler<XML::IXMLNode*> ip_pConfiguration(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_Configuration));
	ip_pConfiguration = l_pTempNode->getChild(0);

	TParameterHandler<CIdentifier *> ip_pClassificationAlgorithm(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier));
	ip_pClassificationAlgorithm = &l_pAlgorithmIdentifier;

	l_pTempNode = pConfigurationNode->getChildByName(c_sSubClassifierCountNodeName);
	std::stringstream l_sCountData(l_pTempNode->getPCData());
	l_sCountData >> m_ui32NumberOfSubClassifiers;

	// Invert the class count from subCls = numClass*(numClass-1)/2.
	const uint32_t l_ui32DeltaCarre = 1 + 8 * m_ui32NumberOfSubClassifiers;
	m_ui32NumberOfClasses           = static_cast<uint32_t>((1 + sqrt(static_cast<double>(l_ui32DeltaCarre))) / 2);

	TParameterHandler<uint64_t> ip_pClassCount(m_pDecisionStrategyAlgorithm->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	ip_pClassCount = m_ui32NumberOfClasses;

	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfiguration),
		"Loading decision strategy configuration failed",
		OpenViBE::Kernel::ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(
		m_pDecisionStrategyAlgorithm->process(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize),
		"Parameterizing decision strategy failed",
		OpenViBE::Kernel::ErrorType::Internal);

	return loadSubClassifierConfiguration(pConfigurationNode->getChildByName(c_sSubClassifiersNodeName));
}

uint32_t CAlgorithmClassifierOneVsOne::getOutputProbabilityVectorLength()
{
	return static_cast<uint32_t>(m_ui32NumberOfClasses);
}

uint32_t CAlgorithmClassifierOneVsOne::getOutputDistanceVectorLength() { return 0; }

bool CAlgorithmClassifierOneVsOne::loadSubClassifierConfiguration(XML::IXMLNode* pSubClassifiersNode)
{
	createSubClassifiers();

	for (uint32_t i = 0; i < pSubClassifiersNode->getChildCount(); ++i)
	{
		double l_f64FirstClass, l_f64SecondClass;

		//Now we have to restore class indexes
		XML::IXMLNode* l_pSubClassifierNode = pSubClassifiersNode->getChild(i);
		std::stringstream l_sFirstClass(l_pSubClassifierNode->getAttribute(c_sFirstClassAtrributeName));
		l_sFirstClass >> l_f64FirstClass;
		std::stringstream l_sSecondClass(l_pSubClassifierNode->getAttribute(c_sSecondClassAttributeName));
		l_sSecondClass >> l_f64SecondClass;

		IAlgorithmProxy* l_pSubClassifier = m_oSubClassifiers[std::make_pair(static_cast<uint32_t>(l_f64FirstClass), static_cast<uint32_t>(l_f64SecondClass))];

		TParameterHandler<XML::IXMLNode*> ip_pConfiguration(l_pSubClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
		ip_pConfiguration = l_pSubClassifierNode->getChild(0);

		OV_ERROR_UNLESS_KRF(
			l_pSubClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration),
			"Unable to load the configuration for the sub-classifier " << static_cast<uint64_t>(i+1),
			OpenViBE::Kernel::ErrorType::Internal);
	}

	OV_ERROR_UNLESS_KRF(
		m_oSubClassifiers.size() == m_ui32NumberOfSubClassifiers,
		"Invalid number of loaded classifiers [" << static_cast<uint64_t>(m_oSubClassifiers.size()) << "] (expected = " << m_ui32NumberOfSubClassifiers << ")",
		OpenViBE::Kernel::ErrorType::Internal);

	return true;
}

bool CAlgorithmClassifierOneVsOne::setSubClassifierIdentifier(const CIdentifier& rId)
{
	m_oSubClassifierAlgorithmIdentifier = rId;
	m_fAlgorithmComparison              = getClassificationComparisonFunction(rId);

	OV_ERROR_UNLESS_KRF(
		m_fAlgorithmComparison != nullptr,
		"No comparison function found for classifier " << m_oSubClassifierAlgorithmIdentifier.toString(),
		OpenViBE::Kernel::ErrorType::ResourceNotFound);

	return true;
}
