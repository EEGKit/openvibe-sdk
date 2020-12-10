#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <vector>


namespace OpenViBE {
namespace Plugins {
namespace Classification {
class CAlgorithmClassifierOneVsAll final : public Toolkit::CAlgorithmPairingStrategy
{
public:
	bool initialize() override;
	bool uninitialize() override;
	bool train(const Toolkit::IFeatureVectorSet& dataset) override;
	bool classify(const Toolkit::IFeatureVector& sample, double& classId, Toolkit::IVector& distance, Toolkit::IVector& probability) override;
	bool designArchitecture(const CIdentifier& id, const size_t nClass) override;
	XML::IXMLNode* saveConfig() override;
	bool loadConfig(XML::IXMLNode* configNode) override;
	size_t getNProbabilities() override { return m_subClassifiers.size(); }
	size_t getNDistances() override;

	_IsDerivedFromClass_Final_(Toolkit::CAlgorithmPairingStrategy, OVP_ClassId_Algorithm_ClassifierOneVsAll)


private:
	static XML::IXMLNode* getClassifierConfig(Kernel::IAlgorithmProxy* classifier);
	bool addNewClassifierAtBack();
	void removeClassifierAtBack();
	bool setSubClassifierIdentifier(const CIdentifier& id);
	size_t getClassCount() const { return m_subClassifiers.size(); }

	bool loadSubClassifierConfig(XML::IXMLNode* node);

	std::vector<Kernel::IAlgorithmProxy*> m_subClassifiers;
	fClassifierComparison m_fAlgorithmComparison = nullptr;
};

class CAlgorithmClassifierOneVsAllDesc final : public Toolkit::CAlgorithmPairingStrategyDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("OneVsAll pairing classifier"); }
	CString getAuthorName() const override { return CString("Guillaume Serriere"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/Loria"); }
	CString getShortDescription() const override { return CString(""); }
	CString getDetailedDescription() const override { return CString(""); }
	CString getCategory() const override { return CString(""); }
	CString getVersion() const override { return CString("0.1"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierOneVsAll; }
	IPluginObject* create() override { return new CAlgorithmClassifierOneVsAll; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CAlgorithmPairingStrategyDesc::getAlgorithmPrototype(prototype);
		return true;
	}

	_IsDerivedFromClass_Final_(CAlgorithmPairingStrategyDesc, OVP_ClassId_Algorithm_ClassifierOneVsAllDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
