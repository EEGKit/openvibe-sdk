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
			bool initialize() override;
			bool uninitialize() override;
			bool parameterize() override;
			bool compute(std::vector<classification_info_t>& pClassificationValueList, OpenViBE::IMatrix* pProbabilityVector) override;
			XML::IXMLNode* saveConfig() override;
			bool loadConfig(XML::IXMLNode& /*node*/) override { return true; }

			_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecision, OVP_ClassId_Algorithm_PairwiseStrategy_PKPD)

		private:
			uint32_t m_nClass = 0;
		};

		class CAlgorithmPairwiseStrategyPKPDDesc final : virtual public CAlgorithmPairwiseDecisionDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Pairwise decision strategy based on PKPD"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Serrière Guillaume"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("."); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Price, S. Knerr, L. Personnaz, and G. Dreyfus."
					"Pairwise neural network classifiers with probabilistic outputs."
					" In G. Tesauro, D. Touretzky, and T. Leen (eds.)"
					"Advances in Neural Information Processing Systems 7 (NIPS-94), pp."
					" 1109-1116. MIT Press, 1995.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_PairwiseStrategy_PKPD; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmPairwiseStrategyPKPD; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(prototype);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmPairwiseDecisionDesc, OVP_ClassId_Algorithm_PairwiseStrategy_PKPDDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
