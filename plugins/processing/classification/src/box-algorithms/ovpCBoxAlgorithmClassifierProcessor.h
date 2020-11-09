#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <map>


namespace OpenViBE {
namespace Plugins {
namespace Classification {
class CBoxAlgorithmClassifierProcessor final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ClassifierProcessor)

protected:
	bool loadClassifier(const char* filename);

private:

	Toolkit::TFeatureVectorDecoder<CBoxAlgorithmClassifierProcessor> m_sampleDecoder;
	Toolkit::TStimulationDecoder<CBoxAlgorithmClassifierProcessor> m_stimDecoder;
	Toolkit::TStimulationEncoder<CBoxAlgorithmClassifierProcessor> m_labelsEncoder;
	Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmClassifierProcessor> m_hyperplanesEncoder;
	Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmClassifierProcessor> m_probabilitiesEncoder;

	Kernel::IAlgorithmProxy* m_classifier = nullptr;

	std::map<double, uint64_t> m_stimulations;
};

class CBoxAlgorithmClassifierProcessorDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Classifier processor"); }
	CString getAuthorName() const override { return CString("Yann Renard, Guillaume Serriere"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
	CString getShortDescription() const override { return CString("Generic classification, relying on several box algorithms"); }

	CString getDetailedDescription() const override { return CString("Classifies incoming feature vectors using a previously learned classifier."); }

	CString getCategory() const override { return CString("Classification"); }
	CString getVersion() const override { return CString("2.1"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.1.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ClassifierProcessor; }
	IPluginObject* create() override { return new CBoxAlgorithmClassifierProcessor; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Features", OV_TypeId_FeatureVector);
		prototype.addInput("Commands", OV_TypeId_Stimulations);
		prototype.addOutput("Labels", OV_TypeId_Stimulations);
		prototype.addOutput("Hyperplane distance", OV_TypeId_StreamedMatrix);
		prototype.addOutput("Probability values", OV_TypeId_StreamedMatrix);

		//We load everything in the save filed
		prototype.addSetting("Filename to load configuration from", OV_TypeId_Filename, "");
		return true;
	}

	// virtual IBoxListener* createBoxListener() const { return new CBoxAlgorithmCommonClassifierListener(5); }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierProcessorDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
