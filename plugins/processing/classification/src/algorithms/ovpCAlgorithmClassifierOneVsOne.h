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

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet);
			virtual bool classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector
								  , double& rf64Class
								  , OpenViBEToolkit::IVector& rDistanceValue
								  , OpenViBEToolkit::IVector& rProbabilityValue);
			virtual bool designArchitecture(const OpenViBE::CIdentifier& rId, uint32_t rClassCount);

			virtual XML::IXMLNode* saveConfiguration();
			virtual bool loadConfiguration(XML::IXMLNode* pConfigurationNode);

			virtual uint32_t getOutputProbabilityVectorLength();
			virtual uint32_t getOutputDistanceVectorLength();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsOne)

		protected:

			virtual bool createSubClassifiers();

		private:
			uint32_t m_ui32NumberOfClasses;
			uint32_t m_ui32NumberOfSubClassifiers;

			std::map<std::pair<uint32_t, uint32_t>, OpenViBE::Kernel::IAlgorithmProxy*> m_oSubClassifiers;
			fClassifierComparison m_fAlgorithmComparison;

			OpenViBE::Kernel::IAlgorithmProxy* m_pDecisionStrategyAlgorithm;
			OpenViBE::CIdentifier m_oPairwiseDecisionIdentifier = OV_UndefinedIdentifier;

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

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("OneVsOne pairing classifier"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Guillaume Serriere"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/Loria"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("0.2"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_ClassifierOneVsOne; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmClassifierOneVsOne; }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
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
