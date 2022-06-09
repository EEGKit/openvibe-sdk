#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCAlgorithmPairwiseDecision.h"

namespace OpenViBE {
namespace Plugins {
namespace Classification {
/**
 * @brief The CAlgorithmPairwiseStrategyPKPD class
 * This strategy relies on the algorithm describe in the article . Price, S. Knerr, L. Personnaz, and G. Dreyfus.
 * Pairwise neural network classifiers with probabilistic outputs. In G. Tesauro, D. Touretzky, and T. Leen (eds.)
 * Advances in Neural Information Processing Systems 7 (NIPS-94), pp. 1109-1116. MIT Press, 1995.
 */
class CAlgorithmPairwiseStrategyPKPD final : virtual public CAlgorithmPairwiseDecision
{
public:
	CAlgorithmPairwiseStrategyPKPD() { }
	void release() override { delete this; }
	bool initialize() override { return true; }
	bool uninitialize() override { return true; }
	bool parameterize() override;
	bool compute(std::vector<classification_info_t>& classifications, CMatrix* probabilities) override;
	XML::IXMLNode* saveConfig() override;
	bool loadConfig(XML::IXMLNode& /*node*/) override { return true; }

	_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecision, OVP_ClassId_Algorithm_PairwiseStrategy_PKPD)

private:
	size_t m_nClass = 0;
};

class CAlgorithmPairwiseStrategyPKPDDesc final : virtual public CAlgorithmPairwiseDecisionDesc
{
public:
	void release() override { }

	CString getName() const override { return "Pairwise decision strategy based on PKPD"; }
	CString getAuthorName() const override { return "Serrière Guillaume"; }
	CString getAuthorCompanyName() const override { return "Inria"; }
	CString getShortDescription() const override { return "."; }

	CString getDetailedDescription() const override
	{
		return "Price, S. Knerr, L. Personnaz, and G. Dreyfus."
				"Pairwise neural network classifiers with probabilistic outputs."
				" In G. Tesauro, D. Touretzky, and T. Leen (eds.)"
				"Advances in Neural Information Processing Systems 7 (NIPS-94), pp."
				" 1109-1116. MIT Press, 1995.";
	}

	CString getCategory() const override { return ""; }
	CString getVersion() const override { return "0.1"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_PairwiseStrategy_PKPD; }
	IPluginObject* create() override { return new CAlgorithmPairwiseStrategyPKPD; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(prototype);
		return true;
	}

	_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecisionDesc, OVP_ClassId_Algorithm_PairwiseStrategy_PKPDDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
