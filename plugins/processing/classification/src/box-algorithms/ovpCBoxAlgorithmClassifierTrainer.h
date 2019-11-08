#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCBoxAlgorithmCommonClassifierListener.inl"

#include <map>
#include <vector>
#include <iostream>


namespace OpenViBEPlugins
{
	namespace Classification
	{
		const char* const TRAIN_TRIGGER_SETTING_NAME = "Train trigger";
		const char* const FILENAME_SETTING_NAME = "Filename to save configuration to";
		const char* const MULTICLASS_STRATEGY_SETTING_NAME = "Multiclass strategy to apply";
		const char* const ALGORITHM_SETTING_NAME = "Algorithm to use";
		const char* const FOLD_SETTING_NAME = "Number of partitions for k-fold cross-validation test";
		const char* const BALANCE_SETTING_NAME = "Balance classes";
		
		class CBoxAlgorithmClassifierTrainer final : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClassifierTrainer)

		protected:

			typedef struct
			{
				OpenViBE::CMatrix* m_pFeatureVectorMatrix;
				uint64_t m_startTime;
				uint64_t m_endTime;
				uint32_t m_ui32InputIdx;
			} SFeatureVector;

			bool train(const std::vector<SFeatureVector>& dataset, const std::vector<size_t>& permutation, size_t startIdx, size_t stopIdx);
			double getAccuracy(const std::vector<SFeatureVector>& dataset, const std::vector<size_t>& permutation, size_t startIdx, size_t stopIdx, OpenViBE::CMatrix& confusionMatrix);
			bool printConfusionMatrix(const OpenViBE::CMatrix& oMatrix);
			bool balanceDataset();

		private:
			bool saveConfiguration();

		protected:

			std::map<uint32_t, uint32_t> m_nFeatures;

			OpenViBE::Kernel::IAlgorithmProxy* m_classifier = nullptr;
			uint64_t m_trainStimulation                  = 0;
			size_t m_nPartition                            = 0;

			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmClassifierTrainer> m_stimDecoder;
			std::vector<OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmClassifierTrainer>*> m_featureVectorDecoder;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmClassifierTrainer> m_stimEncoder;

			std::map<OpenViBE::CString, OpenViBE::CString>* m_parameter = nullptr;

			std::vector<SFeatureVector> m_datasets;

			std::vector<SFeatureVector> m_balancedDatasets;
		};

		class CBoxAlgorithmClassifierTrainerDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Classifier trainer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard, Guillaume Serriere"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Generic classifier trainer, relying on several box algorithms");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Performs classifier training with cross-validation -based error estimation");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Classification"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("2.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ClassifierTrainer; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmClassifierTrainer; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
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

				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				//				prototype.addFlag(OpenViBE::Kernel::BoxFlag_ManualUpdate);
				return true;
			}

			OpenViBE::Plugins::IBoxListener* createBoxListener() const override
			{
				const size_t nCommonSetting = 6;
				return new CBoxAlgorithmCommonClassifierListener(nCommonSetting);
			}

			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierTrainerDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
