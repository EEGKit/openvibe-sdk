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

			CAlgorithmPairwiseDecisionHT() : m_ui32ClassCount(0) { };

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool parameterize();

			virtual bool compute(std::vector<SClassificationInfo>& pClassificationValueList, OpenViBE::IMatrix* pProbabilityVector);
			virtual XML::IXMLNode* saveConfiguration();
			virtual bool loadConfiguration(XML::IXMLNode& rNode);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_PairwiseDecision_HT)

		private:
			uint32_t m_ui32ClassCount;
		};

		class CAlgorithmPairwiseDecisionHTDesc : virtual public CAlgorithmPairwiseDecisionDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Pairwise decision strategy based on HT"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Serrière Guillaume"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Inria"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("."); }

			virtual OpenViBE::CString getDetailedDescription() const
			{
				return OpenViBE::CString("This method is based on the method describe in the article "
					"Hastie, Trevor; Tibshirani, Robert. Classification by pairwise coupling."
					"The Annals of Statistics 26 (1998), no. 2, 451--471");
			}

			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_PairwiseDecision_HT; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmPairwiseDecisionHT; }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_PairwiseDecision_HTDesc)
		};
	}
}
