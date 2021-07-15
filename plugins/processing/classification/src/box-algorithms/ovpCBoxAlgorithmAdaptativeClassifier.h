///-------------------------------------------------------------------------------------------------
/// 
/// \file CBoxAlgorithmAdaptativeClassifier.h
/// \brief Classes of the Box AdaptativeClassifier.
/// \author Tristan Venot (INRIA).
/// \version 0.1.
/// \date Tue Jan 19 18:17:13 2021.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

//You may have to change this path to match your folder organisation
#include "../ovp_defines.h"
#include "ovpCBoxAlgorithmClassifierTrainer.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "../algorithms/ovpCAlgorithmLDADiscriminantFunction.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <xml/IXMLNode.h>

#include <stack>

#include <Eigen/Dense>

#include <map>
#include <vector>
#include <iostream>
// The unique identifiers for the box and its descriptor.
// Identifier are randomly chosen by the skeleton-generator.


namespace OpenViBE
{
	namespace Plugins
	{
		namespace Classification
		{	
			/*
			const char* const TRAIN_TRIGGER_SETTING_NAME = "Train trigger";
			const char* const FILENAME_SETTING_NAME = "Filename to save configuration to";
			const char* const MULTICLASS_STRATEGY_SETTING_NAME = "Multiclass strategy to apply";
			const char* const ALGORITHM_SETTING_NAME = "Algorithm to use";
			const char* const FOLD_SETTING_NAME = "Number of partitions for k-fold cross-validation test";
			const char* const BALANCE_SETTING_NAME = "Balance classes";
			*/
			
			class CAlgorithmLDADiscriminantFunction;
			
			int LDAClassificationCompare_Adaptive(IMatrix& first, IMatrix& second);

			typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;
			/// <summary> The class CBoxAlgorithmAdaptativeClassifier describes the box AdaptativeClassifier. </summary>
			class CBoxAlgorithmAdaptativeClassifier final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
			{
			public:
				//Trainer + Processor
				void release() override { delete this; }
				bool processInput(const size_t index) override;
				bool process() override;
				//Trainer + Processor + LDA
				bool initialize() override;
				bool uninitialize() override;
			
				//Processor : 
			protected:
				bool loadClassifier(const char* filename);
			
			//LDA :
			public:
				bool train(const Toolkit::IFeatureVectorSet& dataset);
				
				int Number_Features = 0;
				
				
				
				XML::IXMLNode* saveConfig() ;
				bool loadConfig(XML::IXMLNode* configNode) ;
				size_t getNProbabilities()  { return m_discriminantFunctions.size(); }
				size_t getNDistances()  { return m_discriminantFunctions.size(); }

				_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_AdaptativeClassifier)
			
			//Trainer : 
			protected:
				
				typedef struct
				{
					CMatrix* sampleMatrix;
					uint64_t startTime;
					uint64_t endTime;
					size_t inputIdx;
					
				} sample_t;
				int New_trial_left = 0;
				int New_trial_right = 0;
				bool train(const std::vector<sample_t>& dataset, const std::vector<size_t>& permutation, size_t startIdx, size_t stopIdx);
				Eigen::MatrixXd classify(sample_t sample_proces, Eigen::MatrixXd Covariance);
				bool balanceDataset();
				size_t Size_Cov;
			private:
				bool saveConfig_Trainer();
				bool Adaptation = false;
			protected:
				std::map<size_t, size_t> m_nFeatures_Train;

				Kernel::IAlgorithmProxy* m_classifier_Train = nullptr;
				uint64_t m_trainStimulation_Train           = 0;
				size_t m_nPartition_Train                   = 0;

				Toolkit::TStimulationDecoder<CBoxAlgorithmAdaptativeClassifier> m_stimDecoder_Train;
				std::vector<Toolkit::TFeatureVectorDecoder<CBoxAlgorithmAdaptativeClassifier>*> m_sampleDecoder_Train;

				Eigen::MatrixXd Covariance_Initial;
				Eigen::MatrixXd X_Mean_Initial_1;
				Eigen::MatrixXd X_Mean_Initial_2;
				Eigen::Vector3d Samples_size_beginning;
				void setStimulation(const size_t /*stimulationIndex*/, const uint64_t identifier, const uint64_t /*stimulationDate*/);
				Eigen::MatrixXd openData(std::string fileToOpen);
				double Distance = 0.0;
				Toolkit::TStimulationEncoder<CBoxAlgorithmAdaptativeClassifier> m_encoder_Train;

				std::map<CString, CString>* m_parameter_Train = nullptr;
				bool Classify = false;
				std::vector<sample_t> m_datasets_Train;
				std::vector<sample_t> m_balancedDatasets_Train;
				IMatrix* m_oMatrix = nullptr;
				bool shrinkage;
				double ip_shrinkage;
				bool ip_diagonalCov;
			//Processor : 

			private:

				Toolkit::TFeatureVectorDecoder<CBoxAlgorithmAdaptativeClassifier> m_sampleDecoder_Processor;
				Toolkit::TStimulationDecoder<CBoxAlgorithmAdaptativeClassifier> m_stimDecoder_Processor;

				//Toolkit::TStimulationEncoder<CBoxAlgorithmAdaptativeClassifier> m_labelsEncoder_Processor;
				Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmAdaptativeClassifier> m_hyperplanesEncoder_Processor;
				//Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmAdaptativeClassifier> m_probabilitiesEncoder_Processor;

				Kernel::IAlgorithmProxy* m_classifier_Processor = nullptr;

				std::map<double, uint64_t> m_stimulations_Processor;
			// LDA :
			protected:
				static void dumpMatrix(Kernel::ILogManager& pMgr, const MatrixXdRowMajor& mat, const CString& desc);

				std::vector<double> m_labels;
				std::vector<CAlgorithmLDADiscriminantFunction> m_discriminantFunctions;
				int TrainingSupervised = 0;
				double SupervisedCoeff = 0.05;
				
				Eigen::MatrixXd m_coefficients;
				Eigen::MatrixXd m_weights;
				double m_biasDistance = 0;
				double m_w0           = 0;

				size_t m_nCols    = 0;
				size_t m_nClasses = 0;

				Kernel::IAlgorithmProxy* m_covAlgorithm = nullptr;

			private:
				void loadClassesFromNode(XML::IXMLNode* node);
				void loadCoefsFromNode(XML::IXMLNode* node);

				size_t getClassCount() const { return m_nClasses; }
				
				
			
			};


			// If you need to implement a box Listener, here is a sekeleton for you.
			// Use only the callbacks you need.
			// For example, if your box has a variable number of input, but all of them must be stimulation inputs.
			// The following listener callback will ensure that any newly added input is stimulations :
			/*		
			bool onInputAdded(Kernel::IBox& box, const size_t index) override 
			{
				box.setInputType(index, OV_TypeId_Stimulations);
			};
			*/

			/*
			// The box listener can be used to call specific callbacks whenever the box structure changes : input added, name changed, etc.
			// Please uncomment below the callbacks you want to use.
			/// <summary> Listener of the box AdaptativeClassifier. </summary>
			class CBoxAlgorithmAdaptativeClassifierListener final : public Toolkit::TBoxListener<IBoxListener>
			{
			public:

				//bool onInitialized(Kernel::IBox& box) override { return true; };
				//bool onNameChanged(Kernel::IBox& box) override { return true; };
				//bool onInputConnected(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onInputDisconnected(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onInputAdded(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onInputRemoved(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onInputNameChanged(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onOutputConnected(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onOutputDisconnected(Kernel::IBox& box, const size_t index) { return true; };
				//bool onOutputAdded(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onOutputRemoved(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onOutputTypeChanged(Kernel::IBox& box, const size_t index) override override { return true; };
				//bool onOutputNameChanged(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onSettingAdded(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onSettingRemoved(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onSettingTypeChanged(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onSettingNameChanged(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onSettingDefaultValueChanged(Kernel::IBox& box, const size_t index) override { return true; };
				//bool onSettingValueChanged(Kernel::IBox& box, const size_t index) override { return true; };

				_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
			};
			*/

			/// <summary> Descriptor of the box AdaptativeClassifier. </summary>
			class CBoxAlgorithmAdaptativeClassifierDesc final : virtual public IBoxAlgorithmDesc
			{
			public:

				void release() override { }

				CString getName() const override { return CString("AdaptativeClassifier"); }
				CString getAuthorName() const override { return CString("Tristan Venot"); }
				CString getAuthorCompanyName() const override { return CString("INRIA"); }
				CString getShortDescription() const override { return CString("Classifier Trainer and processor"); }
				CString getDetailedDescription() const override { return CString("Takes the element of the trainer and the processor in order to relearn during the online session"); }
				CString getCategory() const override { return CString("Classification"); }
				CString getVersion() const override { return CString("0.1"); }
				

				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_AdaptativeClassifier; }
				IPluginObject* create() override { return new CBoxAlgorithmAdaptativeClassifier; }

				/*
				IBoxListener* createBoxListener() const override { return new CBoxAlgorithmAdaptativeClassifierListener; }
				void releaseBoxListener(IBoxListener* listener) const override { delete listener; }
				*/
				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				
				{
					prototype.addInput("Stimulations", OV_TypeId_Stimulations);
					prototype.addInput("Features for class 1", OV_TypeId_FeatureVector);
					prototype.addInput("Features for class 2", OV_TypeId_FeatureVector);
					prototype.addInput("Features Online", OV_TypeId_FeatureVector);

					prototype.addOutput("Train-completed Flag", OV_TypeId_Stimulations);
					//prototype.addOutput("Labels", OV_TypeId_Stimulations);
					prototype.addOutput("Hyperplane distance", OV_TypeId_StreamedMatrix);
					//prototype.addOutput("Probability values", OV_TypeId_StreamedMatrix);
					prototype.addSetting(TRAIN_TRIGGER_SETTING_NAME, OV_TypeId_Stimulation, "OVTK_StimulationId_Train");
					prototype.addSetting(FILENAME_SETTING_NAME, OV_TypeId_Filename, "${Path_UserData}/my-classifier.xml");

					
					//Argument of algorithm
					prototype.addSetting("Use Shrinkage", OV_TypeId_Boolean,"False");
					prototype.addSetting("Shrinkage: Force diagonal cov (DDA)", OV_TypeId_Boolean, "False");
					prototype.addSetting("Shrinkage coefficient (-1 == auto)", OV_TypeId_Float,"1");

					prototype.addSetting(FOLD_SETTING_NAME, OV_TypeId_Integer, "10");
					prototype.addSetting(BALANCE_SETTING_NAME, OV_TypeId_Boolean, "false");
					prototype.addSetting("Class 1 Label",OV_TypeId_Stimulation, "OVTK_GDF_Left");
					prototype.addSetting("Class 2 Label",OV_TypeId_Stimulation, "OVTK_GDF_Right");
					prototype.addSetting("Adaptive", OV_TypeId_Boolean, "false");
					prototype.addSetting("Incrementation Supervised Learning (1), Incrementation Kalman(2), Incrementation Covariance (3) ", OV_TypeId_Integer, "0");
					prototype.addSetting("Coeff Supervised Learning in %", OV_TypeId_Integer, "5");
					prototype.addSetting("Number of features in the training set", OV_TypeId_Integer, "0");

					
					
					
					return true;
				}
				_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_AdaptativeClassifierDesc)
			};
		}  // namespace Classification
	}  // namespace Plugins
}  // namespace OpenViBE
