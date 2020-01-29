#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <map>

namespace OpenViBEPlugins
{
	namespace Classification
	{
		//The aim of this structure is to record informations returned by the sub-classifier. They will be used by
		// pairwise decision algorithms to compute probability vector.
		// Should be use only by OneVsOne and pairwise decision algorithm
		typedef struct
		{
			double firstClass;
			double secondClass;
			double classLabel;
			//This output is probabilist
			OpenViBE::IMatrix* classificationValue;
		} classification_info_t;


		class CAlgorithmClassifierOneVsOne final : public OpenViBE::Toolkit::CAlgorithmPairingStrategy
		{
		public:
			bool initialize() override;
			bool uninitialize() override;
			bool train(const OpenViBE::Toolkit::IFeatureVectorSet& dataset) override;
			bool classify(const OpenViBE::Toolkit::IFeatureVector& sample, double& classId, OpenViBE::Toolkit::IVector& distance,
						  OpenViBE::Toolkit::IVector& probability) override;
			bool designArchitecture(const OpenViBE::CIdentifier& id, const size_t classCount) override;
			XML::IXMLNode* saveConfig() override;
			bool loadConfig(XML::IXMLNode* configNode) override;
			size_t getNProbabilities() override { return m_nClasses; }
			size_t getNDistances() override { return 0; }

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsOne)

		protected:

			bool createSubClassifiers();

		private:
			size_t m_nClasses        = 0;
			size_t m_nSubClassifiers = 0;

			std::map<std::pair<size_t, size_t>, OpenViBE::Kernel::IAlgorithmProxy*> m_subClassifiers;
			fClassifierComparison m_algorithmComparison = nullptr;

			OpenViBE::Kernel::IAlgorithmProxy* m_decisionStrategyAlgorithm = nullptr;
			OpenViBE::CIdentifier m_pairwiseDecisionID                     = OV_UndefinedIdentifier;

			static XML::IXMLNode* getClassifierConfig(double firstClass, double secondClass, OpenViBE::Kernel::IAlgorithmProxy* subClassifier);
			XML::IXMLNode* getPairwiseDecisionConfiguration() const;

			// size_t getClassCount() const;

			bool loadSubClassifierConfig(XML::IXMLNode* node);

			// SSubClassifierDescriptor& getSubClassifierDescriptor(const size_t FirstClass, const size_t SecondClass);
			bool setSubClassifierIdentifier(const OpenViBE::CIdentifier& id);
		};

		class CAlgorithmClassifierOneVsOneDesc final : public OpenViBE::Toolkit::CAlgorithmPairingStrategyDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("OneVsOne pairing classifier"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Guillaume Serriere"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/Loria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("0.2"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierOneVsOne; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmClassifierOneVsOne; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CAlgorithmPairingStrategyDesc::getAlgorithmPrototype(prototype);
				prototype.addInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType, "Pairwise Decision Strategy",
											OpenViBE::Kernel::ParameterType_Enumeration, OVP_TypeId_ClassificationPairwiseStrategy);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmPairingStrategyDesc, OVP_ClassId_Algorithm_ClassifierOneVsOneDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
