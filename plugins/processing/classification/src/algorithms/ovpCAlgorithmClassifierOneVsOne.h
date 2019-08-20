#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <vector>
#include <map>

#define OVP_ClassId_Algorithm_ClassifierOneVsOne								OpenViBE::CIdentifier(0x638C2F90, 0xEAE10226)
#define OVP_ClassId_Algorithm_ClassifierOneVsOneDesc							OpenViBE::CIdentifier(0xE78E7CDB, 0x369AA9EF)

#define OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType			OpenViBE::CIdentifier(0x0C347BBA, 0x180577F9)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		//The aim of this structure is to record informations returned by the sub-classifier. They will be used by
		// pairwise decision algorithms to compute probability vector.
		// Should be use only by OneVsOne and pairwise decision algorithm
		typedef struct
		{
			double m_f64FirstClass;
			double m_f64SecondClass;
			double m_f64ClassLabel;
			//This output is probabilist
			OpenViBE::IMatrix* m_pClassificationValue;
		} SClassificationInfo;


		class CAlgorithmClassifierOneVsOne : public OpenViBEToolkit::CAlgorithmPairingStrategy
		{
		public:
			bool initialize() override;
			bool uninitialize() override;
			bool train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet) override;
			bool classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector
						  , double& rf64Class
						  , OpenViBEToolkit::IVector& rDistanceValue
						  , OpenViBEToolkit::IVector& rProbabilityValue) override;
			bool designArchitecture(const OpenViBE::CIdentifier& rId, uint32_t rClassCount) override;
			XML::IXMLNode* saveConfiguration() override;
			bool loadConfiguration(XML::IXMLNode* pConfigurationNode) override;
			uint32_t getOutputProbabilityVectorLength() override;
			uint32_t getOutputDistanceVectorLength() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsOne)

		protected:

			virtual bool createSubClassifiers();

		private:
			uint32_t m_ui32NumberOfClasses        = 0;
			uint32_t m_ui32NumberOfSubClassifiers = 0;

			std::map<std::pair<uint32_t, uint32_t>, OpenViBE::Kernel::IAlgorithmProxy*> m_oSubClassifiers;
			fClassifierComparison m_fAlgorithmComparison;

			OpenViBE::Kernel::IAlgorithmProxy* m_pDecisionStrategyAlgorithm = nullptr;
			OpenViBE::CIdentifier m_oPairwiseDecisionIdentifier             = OV_UndefinedIdentifier;

			XML::IXMLNode* getClassifierConfiguration(double f64FirstClass, double f64SecondClass, OpenViBE::Kernel::IAlgorithmProxy* pSubClassifier);
			XML::IXMLNode* getPairwiseDecisionConfiguration();

			// uint32_t getClassCount() const;

			bool loadSubClassifierConfiguration(XML::IXMLNode* pSubClassifiersNode);

			// SSubClassifierDescriptor& getSubClassifierDescriptor(const uint32_t f64FirstClass, const uint32_t f64SecondClass);
			bool setSubClassifierIdentifier(const OpenViBE::CIdentifier& rId);
		};

		class CAlgorithmClassifierOneVsOneDesc : public OpenViBEToolkit::CAlgorithmPairingStrategyDesc
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

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CAlgorithmPairingStrategyDesc::getAlgorithmPrototype(rAlgorithmPrototype);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType, "Pairwise Decision Strategy",
													  OpenViBE::Kernel::ParameterType_Enumeration, OVP_TypeId_ClassificationPairwiseStrategy);
				return true;
			}

			_IsDerivedFromClass_Final_(CAlgorithmPairingStrategyDesc, OVP_ClassId_Algorithm_ClassifierOneVsOneDesc)
		};
	}
}
