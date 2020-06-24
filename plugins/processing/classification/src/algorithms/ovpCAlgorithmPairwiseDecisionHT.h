#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>

#include "ovpCAlgorithmPairwiseDecision.h"

namespace OpenViBE {
namespace Plugins {
namespace Classification {	/**
		 * @brief The CAlgorithmPairwiseDecisionHT class is a decision strategy for the One Vs One pairwise decision that implement the
		 * method describe in the article Hastie, Trevor; Tibshirani, Robert. Classification by pairwise coupling. The Annals of Statistics 26 (1998), no. 2, 451--471
		 *
		 * Probability required
		 */
class CAlgorithmPairwiseDecisionHT final : virtual public CAlgorithmPairwiseDecision
{
public:

	CAlgorithmPairwiseDecisionHT() { }
	void release() override { delete this; }
	bool initialize() override { return true; }
	bool uninitialize() override { return true; }
	bool parameterize() override;
	bool compute(std::vector<classification_info_t>& classifications, CMatrix* probabilities) override;
	XML::IXMLNode* saveConfig() override;
	bool loadConfig(XML::IXMLNode& node) override;

	_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecision, OVP_ClassId_Algorithm_PairwiseDecision_HT)

private:
	size_t m_nClass = 0;
};

class CAlgorithmPairwiseDecisionHTDesc final : virtual public CAlgorithmPairwiseDecisionDesc
{
public:
	void release() override { }
	CString getName() const override { return "Pairwise decision strategy based on HT"; }
	CString getAuthorName() const override { return "Serrière Guillaume"; }
	CString getAuthorCompanyName() const override { return "Inria"; }
	CString getShortDescription() const override { return "."; }

	CString getDetailedDescription() const override
	{
		return "This method is based on the method describe in the article "
			"Hastie, Trevor; Tibshirani, Robert. Classification by pairwise coupling."
			"The Annals of Statistics 26 (1998), no. 2, 451--471";
	}

	CString getCategory() const override { return ""; }
	CString getVersion() const override { return "0.1"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_PairwiseDecision_HT; }
	IPluginObject* create() override { return new CAlgorithmPairwiseDecisionHT; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(prototype);
		return true;
	}

	_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecisionDesc, OVP_ClassId_Algorithm_PairwiseDecision_HTDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
