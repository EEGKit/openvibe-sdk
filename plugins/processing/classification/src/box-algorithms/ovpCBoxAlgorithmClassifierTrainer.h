#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpCBoxAlgorithmCommonClassifierListener.inl"

#include <map>
#include <vector>
#include <iostream>


#define OVP_ClassId_BoxAlgorithm_ClassifierTrainer       OpenViBE::CIdentifier(0xF3DAE8A8, 0x3B444154)
#define OVP_ClassId_BoxAlgorithm_ClassifierTrainerDesc   OpenViBE::CIdentifier(0xFE277C91, 0x1593B824)

#define OVP_BoxAlgorithm_ClassifierTrainer_CommonSettingsCount 6

namespace
{
	const char* const TRAIN_TRIGGER_SETTING_NAME       = "Train trigger";
	const char* const FILENAME_SETTING_NAME            = "Filename to save configuration to";
	const char* const MULTICLASS_STRATEGY_SETTING_NAME = "Multiclass strategy to apply";
	const char* const ALGORITHM_SETTING_NAME           = "Algorithm to use";
	const char* const FOLD_SETTING_NAME                = "Number of partitions for k-fold cross-validation test";
	const char* const BALANCE_SETTING_NAME             = "Balance classes";
}


namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmClassifierTrainer final : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClassifierTrainer)

		protected:

			typedef struct
			{
				OpenViBE::CMatrix* m_pFeatureVectorMatrix;
				uint64_t m_ui64StartTime;
				uint64_t m_ui64EndTime;
				uint32_t m_ui32InputIndex;
			} SFeatureVector;

			virtual bool train(const std::vector<SFeatureVector>& rDataset, const std::vector<size_t>& rPermutation, size_t startIdx, size_t stopIndex);
			virtual double getAccuracy(const std::vector<SFeatureVector>& rDataset, const std::vector<size_t>& rPermutation, size_t uiStartIndex,
									   size_t uiStopIndex, OpenViBE::CMatrix& oConfusionMatrix);
			virtual bool printConfusionMatrix(const OpenViBE::CMatrix& oMatrix);
			virtual bool balanceDataset();

		private:
			bool saveConfiguration();

		protected:

			std::map<uint32_t, uint32_t> m_vFeatureCount;

			OpenViBE::Kernel::IAlgorithmProxy* m_pClassifier = nullptr;
			uint64_t m_ui64TrainStimulation                  = 0;
			uint64_t m_ui64PartitionCount                    = 0;

			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmClassifierTrainer> m_oStimulationDecoder;
			std::vector<OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmClassifierTrainer>*> m_vFeatureVectorDecoder;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmClassifierTrainer> m_oStimulationEncoder;

			std::map<OpenViBE::CString, OpenViBE::CString>* m_pParameter = nullptr;

			std::vector<SFeatureVector> m_vDataset;

			std::vector<SFeatureVector> m_vBalancedDataset;
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
				return new CBoxAlgorithmCommonClassifierListener(OVP_BoxAlgorithm_ClassifierTrainer_CommonSettingsCount);
			}

			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierTrainerDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
