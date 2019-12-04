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
		TParameterHandler<std::vector<classification_info_t> *> ip_classifications = this->getInputParameter(
			OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs);
		TParameterHandler<IMatrix*> op_probability = this->getOutputParameter(OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector);
		return this->compute(*static_cast<std::vector<classification_info_t> *>(ip_classifications), static_cast<IMatrix*>(op_probability));
	}
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfig))
	{
		TParameterHandler<XML::IXMLNode*> op_pConfiguration(this->getOutputParameter(OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Config));
		XML::IXMLNode* tmp = this->saveConfig();

		OV_ERROR_UNLESS_KRF(tmp != nullptr, "Invalid NULL xml node returned while saving configuration", OpenViBE::Kernel::ErrorType::Internal);

		op_pConfiguration = tmp;
		return true;
	}
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfig))
	{
		TParameterHandler<XML::IXMLNode*> op_config(this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_Config));
		XML::IXMLNode* tmp = static_cast<XML::IXMLNode*>(op_config);

		OV_ERROR_UNLESS_KRF(tmp != nullptr, "Invalid NULL xml node to load configuration in", OpenViBE::Kernel::ErrorType::BadInput);

		return this->loadConfig(*tmp);
	}
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize)) { return this->parameterize(); }

	return true;
}
