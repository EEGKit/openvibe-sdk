#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCAlgorithmPairwiseDecision.h"

#define OVP_ClassId_Algorithm_PairwiseDecision_Voting												OpenViBE::CIdentifier(0xA111B830, 0x4679BAFD)
#define OVP_ClassId_Algorithm_PairwiseDecision_VotingDesc											OpenViBE::CIdentifier(0xAC5A39E8, 0x3A57822A)

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
		class CAlgorithmPairwiseDecisionVoting : virtual public CAlgorithmPairwiseDecision
		{
		public:

			CAlgorithmPairwiseDecisionVoting() : m_ui32ClassCount(0) { };

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool parameterize();

			virtual bool compute(std::vector<SClassificationInfo>& pClassificationValueList, OpenViBE::IMatrix* pProbabilityVector);
			virtual XML::IXMLNode* saveConfiguration();
			virtual bool loadConfiguration(XML::IXMLNode& rNode);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_PairwiseDecision_Voting)

		private:
			uint32_t m_ui32ClassCount;
		};

		class CAlgorithmPairwiseDecisionVotingDesc : virtual public CAlgorithmPairwiseDecisionDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Pairwise decision strategy based on Voting"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Serrière Guillaume"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Inria"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("0.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_PairwiseDecision_Voting; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmPairwiseDecisionVoting; }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CAlgorithmPairwiseDecisionDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_PairwiseDecision_VotingDesc)
		};
	}
}


