#include "ovpCAlgorithmPairwiseDecision.h"

#include <iostream>
namespace OpenViBE {
namespace Plugins {
namespace Classification {


bool CAlgorithmPairwiseDecision::process()
{
	// @note there is essentially no test that these are called in correct order. Caller be careful!
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute))
	{
		Kernel::TParameterHandler<std::vector<classification_info_t>*> ip_classifications = this->getInputParameter(
			OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs);
		Kernel::TParameterHandler<CMatrix*> op_probability = this->getOutputParameter(OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector);
		return this->compute(*static_cast<std::vector<classification_info_t>*>(ip_classifications), static_cast<CMatrix*>(op_probability));
	}
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfig))
	{
		Kernel::TParameterHandler<XML::IXMLNode*> op_configuration(this->getOutputParameter(OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Config));
		XML::IXMLNode* tmp = this->saveConfig();

		OV_ERROR_UNLESS_KRF(tmp != nullptr, "Invalid NULL xml node returned while saving configuration", Kernel::ErrorType::Internal);

		op_configuration = tmp;
		return true;
	}
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfig))
	{
		Kernel::TParameterHandler<XML::IXMLNode*> op_config(this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_Config));
		XML::IXMLNode* tmp = static_cast<XML::IXMLNode*>(op_config);

		OV_ERROR_UNLESS_KRF(tmp != nullptr, "Invalid NULL xml node to load configuration in", Kernel::ErrorType::BadInput);

		return this->loadConfig(*tmp);
	}
	if (this->isInputTriggerActive(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize)) { return this->parameterize(); }

	return true;
}

}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
