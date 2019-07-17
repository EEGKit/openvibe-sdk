#include "ovtkCAlgorithmClassifier.h"
#include "ovtkCFeatureVector.hpp"
#include "ovtkCFeatureVectorSet.hpp"
#include "ovtkCVector.hpp"

#include <xml/IXMLHandler.h>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEToolkit;

bool CAlgorithmClassifier::initialize()
{
	m_AlgorithmProxy     = NULL;
	m_ExtraParametersMap = NULL;
	return true;
}

bool CAlgorithmClassifier::uninitialize()
{
	if (m_AlgorithmProxy != NULL)
	{
		m_AlgorithmProxy->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_AlgorithmProxy);
		m_AlgorithmProxy = NULL;
	}
	return true;
}

bool CAlgorithmClassifier::process(void)
{
	TParameterHandler<IMatrix*> ip_FeatureVector(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector));
	TParameterHandler<IMatrix*> ip_FeatureVectorSet(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));
	TParameterHandler<XML::IXMLNode*> ip_Configuration(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));

	TParameterHandler<double> op_EstimatedClass(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));
	TParameterHandler<IMatrix*> op_ClassificationValues(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));
	TParameterHandler<IMatrix*> op_ProbabilityValues(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
	TParameterHandler<XML::IXMLNode*> op_Configuration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));

	if (this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_Train))
	{
		IMatrix* featureVectorSet = ip_FeatureVectorSet;
		if (!featureVectorSet)
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Feature vector set is NULL", OpenViBE::Kernel::ErrorType::BadInput);
		}
		else
		{
			CFeatureVectorSet featureVectorSetAdapter(*featureVectorSet);
			if (this->train(featureVectorSetAdapter))
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
				OV_ERROR_KRF("Training failed", OpenViBE::Kernel::ErrorType::Internal);
			}
		}
	}

	if (this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_Classify))
	{
		IMatrix* featureVector        = ip_FeatureVector;
		IMatrix* classificationValues = op_ClassificationValues;
		IMatrix* probabilityValues    = op_ProbabilityValues;

		if (!featureVector || !classificationValues || !probabilityValues)
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Classifying failed", (!featureVector) ? OpenViBE::Kernel::ErrorType::BadInput : OpenViBE::Kernel::ErrorType::BadOutput);
		}
		else
		{
			double estimatedClass = 0;
			CFeatureVector featureVectorAdapter(*featureVector);
			CVector classificationValuesAdapter(*classificationValues);
			CVector probabilityValuesAdapter(*probabilityValues);

			if (this->classify(featureVectorAdapter, estimatedClass, classificationValuesAdapter, probabilityValuesAdapter))
			{
				op_EstimatedClass = estimatedClass;
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
				OV_ERROR_KRF("Classifying failed", OpenViBE::Kernel::ErrorType::Internal);
			}
		}
	}

	if (this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration))
	{
		XML::IXMLNode* rootNode = this->saveConfiguration();
		op_Configuration        = rootNode;
		if (rootNode)
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
		}
		else
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Saving configuration failed", OpenViBE::Kernel::ErrorType::Internal);
		}
	}

	if (this->isInputTriggerActive(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration))
	{
		XML::IXMLNode* rootNode = ip_Configuration;
		if (!rootNode)
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Configuration XML node is NULL", OpenViBE::Kernel::ErrorType::BadInput);
		}
		else
		{
			if (this->loadConfiguration(rootNode))
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
				//Now we need to parametrize the two output Matrix for values
				setMatrixOutputDimension(op_ProbabilityValues, this->getOutputProbabilityVectorLength());
				setMatrixOutputDimension(op_ClassificationValues, this->getOutputDistanceVectorLength());
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
				OV_ERROR_KRF("Loading configuration failed", OpenViBE::Kernel::ErrorType::Internal);
			}
		}
	}

	return true;
}

bool CAlgorithmClassifier::initializeExtraParameterMechanism()
{
	TParameterHandler<std::map<CString, CString>*> ip_ExtraParameter(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));
	m_ExtraParametersMap = static_cast<std::map<CString, CString>*>(ip_ExtraParameter);

	m_AlgorithmProxy = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(this->getClassIdentifier()));

	OV_ERROR_UNLESS_KRF(
		m_AlgorithmProxy->initialize(),
		"Failed to initialize algorithm",
		OpenViBE::Kernel::ErrorType::Internal
	);

	return true;
}

bool CAlgorithmClassifier::uninitializeExtraParameterMechanism()
{
	OV_ERROR_UNLESS_KRF(
		m_AlgorithmProxy->uninitialize(),
		"Failed to uninitialize algorithm",
		OpenViBE::Kernel::ErrorType::Internal
	);

	this->getAlgorithmManager().releaseAlgorithm(*m_AlgorithmProxy);

	m_AlgorithmProxy     = NULL;
	m_ExtraParametersMap = NULL;
	return true;
}

CString& CAlgorithmClassifier::getParameterValue(const CIdentifier& parameterIdentifier)
{
	CString parameterName = m_AlgorithmProxy->getInputParameterName(parameterIdentifier);
	return (*static_cast<std::map<CString, CString>*>(m_ExtraParametersMap))[parameterName];
}

void CAlgorithmClassifier::setMatrixOutputDimension(TParameterHandler<IMatrix*>& matrix, uint32 length)
{
	matrix->setDimensionCount(1);
	matrix->setDimensionSize(0, length);
}

uint64 CAlgorithmClassifier::getUInt64Parameter(const CIdentifier& parameterIdentifier)
{
	TParameterHandler<uint64> temp(getInputParameter(parameterIdentifier));
	temp = this->getAlgorithmContext().getConfigurationManager().expandAsUInteger(getParameterValue(parameterIdentifier));
	return static_cast<uint64>(temp);
}

int64 CAlgorithmClassifier::getInt64Parameter(const CIdentifier& parameterIdentifier)
{
	TParameterHandler<int64> temp(getInputParameter(parameterIdentifier));
	temp = this->getAlgorithmContext().getConfigurationManager().expandAsInteger(getParameterValue(parameterIdentifier));
	return static_cast<int64>(temp);
}

double CAlgorithmClassifier::getFloat64Parameter(const CIdentifier& parameterIdentifier)
{
	TParameterHandler<double> temp(getInputParameter(parameterIdentifier));
	temp = this->getAlgorithmContext().getConfigurationManager().expandAsFloat(getParameterValue(parameterIdentifier));
	return static_cast<double>(temp);
}

bool CAlgorithmClassifier::getBooleanParameter(const CIdentifier& parameterIdentifier)
{
	TParameterHandler<bool> temp(getInputParameter(parameterIdentifier));
	temp = this->getAlgorithmContext().getConfigurationManager().expandAsBoolean(getParameterValue(parameterIdentifier));
	return static_cast<bool>(temp);
}

CString* CAlgorithmClassifier::getCStringParameter(const CIdentifier& parameterIdentifier)
{
	TParameterHandler<CString*> temp(getInputParameter(parameterIdentifier));
	temp = &getParameterValue(parameterIdentifier);
	return static_cast<CString*>(temp);
}

uint64 CAlgorithmClassifier::getEnumerationParameter(const CIdentifier& parameterIdentifier, const CIdentifier& enumerationIdentifier)
{
	TParameterHandler<uint64> temp(getInputParameter(parameterIdentifier));
	temp = this->getTypeManager().getEnumerationEntryValueFromName(enumerationIdentifier, getParameterValue(parameterIdentifier));
	return static_cast<uint64>(temp);
}
