#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>
#include "ovpCAlgorithmClassifierOneVsOne.h"

namespace OpenViBEPlugins
{
	namespace Classification
	{
		/**
		 * @brief The CAlgorithmPairwiseDecision class
		 * This is the default class for every decision usable with the One Vs One pairwise strategy.
		 */
		class CAlgorithmPairwiseDecision : virtual public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override = 0;
			bool uninitialize() override = 0;

			virtual bool parameterize() = 0;

			virtual bool compute(std::vector<SClassificationInfo>& pClassificationValueList, OpenViBE::IMatrix* pProbabilityVector) = 0;
			virtual XML::IXMLNode* saveConfiguration() = 0;
			virtual bool loadConfiguration(XML::IXMLNode& rNode) = 0;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_PairwiseDecision)
		};

		class CAlgorithmPairwiseDecisionDesc : virtual public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputParameter(
					OVP_Algorithm_Classifier_InputParameter_ProbabilityMatrix, "Probability Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addInputParameter(
					OVP_Algorithm_Classifier_Pairwise_InputParameterId_Configuration, "Configuration node", OpenViBE::Kernel::ParameterType_Pointer);
				prototype.addInputParameter(
					OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition, "Set repartition", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addInputParameter(
					OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier, "Classification Algorithm",
					OpenViBE::Kernel::ParameterType_Identifier);
				prototype.addInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs,
													  "Classification Outputs", OpenViBE::Kernel::ParameterType_Pointer);
				prototype.addInputParameter(
					OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount, "Class Count", OpenViBE::Kernel::ParameterType_UInteger);


				prototype.addOutputParameter(
					OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector, "Probability Vector", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addOutputParameter(
					OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Configuration, "Configuration node", OpenViBE::Kernel::ParameterType_Pointer);


				prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute, "Compute");
				prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize, "Parametrize");
				prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfiguration, "Save configuration");
				prototype.addInputTrigger(OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfiguration, "Load configuration");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_PairwiseDecisionDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
