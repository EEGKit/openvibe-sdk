#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCAlgorithmPairwiseDecision.h"

#define OVP_ClassId_Algorithm_PairwiseStrategy_PKPD												OpenViBE::CIdentifier(0x26EF6DDA, 0xF137053C)
#define OVP_ClassId_Algorithm_PairwiseStrategy_PKPDDesc											OpenViBE::CIdentifier(0x191EB02A, 0x6866214A)

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
		class CAlgorithmPairwiseStrategyPKPD : virtual public CAlgorithmPairwiseDecision
		{
		public:

			CAlgorithmPairwiseStrategyPKPD() { }

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool parameterize();

			virtual bool compute(std::vector<SClassificationInfo>& pClassificationValueList, OpenViBE::IMatrix* pProbabilityVector);
			virtual XML::IXMLNode* saveConfiguration();
			virtual bool loadConfiguration(XML::IXMLNode& rNode);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_PairwiseStrategy_PKPD)

		private:
			uint32_t m_ui32ClassCount = 0;
		};

		class CAlgorithmPairwiseStrategyPKPDDesc : virtual public CAlgorithmPairwiseDecisionDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Pairwise decision strategy based on PKPD"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Serrière Guillaume"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Inria"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("."); }

			virtual OpenViBE::CString getDetailedDescription() const
			{
				return OpenViBE::CString("Price, S. Knerr, L. Personnaz, and G. Dreyfus."
					"Pairwise neural network classifiers with probabilistic outputs."
					" In G. Tesauro, D. Touretzky, and T. Leen (eds.)"
					"Advances in Neural Information Processing Systems 7 (NIPS-94), pp."
					" 1109-1116. MIT Press, 1995.");
			}

			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_PairwiseStrategy_PKPD; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmPairwiseStrategyPKPD; }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_PairwiseStrategy_PKPDDesc)
		};
	}
}
