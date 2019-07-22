#include "ovpCAlgorithmPairwiseDecision.h"

#include <iostream>
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

bool CAlgorithmPairwiseDecision::process()
{
	// @note there is essentially no test that these are called in correct order. Caller be careful!
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute))
	{
		TParameterHandler<std::vector<SClassificationInfo> *> ip_pClassificationValues = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs);
		TParameterHandler<IMatrix*> op_pProbabilityVector                              = this->getOutputParameter(OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector);
		return this->compute(*((std::vector<SClassificationInfo> *)ip_pClassificationValues), (IMatrix*)op_pProbabilityVector);
	}
	else if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfiguration))
	{
		TParameterHandler<XML::IXMLNode*> op_pConfiguration(this->getOutputParameter(OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Configuration));
		XML::IXMLNode* l_pTempNode = this->saveConfiguration();

		OV_ERROR_UNLESS_KRF(
			l_pTempNode != NULL,
			"Invalid NULL xml node returned while saving configuration",
			OpenViBE::Kernel::ErrorType::Internal);

		op_pConfiguration = l_pTempNode;
		return true;
	}
	else if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfiguration))
	{
		TParameterHandler<XML::IXMLNode*> op_pConfiguration(this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_Configuration));
		XML::IXMLNode* l_pTempNode = (XML::IXMLNode*)op_pConfiguration;

		OV_ERROR_UNLESS_KRF(
			l_pTempNode != NULL,
			"Invalid NULL xml node to load configuration in",
			OpenViBE::Kernel::ErrorType::BadInput);

		return this->loadConfiguration(*l_pTempNode);
	}
	else if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize))
	{
		return this->parameterize();
	}

	return true;
}
