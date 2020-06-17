#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>

#include "ovpCBoxAlgorithmCommonClassifierListener.inl"

#include <map>
#include <vector>
#include <iostream>


namespace OpenViBE {
namespace Plugins {
namespace Classification {
const char* const TRAIN_TRIGGER_SETTING_NAME       = "Train trigger";
const char* const FILENAME_SETTING_NAME            = "Filename to save configuration to";
const char* const MULTICLASS_STRATEGY_SETTING_NAME = "Multiclass strategy to apply";
const char* const ALGORITHM_SETTING_NAME           = "Algorithm to use";
const char* const FOLD_SETTING_NAME                = "Number of partitions for k-fold cross-validation test";
const char* const BALANCE_SETTING_NAME             = "Balance classes";

class CBoxAlgorithmClassifierTrainer final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ClassifierTrainer)

protected:

	typedef struct
	{
		CMatrix* sampleMatrix;
		CTime startTime;
		CTime endTime;
		size_t inputIdx;
	} sample_t;

	bool train(const std::vector<sample_t>& dataset, const std::vector<size_t>& permutation, size_t startIdx, size_t stopIdx);
	double getAccuracy(const std::vector<sample_t>& dataset, const std::vector<size_t>& permutation, size_t startIdx, size_t stopIdx,
					   CMatrix& confusionMatrix);
	bool printConfusionMatrix(const CMatrix& oMatrix);
	bool balanceDataset();

private:
	bool saveConfig();

protected:

	std::map<size_t, size_t> m_nFeatures;

	Kernel::IAlgorithmProxy* m_classifier = nullptr;
	uint64_t m_trainStimulation           = 0;
	size_t m_nPartition                   = 0;

	Toolkit::TStimulationDecoder<CBoxAlgorithmClassifierTrainer> m_stimDecoder;
	std::vector<Toolkit::TFeatureVectorDecoder<CBoxAlgorithmClassifierTrainer>*> m_sampleDecoder;

	Toolkit::TStimulationEncoder<CBoxAlgorithmClassifierTrainer> m_encoder;

	std::map<CString, CString>* m_parameter = nullptr;

	std::vector<sample_t> m_datasets;
	std::vector<sample_t> m_balancedDatasets;
};

class CBoxAlgorithmClassifierTrainerDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Classifier trainer"; }
	CString getAuthorName() const override { return "Yann Renard, Guillaume Serriere"; }
	CString getAuthorCompanyName() const override { return "INRIA/IRISA"; }

	CString getShortDescription() const override { return "Generic classifier trainer, relying on several box algorithms"; }

	CString getDetailedDescription() const override { return "Performs classifier training with cross-validation -based error estimation"; }

	CString getCategory() const override { return "Classification"; }
	CString getVersion() const override { return "2.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.1.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ClassifierTrainer; }
	IPluginObject* create() override { return new CBoxAlgorithmClassifierTrainer; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Stimulations", OV_TypeId_Stimulations);
		prototype.addInput("Features for class 1", OV_TypeId_FeatureVector);
		prototype.addInput("Features for class 2", OV_TypeId_FeatureVector);

		prototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

		prototype.addSetting(TRAIN_TRIGGER_SETTING_NAME, OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
		prototype.addSetting(FILENAME_SETTING_NAME, OV_TypeId_Filename, "${Path_UserData}/my-classifier.xml");

		prototype.addSetting(MULTICLASS_STRATEGY_SETTING_NAME, OVTK_TypeId_ClassificationStrategy, "Native");
		//Pairing startegy argument
		//Class label

		prototype.addSetting(ALGORITHM_SETTING_NAME, OVTK_TypeId_ClassificationAlgorithm, "Linear Discrimimant Analysis (LDA)");
		//Argument of algorithm

		prototype.addSetting(FOLD_SETTING_NAME, OV_TypeId_Integer, "10");
		prototype.addSetting(BALANCE_SETTING_NAME, OV_TypeId_Boolean, "false");

		prototype.addFlag(Kernel::BoxFlag_CanAddInput);

		//				prototype.addFlag(Kernel::BoxFlag_ManualUpdate);
		return true;
	}

	IBoxListener* createBoxListener() const override
	{
		const size_t nCommonSetting = 6;
		return new CBoxAlgorithmCommonClassifierListener(nCommonSetting);
	}

	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierTrainerDesc)
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
