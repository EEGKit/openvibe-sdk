#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCAlgorithmPairwiseDecision.h"

namespace OpenViBEPlugins
{
	namespace Classification
	{
		/**
		 * @brief The CAlgorithmPairwiseDecisionVoting class
		 * This strategy relies on a basic voting system. If class A beats class B, class A win 1 point and B 0 point. At the end, the vector of
		 * probability is composed by the normalized score of each class.
		 *
		 * Probability required.
		 */
		class CAlgorithmPairwiseDecisionVoting final : virtual public CAlgorithmPairwiseDecision
		{
		public:

			CAlgorithmPairwiseDecisionVoting() { }
			void release() override { delete this; }
			bool initialize() override { return true; }
			bool uninitialize() override { return true; }
			bool parameterize() override;
			bool compute(std::vector<SClassificationInfo>& pClassificationValueList, OpenViBE::IMatrix* pProbabilityVector) override;
			XML::IXMLNode* saveConfiguration() override;
			bool loadConfiguration(XML::IXMLNode& /*node*/) override { return true; }

			_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecision, OVP_ClassId_Algorithm_PairwiseDecision_Voting)

		private:
			uint32_t m_nClass = 0;
		};

		class CAlgorithmPairwiseDecisionVotingDesc final : virtual public CAlgorithmPairwiseDecisionDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Pairwise decision strategy based on Voting"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Serrière Guillaume"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_PairwiseDecision_Voting; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmPairwiseDecisionVoting; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecisionDesc, OVP_ClassId_Algorithm_PairwiseDecision_VotingDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
