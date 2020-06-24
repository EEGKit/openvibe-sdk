#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>
#include "ovpCAlgorithmClassifierOneVsOne.h"

namespace OpenViBE {
namespace Plugins {
namespace Classification {
/**
 * @brief The CAlgorithmPairwiseDecision class
 * This is the default class for every decision usable with the One Vs One pairwise strategy.
 */
class CAlgorithmPairwiseDecision : virtual public Toolkit::TAlgorithm<IAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override = 0;
	bool uninitialize() override = 0;

	virtual bool parameterize() = 0;

	virtual bool compute(std::vector<classification_info_t>& classifications, CMatrix* probabilities) = 0;
	virtual XML::IXMLNode* saveConfig() = 0;
	virtual bool loadConfig(XML::IXMLNode& node) = 0;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TAlgorithm<IAlgorithm>, OVP_ClassId_Algorithm_PairwiseDecision)
};

class CAlgorithmPairwiseDecisionDesc : virtual public IAlgorithmDesc
{
public:
	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		prototype.addInputParameter(OVP_Algorithm_Classifier_InputParameter_ProbabilityMatrix, "Probability Matrix", Kernel::ParameterType_Matrix);
		prototype.addInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_Config, "Configuration node", Kernel::ParameterType_Pointer);
		prototype.addInputParameter(
			OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition, "Set repartition", Kernel::ParameterType_Matrix);
		prototype.addInputParameter(
			OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier, "Classification Algorithm", Kernel::ParameterType_Identifier);
		prototype.addInputParameter(
			OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs, "Classification Outputs", Kernel::ParameterType_Pointer);
		prototype.addInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount, "Class Count", Kernel::ParameterType_UInteger);

		prototype.addOutputParameter(
			OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector, "Probability Vector", Kernel::ParameterType_Matrix);
		prototype.addOutputParameter(
			OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Config, "Configuration node", Kernel::ParameterType_Pointer);

		prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute, "Compute");
		prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize, "Parametrize");
		prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfig, "Save configuration");
		prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfig, "Load configuration");
		return true;
	}

	_IsDerivedFromClass_Final_(IAlgorithmDesc, OVP_ClassId_Algorithm_PairwiseDecisionDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
