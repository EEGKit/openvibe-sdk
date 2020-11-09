#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <map>

namespace OpenViBE {
namespace Plugins {
namespace Classification {
//The aim of this structure is to record informations returned by the sub-classifier. They will be used by
// pairwise decision algorithms to compute probability vector.
// Should be use only by OneVsOne and pairwise decision algorithm
typedef struct
{
	double firstClass;
	double secondClass;
	double classLabel;
	//This output is probabilist
	IMatrix* classificationValue;
} classification_info_t;


class CAlgorithmClassifierOneVsOne final : public Toolkit::CAlgorithmPairingStrategy
{
public:
	bool initialize() override;
	bool uninitialize() override;
	bool train(const Toolkit::IFeatureVectorSet& dataset) override;
	bool classify(const Toolkit::IFeatureVector& sample, double& classId, Toolkit::IVector& distance, Toolkit::IVector& probability) override;
	bool designArchitecture(const CIdentifier& id, const size_t classCount) override;
	XML::IXMLNode* saveConfig() override;
	bool loadConfig(XML::IXMLNode* configNode) override;
	size_t getNProbabilities() override { return m_nClasses; }
	size_t getNDistances() override { return 0; }

	_IsDerivedFromClass_Final_(Toolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsOne)

protected:

	bool createSubClassifiers();

private:
	size_t m_nClasses        = 0;
	size_t m_nSubClassifiers = 0;

	std::map<std::pair<size_t, size_t>, Kernel::IAlgorithmProxy*> m_subClassifiers;
	fClassifierComparison m_algorithmComparison = nullptr;

	Kernel::IAlgorithmProxy* m_decisionStrategyAlgorithm = nullptr;
	CIdentifier m_pairwiseDecisionID                     = OV_UndefinedIdentifier;

	static XML::IXMLNode* getClassifierConfig(double firstClass, double secondClass, Kernel::IAlgorithmProxy* subClassifier);
	XML::IXMLNode* getPairwiseDecisionConfiguration() const;

	// size_t getClassCount() const;

	bool loadSubClassifierConfig(XML::IXMLNode* node);

	// SSubClassifierDescriptor& getSubClassifierDescriptor(const size_t FirstClass, const size_t SecondClass);
	bool setSubClassifierIdentifier(const CIdentifier& id);
};

class CAlgorithmClassifierOneVsOneDesc final : public Toolkit::CAlgorithmPairingStrategyDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("OneVsOne pairing classifier"); }
	CString getAuthorName() const override { return CString("Guillaume Serriere"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/Loria"); }
	CString getShortDescription() const override { return CString(""); }
	CString getDetailedDescription() const override { return CString(""); }
	CString getCategory() const override { return CString(""); }
	CString getVersion() const override { return CString("0.2"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierOneVsOne; }
	IPluginObject* create() override { return new CAlgorithmClassifierOneVsOne; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CAlgorithmPairingStrategyDesc::getAlgorithmPrototype(prototype);
		prototype.addInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType, "Pairwise Decision Strategy",
									Kernel::ParameterType_Enumeration, OVP_TypeId_ClassificationPairwiseStrategy);
		return true;
	}

	_IsDerivedFromClass_Final_(CAlgorithmPairingStrategyDesc, OVP_ClassId_Algorithm_ClassifierOneVsOneDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
