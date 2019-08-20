#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCAlgorithmPairwiseDecision.h"

#define OVP_ClassId_Algorithm_PairwiseDecision_HT												OpenViBE::CIdentifier(0xD24F7F19, 0xA744FAD2)
#define OVP_ClassId_Algorithm_PairwiseDecision_HTDesc											OpenViBE::CIdentifier(0xE837F5C0, 0xF65C1341)

namespace OpenViBEPlugins
{
	namespace Classification
	{	/**
		 * @brief The CAlgorithmPairwiseDecisionHT class is a decision strategy for the One Vs One pairwise decision that implement the
		 * method describe in the article Hastie, Trevor; Tibshirani, Robert. Classification by pairwise coupling. The Annals of Statistics 26 (1998), no. 2, 451--471
		 *
		 * Probability required
		 */
		class CAlgorithmPairwiseDecisionHT : virtual public CAlgorithmPairwiseDecision
		{
		public:

			CAlgorithmPairwiseDecisionHT() { }
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool parameterize() override;
			bool compute(std::vector<SClassificationInfo>& pClassificationValueList, OpenViBE::IMatrix* pProbabilityVector) override;
			XML::IXMLNode* saveConfiguration() override;
			bool loadConfiguration(XML::IXMLNode& rNode) override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_PairwiseDecision_HT)

		private:
			uint32_t m_ui32ClassCount = 0;
		};

		class CAlgorithmPairwiseDecisionHTDesc : virtual public CAlgorithmPairwiseDecisionDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Pairwise decision strategy based on HT"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Serrière Guillaume"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("."); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("This method is based on the method describe in the article "
					"Hastie, Trevor; Tibshirani, Robert. Classification by pairwise coupling."
					"The Annals of Statistics 26 (1998), no. 2, 451--471");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_PairwiseDecision_HT; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmPairwiseDecisionHT; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_PairwiseDecision_HTDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
