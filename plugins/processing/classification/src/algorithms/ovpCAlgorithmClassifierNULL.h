#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

namespace OpenViBE {
namespace Plugins {
namespace Classification {
class CAlgorithmClassifierNULL final : public Toolkit::CAlgorithmClassifier
{
public:
	CAlgorithmClassifierNULL() { }
	bool initialize() override;
	bool train(const Toolkit::IFeatureVectorSet& featureVectorSet) override;
	bool classify(const Toolkit::IFeatureVector& featureVector, double& classId, Toolkit::IVector& distance, Toolkit::IVector& probability) override;
	XML::IXMLNode* saveConfig() override { return nullptr; }
	bool loadConfig(XML::IXMLNode* /*configurationNode*/) override { return true; }
	size_t getNProbabilities() override { return 1; }
	size_t getNDistances() override { return 1; }

	_IsDerivedFromClass_Final_(CAlgorithmClassifier, OVP_ClassId_Algorithm_ClassifierNULL)
};

class CAlgorithmClassifierNULLDesc final : public Toolkit::CAlgorithmClassifierDesc
{
public:
	void release() override { }

	CString getName() const override { return "NULL Classifier (does nothing)"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }
	CString getShortDescription() const override { return ""; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Samples"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ClassifierNULL; }
	IPluginObject* create() override { return new CAlgorithmClassifierNULL; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CAlgorithmClassifierDesc::getAlgorithmPrototype(prototype);
		prototype.addInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1, "Parameter 1", Kernel::ParameterType_Boolean);
		prototype.addInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2, "Parameter 2", Kernel::ParameterType_Float);
		prototype.addInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3, "Parameter 3", Kernel::ParameterType_Enumeration,
									OV_TypeId_Stimulation);
		return true;
	}

	_IsDerivedFromClass_Final_(CAlgorithmClassifierDesc, OVP_ClassId_Algorithm_ClassifierNULLDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
