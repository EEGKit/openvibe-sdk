#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCAlgorithmPairwiseDecision.h"

namespace OpenViBE {
namespace Plugins {
namespace Classification {
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
	bool compute(std::vector<classification_info_t>& classifications, IMatrix* probabilities) override;
	XML::IXMLNode* saveConfig() override;
	bool loadConfig(XML::IXMLNode& /*node*/) override { return true; }

	_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecision, OVP_ClassId_Algorithm_PairwiseDecision_Voting)

private:
	size_t m_nClass = 0;
};

class CAlgorithmPairwiseDecisionVotingDesc final : virtual public CAlgorithmPairwiseDecisionDesc
{
public:
	void release() override { }
	CString getName() const override { return "Pairwise decision strategy based on Voting"; }
	CString getAuthorName() const override { return "Serrière Guillaume"; }
	CString getAuthorCompanyName() const override { return "Inria"; }
	CString getShortDescription() const override { return "."; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return ""; }
	CString getVersion() const override { return "0.1"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_PairwiseDecision_Voting; }
	IPluginObject* create() override { return new CAlgorithmPairwiseDecisionVoting; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(prototype);
		return true;
	}

	_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecisionDesc, OVP_ClassId_Algorithm_PairwiseDecision_VotingDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
