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
	const char* const c_sTrainTriggerSettingName       = "Train trigger";
	const char* const c_sFilenameSettingName           = "Filename to save configuration to";
	const char* const c_sMulticlassStrategySettingName = "Multiclass strategy to apply";
	const char* const c_sAlgorithmSettingName          = "Algorithm to use";
	const char* const c_sKFoldSettingName              = "Number of partitions for k-fold cross-validation test";
	const char* const c_sBalanceSettingName            = "Balance classes";
}


namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmClassifierTrainer : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClassifierTrainer)

		protected:

			typedef struct
			{
				OpenViBE::CMatrix* m_pFeatureVectorMatrix;
				uint64_t m_ui64StartTime;
				uint64_t m_ui64EndTime;
				uint32_t m_ui32InputIndex;
			} SFeatureVector;

			virtual bool train(const std::vector<SFeatureVector>& rDataset,
							   const std::vector<size_t>& rPermutation,
							   size_t uiStartIndex, size_t uiStopIndex);
			virtual double getAccuracy(const std::vector<SFeatureVector>& rDataset,
									   const std::vector<size_t>& rPermutation,
									   size_t uiStartIndex, size_t uiStopIndex, OpenViBE::CMatrix& oConfusionMatrix);
			virtual bool printConfusionMatrix(const OpenViBE::CMatrix& oMatrix);
			virtual bool balanceDataset();

		private:
			bool saveConfiguration();

		protected:

			std::map<uint32_t, uint32_t> m_vFeatureCount;

			OpenViBE::Kernel::IAlgorithmProxy* m_pClassifier;
			uint64_t m_ui64TrainStimulation;
			uint64_t m_ui64PartitionCount;

			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmClassifierTrainer> m_oStimulationDecoder;
			std::vector<OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmClassifierTrainer>*> m_vFeatureVectorDecoder;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmClassifierTrainer> m_oStimulationEncoder;

			std::map<OpenViBE::CString, OpenViBE::CString>* m_pParameter;

			std::vector<SFeatureVector> m_vDataset;

			std::vector<SFeatureVector> m_vBalancedDataset;
		};

		class CBoxAlgorithmClassifierTrainerDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Classifier trainer"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard, Guillaume Serriere"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Generic classifier trainer, relying on several box algorithms"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Performs classifier training with cross-validation -based error estimation"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Classification"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("2.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_ClassifierTrainer; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmClassifierTrainer; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Features for class 1", OV_TypeId_FeatureVector);
				rBoxAlgorithmPrototype.addInput("Features for class 2", OV_TypeId_FeatureVector);

				rBoxAlgorithmPrototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting(c_sTrainTriggerSettingName, OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
				rBoxAlgorithmPrototype.addSetting(c_sFilenameSettingName, OV_TypeId_Filename, "${Path_UserData}/my-classifier.xml");

				rBoxAlgorithmPrototype.addSetting(c_sMulticlassStrategySettingName, OVTK_TypeId_ClassificationStrategy, "Native");
				//Pairing startegy argument
				//Class label

				rBoxAlgorithmPrototype.addSetting(c_sAlgorithmSettingName, OVTK_TypeId_ClassificationAlgorithm, "Linear Discrimimant Analysis (LDA)");
				//Argument of algorithm

				rBoxAlgorithmPrototype.addSetting(c_sKFoldSettingName, OV_TypeId_Integer, "10");
				rBoxAlgorithmPrototype.addSetting(c_sBalanceSettingName, OV_TypeId_Boolean, "false");

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				//				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_ManualUpdate);
				return true;
			}

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmCommonClassifierListener(OVP_BoxAlgorithm_ClassifierTrainer_CommonSettingsCount); }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClassifierTrainerDesc)
		};
	}
}
