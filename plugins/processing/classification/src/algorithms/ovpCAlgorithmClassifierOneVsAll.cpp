#include "ovpCAlgorithmClassifierOneVsAll.h"

#include <map>
#include <sstream>
#include <cstring>
#include <string>
#include <utility>
#include <iostream>
#include <system/ovCMemory.h>

namespace
{
	const char* const TYPE_NODE_NAME                      = "OneVsAll";
	const char* const SUB_CLASSIFIER_IDENTIFIER_NODE_NAME = "SubClassifierIdentifier";
	const char* const ALGORITHM_ID_ATTRIBUTE              = "algorithm-id";
	const char* const SUB_CLASSIFIER_COUNT_NODE_NAME      = "SubClassifierCount";
	const char* const SUB_CLASSIFIERS_NODE_NAME           = "SubClassifiers";
	//const char* const SUB_CLASSIFIER_NODE_NAME = "SubClassifier";
} // namespace

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

typedef std::pair<IMatrix*, IMatrix*> CIMatrixPointerPair;
typedef std::pair<double, IMatrix*> CClassifierOutput;

bool CAlgorithmClassifierOneVsAll::initialize()
{
	TParameterHandler<XML::IXMLNode*> op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	op_pConfiguration = nullptr;

	return CAlgorithmPairingStrategy::initialize();
}

bool CAlgorithmClassifierOneVsAll::uninitialize()
{
	while (!m_oSubClassifierList.empty()) { this->removeClassifierAtBack(); }
	return CAlgorithmPairingStrategy::uninitialize();
}

bool CAlgorithmClassifierOneVsAll::train(const IFeatureVectorSet& featureVectorSet)
{
	const uint32_t nClass = uint32_t(m_oSubClassifierList.size());
	std::map<double, size_t> classLabels;

	for (uint32_t i = 0; i < featureVectorSet.getFeatureVectorCount(); i++)
	{
		if (!classLabels.count(featureVectorSet[i].getLabel())) { classLabels[featureVectorSet[i].getLabel()] = 0; }
		classLabels[featureVectorSet[i].getLabel()]++;
	}

	OV_ERROR_UNLESS_KRF(classLabels.size() == nClass,
						"Invalid samples count for [" << (uint32_t)classLabels.size() << "] classes (expected samples for " << nClass << " classes)",
						OpenViBE::Kernel::ErrorType::BadConfig);

	//We set the IMatrix fo the first classifier
	const uint32_t featureVectorSize = featureVectorSet[0].getSize();
	TParameterHandler<IMatrix*> featureVectorSetReference(m_oSubClassifierList[0]->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));
	featureVectorSetReference->setDimensionCount(2);
	featureVectorSetReference->setDimensionSize(0, featureVectorSet.getFeatureVectorCount());
	featureVectorSetReference->setDimensionSize(1, featureVectorSize + 1);

	double* featureVectorSetBuffer = featureVectorSetReference->getBuffer();
	for (uint32_t j = 0; j < featureVectorSet.getFeatureVectorCount(); j++)
	{
		System::Memory::copy(featureVectorSetBuffer, featureVectorSet[j].getBuffer(), featureVectorSize * sizeof(double));
		//We let the space for the label
		featureVectorSetBuffer += (featureVectorSize + 1);
	}

	//And then we just change adapt the label for each feature vector but we don't copy them anymore
	for (size_t classifierCounter = 0; classifierCounter < m_oSubClassifierList.size(); ++classifierCounter)
	{
		TParameterHandler<IMatrix*> ip_pFeatureVectorSet(m_oSubClassifierList[classifierCounter]->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));
		ip_pFeatureVectorSet = static_cast<IMatrix*>(featureVectorSetReference);

		featureVectorSetBuffer = ip_pFeatureVectorSet->getBuffer();
		for (uint32_t j = 0; j < featureVectorSet.getFeatureVectorCount(); j++)
		{
			//Modify the class of each featureVector
			const double classLabel = featureVectorSet[j].getLabel();
			if (size_t(classLabel) == classifierCounter) { featureVectorSetBuffer[featureVectorSize] = 0; }
			else { featureVectorSetBuffer[featureVectorSize] = 1; }
			featureVectorSetBuffer += (featureVectorSize + 1);
		}

		m_oSubClassifierList[classifierCounter]->process(OVTK_Algorithm_Classifier_InputTriggerId_Train);
	}
	return true;
}

bool CAlgorithmClassifierOneVsAll::classify(const IFeatureVector& featureVector, double& classId, IVector& distanceValue, IVector& probabilityValue)
{
	std::vector<CClassifierOutput> classification;

	const uint32_t featureVectorSize = featureVector.getSize();

	for (size_t i = 0; i < m_oSubClassifierList.size(); ++i)
	{
		IAlgorithmProxy* subClassifier = this->m_oSubClassifierList[i];
		TParameterHandler<IMatrix*> ip_pFeatureVector(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector));
		TParameterHandler<double> op_f64ClassificationStateClass(subClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));
		TParameterHandler<IMatrix*> op_pClassificationValues(subClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));
		TParameterHandler<IMatrix*> op_pProbabilityValues(subClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
		ip_pFeatureVector->setDimensionCount(1);
		ip_pFeatureVector->setDimensionSize(0, featureVectorSize);

		double* featureVectorBuffer = ip_pFeatureVector->getBuffer();
		System::Memory::copy(featureVectorBuffer, featureVector.getBuffer(), featureVectorSize * sizeof(double));
		subClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify);

		IMatrix* probabilities = static_cast<IMatrix*>(op_pProbabilityValues);
		//If the algorithm give a probability we take it, instead we take the first value
		if (probabilities->getDimensionCount() != 0)
		{
			classification.push_back(CClassifierOutput(double(op_f64ClassificationStateClass), probabilities));
		}
		else
		{
			classification.push_back(CClassifierOutput(double(op_f64ClassificationStateClass), static_cast<IMatrix*>(op_pClassificationValues)));
		}
		this->getLogManager() << LogLevel_Debug << uint64_t(i) << " " << double(op_f64ClassificationStateClass) << " " << double((*op_pProbabilityValues)[0]) << " " << double((*op_pProbabilityValues)[1]) << "\n";
	}

	//Now, we determine the best classification
	CClassifierOutput best = CClassifierOutput(-1.0, static_cast<IMatrix*>(nullptr));
	classId              = -1;

	for (size_t i = 0; i < classification.size(); ++i)
	{
		CClassifierOutput& l_pTemp = classification[i];
		if (int(l_pTemp.first) == 0)		// Predicts its "own" class, class=0
		{
			if (best.second == nullptr)
			{
				best   = l_pTemp;
				classId = double(i);
			}
			else
			{
				if ((*m_fAlgorithmComparison)((*best.second), *(l_pTemp.second)) > 0)
				{
					best   = l_pTemp;
					classId = double(i);
				}
			}
		}
	}

	//If no one recognize the class, let's take the more relevant
	if (int(classId) == -1)
	{
		this->getLogManager() << LogLevel_Debug << "Unable to find a class in first instance\n";
		for (uint32_t l_iClassificationCount = 0; l_iClassificationCount < classification.size(); ++l_iClassificationCount)
		{
			CClassifierOutput& tmp = classification[l_iClassificationCount];
			if (best.second == nullptr)
			{
				best   = tmp;
				classId = (double(l_iClassificationCount));
			}
			else
			{
				//We take the one that is the least like the second class
				if ((*m_fAlgorithmComparison)((*best.second), *(tmp.second)) < 0)
				{
					best   = tmp;
					classId = l_iClassificationCount;
				}
			}
		}
	}

	OV_ERROR_UNLESS_KRF(best.second != nullptr, "Unable to find a class for feature vector", OpenViBE::Kernel::ErrorType::BadProcessing);

	// Now that we made the calculation, we send the corresponding data

	// For distances we just send the distance vector of the winner
	IAlgorithmProxy* winner = this->m_oSubClassifierList[uint32_t(classId)];
	TParameterHandler<IMatrix*> op_pClassificationWinnerValues(winner->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));
	IMatrix* l_pTempMatrix = static_cast<IMatrix*>(op_pClassificationWinnerValues);
	distanceValue.setSize(l_pTempMatrix->getBufferElementCount());
	System::Memory::copy(distanceValue.getBuffer(), l_pTempMatrix->getBuffer(), l_pTempMatrix->getBufferElementCount() * sizeof(double));

	// We take the probabilities of the single class winning from each of the sub classifiers and normalize them
	double subProbabilitySum = 0;
	probabilityValue.setSize(uint32_t(m_oSubClassifierList.size()));
	for (uint32_t i = 0; i < m_oSubClassifierList.size(); ++i)
	{
		TParameterHandler<IMatrix*> op_ProbabilityValues(m_oSubClassifierList[i]->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
		probabilityValue[i] = op_ProbabilityValues->getBuffer()[0];
		subProbabilitySum += probabilityValue[i];
	}

	for (uint32_t i = 0; i < probabilityValue.getSize(); ++i)
	{
		probabilityValue[i] /= subProbabilitySum;
	}

	return true;
}

bool CAlgorithmClassifierOneVsAll::addNewClassifierAtBack()
{
	const CIdentifier subClassifierAlgorithm = this->getAlgorithmManager().createAlgorithm(this->m_oSubClassifierAlgorithmIdentifier);

	OV_ERROR_UNLESS_KRF(subClassifierAlgorithm != OV_UndefinedIdentifier,
						"Invalid classifier identifier [" << this->m_oSubClassifierAlgorithmIdentifier.toString() << "]",
						OpenViBE::Kernel::ErrorType::BadConfig);

	IAlgorithmProxy* subClassifier = &this->getAlgorithmManager().getAlgorithm(subClassifierAlgorithm);
	subClassifier->initialize();

	TParameterHandler<uint64_t> ip_pNumberOfClasses(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));
	ip_pNumberOfClasses = 2;

	//Set a references to the extra parameters input of the pairing strategy
	TParameterHandler<std::map<CString, CString>*> ip_pExtraParameters(subClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));
	ip_pExtraParameters.setReferenceTarget(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));

	this->m_oSubClassifierList.push_back(subClassifier);

	return true;
}

void CAlgorithmClassifierOneVsAll::removeClassifierAtBack()
{
	IAlgorithmProxy* subClassifier = m_oSubClassifierList.back();
	subClassifier->uninitialize();
	this->getAlgorithmManager().releaseAlgorithm(*subClassifier);
	this->m_oSubClassifierList.pop_back();
}

bool CAlgorithmClassifierOneVsAll::designArchitecture(const CIdentifier& id, uint32_t nClass)
{
	if (!this->setSubClassifierIdentifier(id)) { return false; }
	for (size_t i = 0; i < nClass; ++i)
	{
		if (!this->addNewClassifierAtBack()) { return false; }
	}
	return true;
}

XML::IXMLNode* CAlgorithmClassifierOneVsAll::getClassifierConfiguration(IAlgorithmProxy* classifier)
{
	TParameterHandler<XML::IXMLNode*> op_pConfiguration(classifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	classifier->process(OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration);
	XML::IXMLNode* l_pRes = op_pConfiguration;
	return l_pRes;
}

XML::IXMLNode* CAlgorithmClassifierOneVsAll::saveConfiguration()
{
	std::stringstream classCountes;
	classCountes << getClassCount();

	XML::IXMLNode* oneVsAllNode = XML::createNode(TYPE_NODE_NAME);

	XML::IXMLNode* tempNode = XML::createNode(SUB_CLASSIFIER_IDENTIFIER_NODE_NAME);
	tempNode->addAttribute(ALGORITHM_ID_ATTRIBUTE, this->m_oSubClassifierAlgorithmIdentifier.toString());
	tempNode->setPCData(this->getTypeManager().getEnumerationEntryNameFromValue(OVTK_TypeId_ClassificationAlgorithm, m_oSubClassifierAlgorithmIdentifier.toUInteger()).toASCIIString());
	oneVsAllNode->addChild(tempNode);

	tempNode = XML::createNode(SUB_CLASSIFIER_COUNT_NODE_NAME);
	tempNode->setPCData(classCountes.str().c_str());
	oneVsAllNode->addChild(tempNode);

	XML::IXMLNode* subClassifersNode = XML::createNode(SUB_CLASSIFIERS_NODE_NAME);

	//We now add configuration of each subclassifiers
	for (size_t i = 0; i < m_oSubClassifierList.size(); ++i) { subClassifersNode->addChild(getClassifierConfiguration(m_oSubClassifierList[i])); }
	oneVsAllNode->addChild(subClassifersNode);

	return oneVsAllNode;
}

bool CAlgorithmClassifierOneVsAll::loadConfiguration(XML::IXMLNode* configurationNode)
{
	XML::IXMLNode* tempNode = configurationNode->getChildByName(SUB_CLASSIFIER_IDENTIFIER_NODE_NAME);
	CIdentifier id;
	id.fromString(tempNode->getAttribute(ALGORITHM_ID_ATTRIBUTE));
	if (m_oSubClassifierAlgorithmIdentifier.toUInteger() != id)
	{
		while (!m_oSubClassifierList.empty()) { this->removeClassifierAtBack(); }
		if (!this->setSubClassifierIdentifier(id))
		{
			//if the sub classifier doesn't have comparison function it is an error
			return false;
		}
	}

	tempNode = configurationNode->getChildByName(SUB_CLASSIFIER_COUNT_NODE_NAME);
	std::stringstream countData(tempNode->getPCData());
	uint64_t nClass;
	countData >> nClass;

	while (nClass != getClassCount())
	{
		if (nClass < getClassCount()) { this->removeClassifierAtBack(); }
		else 
		{
			if (!this->addNewClassifierAtBack()) { return false; }
		}
	}

	return loadSubClassifierConfiguration(configurationNode->getChildByName(SUB_CLASSIFIERS_NODE_NAME));
}

uint32_t CAlgorithmClassifierOneVsAll::getOutputProbabilityVectorLength() { return uint32_t(m_oSubClassifierList.size()); }

uint32_t CAlgorithmClassifierOneVsAll::getOutputDistanceVectorLength()
{
	TParameterHandler<IMatrix*> op_pDistanceValues(m_oSubClassifierList[0]->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));
	return op_pDistanceValues->getDimensionSize(0);
}

bool CAlgorithmClassifierOneVsAll::loadSubClassifierConfiguration(XML::IXMLNode* subClassifiersNode)
{
	for (uint32_t i = 0; i < subClassifiersNode->getChildCount(); ++i)
	{
		XML::IXMLNode* subClassifierNode = subClassifiersNode->getChild(i);
		TParameterHandler<XML::IXMLNode*> ip_pConfiguration(m_oSubClassifierList[i]->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
		ip_pConfiguration = subClassifierNode;

		OV_ERROR_UNLESS_KRF(m_oSubClassifierList[i]->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration),
							"Unable to load the configuration of the classifier " << uint64_t(i+1),
							OpenViBE::Kernel::ErrorType::Internal);
	}
	return true;
}

uint32_t CAlgorithmClassifierOneVsAll::getClassCount() const { return uint32_t(m_oSubClassifierList.size()); }

bool CAlgorithmClassifierOneVsAll::setSubClassifierIdentifier(const CIdentifier& id)
{
	m_oSubClassifierAlgorithmIdentifier = id;
	m_fAlgorithmComparison              = getClassificationComparisonFunction(id);

	OV_ERROR_UNLESS_KRF(m_fAlgorithmComparison != nullptr, "No comparison function found for classifier [" << m_oSubClassifierAlgorithmIdentifier.toString() << "]", OpenViBE::Kernel::ErrorType::ResourceNotFound);

	return true;
}
