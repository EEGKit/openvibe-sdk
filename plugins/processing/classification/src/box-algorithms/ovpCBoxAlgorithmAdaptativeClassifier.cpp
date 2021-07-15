#include "ovpCBoxAlgorithmAdaptativeClassifier.h"

#include <system/ovCMath.h>

#include <xml/IXMLHandler.h>
#include <xml/IXMLNode.h>

#include <sstream>
#include <cmath>
#include <algorithm>
#include <fstream>
#include <map>

#include <iomanip> 
#if defined TARGET_HAS_ThirdPartyEIGEN

#include <iostream>


#include <xml/IXMLHandler.h>

#include <Eigen/Eigenvalues>

#include "../algorithms/ovpCAlgorithmConditionedCovariance.h"
// setw
//This needs to reachable from outside
/*
const char* const CLASSIFIER_ROOT = "OpenViBE-Classifier";
const char* const FORMAT_VERSION_ATTRIBUTE_NAME = "FormatVersion";



const char* const IDENTIFIER_ATTRIBUTE_NAME = "class-id";
const char* const STRATEGY_NODE_NAME = "Strategy-Identifier";
const char* const ALGORITHM_NODE_NAME = "Algorithm-Identifier";
const char* const STIMULATIONS_NODE_NAME = "Stimulations";
const char* const REJECTED_CLASS_NODE_NAME = "Rejected-Class";
const char* const CLASS_STIMULATION_NODE_NAME = "Class-Stimulation";
const char* const CLASSIFICATION_BOX_ROOT = "OpenViBE-Classifier-Box";
*/
using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::*/Toolkit;
const char* const CREATOR_ATTRIBUTE_NAME_1 = "Venot";
const char* const CREATOR_VERSION_ATTRIBUTE_NAME_1 = "VenotV1";
using namespace /*OpenViBE::*/Plugins;
using namespace Classification;
using namespace std;

using namespace Eigen;

/*
namespace
{
	const char* const TYPE_NODE_NAME = "LDA_Adaptive";
	const char* const CLASSES_NODE_NAME = "Classes";
	//const char* const COEFFICIENTS_NODE_NAME = "Weights";
	//const char* const BIAS_DISTANCE_NODE_NAME = "Bias-distance";
	//const char* const COEFFICIENT_PROBABILITY_NODE_NAME = "Coefficient-probability";
	const char* const COMPUTATION_HELPERS_CONFIGURATION_NODE = "Class-config-list";
	const char* const LDA_CONFIG_FILE_VERSION_ATTRIBUTE_NAME = "version";
}

extern const char* const CLASSIFIER_ROOT;
*/
namespace OpenViBE {
	namespace Plugins {
		namespace Classification {

			void CBoxAlgorithmAdaptativeClassifier::setStimulation(const size_t /*stimulationIndex*/, const uint64_t identifier, const uint64_t /*stimulationDate*/)
			{
				/*
				OVTK_GDF_Start_Of_Trial
				OVTK_GDF_Cross_On_Screen
				OVTK_GDF_Left
				OVTK_GDF_Right

				We only compute the mean and std if we are in the adequate conditions (ex : OVTK_GDF_LEFT/RIGHT)


				*/

				bool stateUpdated = false;
				Classify = false;
				switch (identifier)
				{
				case OVTK_GDF_Feedback_Continuous:
					Classify = true;
					stateUpdated = true;
					break;
				case OVTK_GDF_Left:
					//Classify = true;
					New_trial_left += 1;
					stateUpdated = true;
					break;
				case OVTK_GDF_Right:
					//Classify = true;
					New_trial_right += 1;
					stateUpdated = true;
					break;


				default: break;
				}
			}

			/*******************************************************************************/

			/*
			uint64_t CBoxAlgorithmClassification::getClockFrequency()
			{
				// Note that the time is coded on a 64 bits unsigned integer, fixed decimal point (32:32)
				return 1LL<<32; // the box clock frequency
			}
			*/
			/*******************************************************************************/


			int Classification::LDAClassificationCompare_Adaptive(IMatrix& first, IMatrix& second)
			{
				//We first need to find the best classification of each.
				double* buffer = first.getBuffer();
				const double maxFirst = *(std::max_element(buffer, buffer + first.getBufferElementCount()));

				buffer = second.getBuffer();
				const double maxSecond = *(std::max_element(buffer, buffer + second.getBufferElementCount()));

				//Then we just compared them
				if (OVFloatEqual(maxFirst, maxSecond)) { return 0; }
				if (maxFirst > maxSecond) { return -1; }
				return 1;
			}





			bool CBoxAlgorithmAdaptativeClassifier::initialize()
			{
				// LDA :
				// Initialize the Conditioned Covariance Matrix algorithm
				m_covAlgorithm = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ConditionedCovariance));
				New_trial_left = 0;
				New_trial_right = 0;
				OV_ERROR_UNLESS_KRF(m_covAlgorithm->initialize(), "Failed to initialize covariance algorithm", ErrorType::Internal);

				// This is the weight parameter local to this module and automatically exposed to the GUI. Its redirected to the corresponding parameter of the cov alg.
				ip_shrinkage = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

				shrinkage = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
				ip_diagonalCov = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
				Adaptation = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 9);

				//Processor : 
				const CString configFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
				OV_ERROR_UNLESS_KRF(configFilename != CString(""), "Invalid empty configuration file name", ErrorType::BadConfig);


				m_sampleDecoder_Processor.initialize(*this, 3);
				//m_stimDecoder_Processor.initialize(*this, 0);
				m_encoder_Train.initialize(*this, 0);
				//m_labelsEncoder_Processor.initialize(*this, 1);
				m_hyperplanesEncoder_Processor.initialize(*this, 1);
				//m_probabilitiesEncoder_Processor.initialize(*this, 3);
				// Trainer : 
				m_classifier_Train = nullptr;
				m_parameter_Train = nullptr;

				Covariance_Initial = openData("CovarianceMatrix.csv");
				std::cout << Covariance_Initial << "\n";
				X_Mean_Initial_1 = openData("X_MeanClass_1.csv");
				std::cout << X_Mean_Initial_1 << "\n";

				X_Mean_Initial_2 = openData("X_MeanClass_2.csv");
				std::cout << X_Mean_Initial_2 << "\n";
				Samples_size_beginning = openData("Samples.csv");
				const IBox& boxContext = this->getStaticBoxContext();

				m_trainStimulation_Train = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0));

				const int64_t nPartition = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);

				OV_ERROR_UNLESS_KRF(nPartition >= 0, "Invalid partition count [" << nPartition << "] (expected value >= 0)", ErrorType::BadSetting);

				m_nPartition_Train = uint64_t(nPartition);

				m_stimDecoder_Train.initialize(*this, 0);
				for (size_t i = 1; i < 3; ++i)
				{
					m_sampleDecoder_Train.push_back(new TFeatureVectorDecoder<CBoxAlgorithmAdaptativeClassifier>());
					m_sampleDecoder_Train.back()->initialize(*this, i);
				}

				//We link the parameters to the extra parameters input parameter to transmit them



				m_nFeatures_Train.clear();
				Distance = 0.0;
				OV_ERROR_UNLESS_KRF(boxContext.getInputCount() >= 2, "Invalid input count [" << boxContext.getInputCount() << "] (at least 2 input expected)",
					ErrorType::BadSetting);

				// Provide the number of classes to the classifier
				m_oMatrix = m_hyperplanesEncoder_Processor.getInputMatrix();
				const size_t nClass = boxContext.getInputCount() - 2;
				TrainingSupervised = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 10));
				SupervisedCoeff = double(this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 11))) / 100;
				std::cout << SupervisedCoeff << "\n";
				Number_Features = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 12));
				return true;
			}
			/*******************************************************************************/

			bool CBoxAlgorithmAdaptativeClassifier::uninitialize()
			{
				//LDA :
				OV_ERROR_UNLESS_KRF(m_covAlgorithm->uninitialize(), "Failed to uninitialize covariance algorithm", ErrorType::Internal);
				this->getAlgorithmManager().releaseAlgorithm(*m_covAlgorithm);



				//Processor :
				if (m_classifier_Processor)
				{
					m_classifier_Processor->uninitialize();
					this->getAlgorithmManager().releaseAlgorithm(*m_classifier_Processor);
					m_classifier_Processor = nullptr;
				}

				//m_probabilitiesEncoder_Processor.uninitialize();
				m_hyperplanesEncoder_Processor.uninitialize();
				//m_labelsEncoder_Processor.uninitialize();

				m_stimDecoder_Processor.uninitialize();
				m_sampleDecoder_Processor.uninitialize();

				// Trainer :

				m_stimDecoder_Train.uninitialize();
				m_encoder_Train.uninitialize();

				if (m_classifier_Train)
				{
					m_classifier_Train->uninitialize();
					this->getAlgorithmManager().releaseAlgorithm(*m_classifier_Train);
				}

				for (size_t i = 0; i < m_sampleDecoder_Train.size(); ++i)
				{
					m_sampleDecoder_Train[i]->uninitialize();
					delete m_sampleDecoder_Train[i];
				}
				m_sampleDecoder_Train.clear();

				//m_encoder_Train.uninitialize();
				m_stimDecoder_Train.uninitialize();

				for (size_t i = 0; i < m_datasets_Train.size(); ++i)
				{
					delete m_datasets_Train[i].sampleMatrix;
					m_datasets_Train[i].sampleMatrix = nullptr;
				}
				m_datasets_Train.clear();

				if (m_parameter_Train)
				{
					delete m_parameter_Train;
					m_parameter_Train = nullptr;
				}
				return true;
			}
			/*******************************************************************************/
			bool CBoxAlgorithmAdaptativeClassifier::processInput(const size_t /*index*/)
			{
				getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
				return true;
			}

			bool CBoxAlgorithmAdaptativeClassifier::process()
			{
				// Train : 

				IBoxIO& boxContext = this->getDynamicBoxContext();
				const size_t nInput = this->getStaticBoxContext().getInputCount();

				bool startTrain = false;

				for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
				{
					m_stimDecoder_Train.decode(i);

					if (m_stimDecoder_Train.isHeaderReceived())
					{
						m_encoder_Train.encodeHeader();
						boxContext.markOutputAsReadyToSend(0, 0, 0);
					}
					if (m_stimDecoder_Train.isBufferReceived())
					{
						const IStimulationSet* iStimulationSet = m_stimDecoder_Train.getOutputStimulationSet();
						IStimulationSet* oStimulationSet = m_encoder_Train.getInputStimulationSet();
						oStimulationSet->clear();

						for (size_t j = 0; j < iStimulationSet->getStimulationCount(); ++j)
						{
							setStimulation(j, iStimulationSet->getStimulationIdentifier(j), iStimulationSet->getStimulationDate(j));
							if (iStimulationSet->getStimulationIdentifier(j) == m_trainStimulation_Train)
							{

								if (Adaptation == false)
									startTrain = true;
								if (Adaptation == true && New_trial_left > m_nPartition_Train && New_trial_right > m_nPartition_Train)
									startTrain = true;
								const uint64_t id = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, "OVTK_StimulationId_TrainCompleted");
								oStimulationSet->appendStimulation(id, iStimulationSet->getStimulationDate(j), 0);
							}

						}
						m_encoder_Train.encodeBuffer();

						boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
					}
					if (m_stimDecoder_Train.isEndReceived())
					{
						m_encoder_Train.encodeEnd();
						boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
					}
				}


				// Classify data
				for (size_t j = 0; j < boxContext.getInputChunkCount(3); ++j)
				{
					m_sampleDecoder_Processor.decode(j);
					const uint64_t startTime = boxContext.getInputChunkStartTime(3, j);
					const uint64_t endTime = boxContext.getInputChunkEndTime(3, j);

					if (m_sampleDecoder_Processor.isHeaderReceived())
					{


						const IMatrix* input = m_sampleDecoder_Processor.getOutputMatrix();
						m_oMatrix = m_hyperplanesEncoder_Processor.getInputMatrix();
						m_oMatrix->setDimensionCount(2);
						m_oMatrix->setDimensionSize(0, input->getDimensionSize(0));
						m_oMatrix->setDimensionSize(1, 1);

						Toolkit::Matrix::clearContent(*m_oMatrix);	// Set to 0
						m_hyperplanesEncoder_Processor.encodeHeader();
						boxContext.markOutputAsReadyToSend(1, startTime, endTime);



					}
					if (m_sampleDecoder_Processor.isBufferReceived())
					{
						if (Classify == true)
						{
							const IMatrix* sampleMatrix = m_sampleDecoder_Processor.getOutputMatrix();
							m_oMatrix = m_hyperplanesEncoder_Processor.getInputMatrix();
							sample_t sample_proces;
							sample_proces.sampleMatrix = new CMatrix();
							sample_proces.startTime = boxContext.getInputChunkStartTime(3, j);
							sample_proces.endTime = boxContext.getInputChunkEndTime(3, j);
							//sample.inputIdx = j - 1;

							Toolkit::Matrix::copy(*sample_proces.sampleMatrix, *sampleMatrix);
							//double* ibuffer = sample_proces.sampleMatrix->getBuffer();
							Eigen::MatrixXd Covariance_inter = Covariance_Initial;
							Covariance_inter = classify(sample_proces, Covariance_Initial);
							Covariance_Initial = Covariance_inter;
							this->getLogManager() << LogLevel_Trace << Distance << "\n";
							double* buffer = m_oMatrix->getBuffer();

							for (size_t j = 0; j < sampleMatrix->getBufferElementCount(); ++j) { buffer[j] = Distance; }



						}
						m_hyperplanesEncoder_Processor.encodeBuffer();
						boxContext.markOutputAsReadyToSend(1, startTime, endTime);


					}
					if (m_sampleDecoder_Processor.isEndReceived())
					{


						m_hyperplanesEncoder_Processor.encodeEnd();


						boxContext.markOutputAsReadyToSend(1, startTime, endTime);

					}


				}

				for (size_t i = 1; i < nInput - 1; ++i)
				{
					for (size_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
					{
						m_sampleDecoder_Train[i - 1]->decode(j);

						if (m_sampleDecoder_Train[i - 1]->isHeaderReceived()) {}
						if (m_sampleDecoder_Train[i - 1]->isBufferReceived())
						{
							const IMatrix* sampleMatrix = m_sampleDecoder_Train[i - 1]->getOutputMatrix();

							sample_t sample;
							sample.sampleMatrix = new CMatrix();
							sample.startTime = boxContext.getInputChunkStartTime(i, j);
							sample.endTime = boxContext.getInputChunkEndTime(i, j);
							sample.inputIdx = i - 1;

							Toolkit::Matrix::copy(*sample.sampleMatrix, *sampleMatrix);
							m_datasets_Train.push_back(sample);
							m_nFeatures_Train[i]++;
						}
						if (m_sampleDecoder_Train[i - 1]->isEndReceived()) {}
					}
				}


				if ((startTrain && (Adaptation == false)) || (startTrain && TrainingSupervised == 3))
				{
					OV_ERROR_UNLESS_KRF(m_datasets_Train.size() >= m_nPartition_Train,
						"Received fewer examples (" << m_datasets_Train.size() << ") than specified partition count (" << m_nPartition_Train << ")",
						ErrorType::BadInput);

					OV_ERROR_UNLESS_KRF(!m_datasets_Train.empty(), "No training example received", ErrorType::BadInput);

					this->getLogManager() << LogLevel_Info << "Received train stimulation. Data dim is [" << m_datasets_Train.size() << "x"
						<< m_datasets_Train[0].sampleMatrix->getBufferElementCount() << "]\n";
					for (size_t i = 1; i < nInput; ++i)
					{
						this->getLogManager() << LogLevel_Info << "For information, we have " << m_nFeatures_Train[i] << " feature vector(s) for input " << i << "\n";
					}

					const bool balancedDataset = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 6);
					if (balancedDataset) { balanceDataset(); }

					const std::vector<sample_t>& actualDataset = (balancedDataset ? m_balancedDatasets_Train : m_datasets_Train);

					vector<double> partitionAccuracies(m_nPartition_Train);

					const bool randomizeVectorOrder = this->getConfigurationManager().expandAsBoolean("${Plugin_Classification_RandomizeKFoldTestData}", false);

					// create a vector used for mapping feature vectors (initialize it as v[i] = i)
					std::vector<size_t> featurePermutation;
					for (size_t i = 0; i < actualDataset.size(); ++i) { featurePermutation.push_back(i); }

					// randomize the vector if necessary
					if (randomizeVectorOrder)
					{
						this->getLogManager() << LogLevel_Info << "Randomizing the feature vector set\n";
						random_shuffle(featurePermutation.begin(), featurePermutation.end(), System::Math::randomWithCeiling);
					}

					const size_t nClass = nInput - 1;
					CMatrix confusion;
					confusion.setDimensionCount(2);
					confusion.setDimensionSize(0, nClass);
					confusion.setDimensionSize(1, nClass);

					if (m_nPartition_Train >= 2)
					{
						double partitionAccuracy = 0;
						double finalAccuracy = 0;

						Toolkit::Matrix::clearContent(confusion);

						this->getLogManager() << LogLevel_Info << "k-fold test could take quite a long time, be patient\n";
						for (size_t i = 0; i < m_nPartition_Train; ++i)
						{
							this->getLogManager() << LogLevel_Info << m_nPartition_Train << "\n";
							const size_t startIdx = size_t(((i)*actualDataset.size()) / m_nPartition_Train);
							const size_t stopIdx = size_t(((i + 1) * actualDataset.size()) / m_nPartition_Train);

							this->getLogManager() << LogLevel_Trace << "Training on partition " << i << " (feature vectors " << startIdx << " to " <<
								stopIdx - 1 << ")...\n";

							train(actualDataset, featurePermutation, startIdx, stopIdx);
						}



						//printConfusionMatrix(confusion);
					}
					else
					{
						this->getLogManager() << LogLevel_Info << "Training without cross-validation.\n";
						this->getLogManager() << LogLevel_Info << "*** Reported training set accuracy will be optimistic ***\n";
					}


					this->getLogManager() << LogLevel_Trace << "Training final classifier on the whole set...\n";

					//OV_ERROR_UNLESS_KRF(this->train(actualDataset, featurePermutation, 0, 0),
									//	"Training failed: bailing out (from whole set training)", ErrorType::Internal);


					//printConfusionMatrix(confusion);

					//OV_ERROR_UNLESS_KRF(this->saveConfig(), "Failed to save configuration", ErrorType::Internal);
				}

				return true;
			}



			bool CBoxAlgorithmAdaptativeClassifier::train(const std::vector<sample_t>& dataset, const std::vector<size_t>& permutation, const size_t startIdx,
				const size_t stopIdx)
			{
				if (Adaptation == false || TrainingSupervised == 3 || TrainingSupervised == 1)
				{
					const size_t nSample = dataset.size() - (stopIdx - startIdx);
					const size_t nFeature = dataset[0].sampleMatrix->getBufferElementCount();
					this->getLogManager() << LogLevel_Info << dataset.size() << "\n";
					this->getLogManager() << LogLevel_Info << dataset[0].sampleMatrix->getBufferElementCount() << "\n";

					const bool useShrinkage = shrinkage;
					std::vector<double> sum_class_1(nFeature, 0.0);
					std::vector<double> mean_class_1(nFeature, 0.0);
					typedef  Eigen::Matrix<double, 1, Dynamic> MatrixType;
					//MatrixType buf(nFeature);

					typedef Map<MatrixType> MapType;
					std::vector<double> sum_class_2(nFeature, 0.0);
					std::vector<double> mean_class_2(nFeature, 0.0);




					bool diagonalCov;
					Eigen::MatrixXd Covariance_class1 = MatrixXd::Constant(nFeature, nFeature, 0.0);
					Eigen::MatrixXd Covariance_class2 = MatrixXd::Constant(nFeature, nFeature, 0.0);
					Eigen::MatrixXd Covariance = MatrixXd::Constant(nFeature, nFeature, 0.0);
					Eigen::MatrixXd Covariance_between_Class = MatrixXd::Constant(nFeature, nFeature, 0.0);
					Eigen::MatrixXd ProductScatterMatrix = MatrixXd::Constant(nFeature, nFeature, 0.0);
					MapType X_sum_class_1(sum_class_1.data(), nFeature);
					MapType X_sum_class_2(sum_class_2.data(), nFeature);
					Eigen::VectorXd X_mean_class_1;
					Eigen::VectorXd X_mean_class_2;


					Eigen::MatrixXd X_mean_class_1_save;
					Eigen::MatrixXd X_mean_class_2_save;

					Eigen::VectorXd X_Mean_Initial_1_inter;
					Eigen::VectorXd X_Mean_Initial_2_inter;

					if (useShrinkage)
					{

						diagonalCov = ip_diagonalCov;
					}
					size_t number_Samples = 0;


					for (size_t i = 1; i < 3; ++i)
					{
						number_Samples = number_Samples + m_nFeatures_Train[i];
						if (i == 1)
						{
							if (Adaptation == false)
							{
								for (size_t j = 0; j < number_Samples; j++)
								{
									double* buffer = dataset[j].sampleMatrix->getBuffer();
									for (size_t k = 0; k < nFeature; k++)
									{
										sum_class_1[k] = sum_class_1[k] + buffer[k];

									}

								}
								MapType X_sum_class_1(sum_class_1.data(), nFeature);
								X_mean_class_1 = X_sum_class_1 / number_Samples;
								//std::cout << "Mean1 = " << X_mean_class_1 << "\n";
								X_mean_class_1_save = X_mean_class_1;

							}

							if (Adaptation == true)
							{
								double* buffer = dataset[number_Samples - 1].sampleMatrix->getBuffer();
								for (size_t k = 0; k < nFeature; k++)
								{
									sum_class_1[k] = buffer[k];

								}

								MapType X_sum_class_1(sum_class_1.data(), nFeature);
								X_Mean_Initial_1 = (1 - SupervisedCoeff) * X_Mean_Initial_1 + SupervisedCoeff * X_sum_class_1;

							}



							for (size_t j = 0; j < number_Samples; j++)
							{
								double* buffer = dataset[j].sampleMatrix->getBuffer();

								MapType X_vec(buffer, nFeature);
								Eigen::VectorXd x_vec = X_vec;
								//Eigen::VectorXd X_mean_class_1;
								//Map<ArrayXd> X_mean_class_1(mean_class_1.data(), nFeature);

								if (Adaptation == false)
									Covariance_class1 = Covariance_class1 + ((x_vec - X_mean_class_1) * (x_vec - X_mean_class_1).transpose());
								//std::cout << "cov1 = " << Covariance_class1 << "\n";
								if (Adaptation == true)
								{
									X_Mean_Initial_1_inter = X_Mean_Initial_1;
									Covariance_class1 = Covariance_class1 + ((x_vec - X_Mean_Initial_1_inter) * (x_vec - X_Mean_Initial_1_inter).transpose());
								}


							}
							//std::cout << "cov1 = " << Covariance_class1 << "\n";
							//this->getLogManager() << LogLevel_Info << "Xmean" << (X_vec-X_mean_class_1).size() << "\n";




						}

						if (i == 2)
						{
							if (Adaptation == false)
							{
								for (size_t j = m_nFeatures_Train[i - 1]; j < number_Samples; j++)
								{
									double* buffer = dataset[j].sampleMatrix->getBuffer();
									for (size_t k = 0; k < nFeature; k++)
									{
										sum_class_2[k] = sum_class_2[k] + buffer[k];
									}

								}
								MapType X_sum_class_2(sum_class_2.data(), nFeature);
								X_mean_class_2 = X_sum_class_2 / m_nFeatures_Train[i];
								X_mean_class_2_save = X_mean_class_2;
								//std::cout << "Mean2 = " << X_mean_class_2 << "\n";
							}

							if (Adaptation == true)
							{
								double* buffer = dataset[number_Samples - 1].sampleMatrix->getBuffer();
								for (size_t k = 0; k < nFeature; k++)
								{
									sum_class_2[k] = buffer[k];

								}
								MapType X_sum_class_2(sum_class_2.data(), nFeature);
								X_Mean_Initial_2 = (1 - SupervisedCoeff) * X_Mean_Initial_2 + SupervisedCoeff * X_sum_class_2;
							}

							for (size_t j = m_nFeatures_Train[i - 1]; j < number_Samples; j++)
							{
								double* buffer = dataset[j].sampleMatrix->getBuffer();

								MapType X_vec(buffer, nFeature);
								Eigen::VectorXd x_vec = X_vec;
								//Eigen::VectorXd X_mean_class_1;
								//Map<ArrayXd> X_mean_class_1(mean_class_1.data(), nFeature);
								if (Adaptation == false)
								{
									Covariance_class2 = Covariance_class2 + ((x_vec - X_mean_class_2) * (x_vec - X_mean_class_2).transpose());
								}
								//std::cout << "Cov2 = " << Covariance_class2;
								if (Adaptation == true)
								{
									X_Mean_Initial_2_inter = X_Mean_Initial_2;
									Covariance_class2 = Covariance_class2 + ((x_vec - X_Mean_Initial_2_inter) * (x_vec - X_Mean_Initial_2_inter).transpose());
								}
							}



						}

						Covariance = Covariance_class1 / (Samples_size_beginning(1) + m_nFeatures_Train[1]) + Covariance_class2 / (Samples_size_beginning(2) + m_nFeatures_Train[2]);
						//MapType X_mean_class_2(sum_class_1.data(), nFeature);
						//std::cout << Covariance << "\n";

					}
					/*
					for (size_t i = 1; i < 3; ++i)
					{
						if (i == 1)
						{
							Covariance_between_Class = Covariance_between_Class + m_nFeatures_Train[i] * (X_mean_class_1 - (X_sum_class_1 + X_sum_class_2) / dataset.size()) * (X_mean_class_1 - (X_sum_class_1 + X_sum_class_2) / dataset.size()).transpose();

						}

						if (i == 2)
						{
							Covariance_between_Class = Covariance_between_Class + m_nFeatures_Train[i] * (X_mean_class_2 - (X_sum_class_1 + X_sum_class_2) / dataset.size()) * (X_mean_class_2 - (X_sum_class_1 + X_sum_class_2) / dataset.size()).transpose();

						}

					}*/


					/* 2 OPTIONS TO DO NORMAL STANDARD COVARIANCE CALCULATION , OR EIGEN DECOMPOSITION,  AND SCATTER MATRIX */
					/*ProductScatterMatrix = Covariance.inverse() * Covariance_between_Class;
					SelfAdjointEigenSolver<MatrixXd> eigensolver(ProductScatterMatrix);
					std::vector<double> index;
					std::vector<int> index_sorting;
					index.resize(eigensolver.eigenvalues().size());
					index_sorting.resize(eigensolver.eigenvalues().size());
					VectorXd::Map(&index[0], eigensolver.eigenvalues().size()) = eigensolver.eigenvalues();
					std::sort(index.begin(), index.end());*/
					/*
					for (size_t l = 0; l < index.size(); l++)
					{
						for (size_t k = 0; k < index.size(); k++)
						{
							if (abs(index[l] - eigensolver.eigenvalues()[k]) <= 0.000001)
							{
								index_sorting[l] = k;
							}
						}
					*/
					/*
					const double tolerance = 1e-10;
					SelfAdjointEigenSolver<MatrixXd> solver;
					solver.compute(Covariance);
					VectorXd eigenValues = solver.eigenvalues();
					for (size_t i = 0; i < nFeature; ++i) { if (eigenValues(i) >= tolerance) { eigenValues(i) = 1.0 / eigenValues(i); } }
					const MatrixXd globalCovInv = solver.eigenvectors() * eigenValues.asDiagonal() * solver.eigenvectors().inverse();
					*/
					const MatrixXd globalCovInv = Covariance.inverse();
					//std::cout << "cov = " << Covariance_Initial << "\n";
					if (TrainingSupervised == 3)
					{
						Covariance_Initial = globalCovInv;
						//std::cout << "cov = " << Covariance_Initial << "\n";
					}

					//std::cout << "Cov = " << globalCovInv << "\n";


					//std::vector<MatrixXd> classMeans = { X_mean_class_1, X_mean_class_2};

					/*
					for (size_t i = 0; i < 2; ++i)
					{
						const double examplesInClass = m_nFeatures_Train[i + 1];
						if (examplesInClass > 0)
						{


							// This formula e.g. in Hastie, Tibshirani & Friedman: "Elements...", 2nd ed., p. 109
							const VectorXd weigth = (globalCovInv * classMeans[i]);

							const MatrixXd inter = -0.5 * classMeans[i].transpose() * globalCovInv * classMeans[i];

							const double bias = inter(0, 0) + std::log(examplesInClass / nSample);

							//this->getLogManager() << LogLevel_Debug << "Bias for " << i << " is " << bias << ", from " << examplesInClass / nSample
								//<< ", " << examplesInClass << "/" << nSample << ", int = " << inter(0, 0) << "\n";
							// dumpMatrix(this->getLogManager(), perClassMeans[i], "Means");

							//m_discriminantFunctions[i].setWeight(weigth);
							//m_discriminantFunctions[i].setBias(bias);
						}
						else { this->getLogManager() << LogLevel_Debug << "Class " << i << " has no examples\n"; }
					}*/

					if (Adaptation == false)
					{
						const static IOFormat CSVFormat(FullPrecision, DontAlignCols, ",", "\n");
						std::ofstream file("CovarianceMatrix.csv");
						if (file.is_open())
						{
							file << globalCovInv.format(CSVFormat);
							file.close();
						}



						std::ofstream file_X_1("X_MeanClass_1.csv");
						if (file_X_1.is_open())
						{
							file_X_1 << X_mean_class_1_save.transpose().format(CSVFormat);
							file_X_1.close();
						}


						std::ofstream file_X_2("X_MeanClass_2.csv");
						if (file_X_2.is_open())
						{
							file_X_2 << X_mean_class_2_save.transpose().format(CSVFormat);
							file_X_2.close();
						}

						std::ofstream file_Samples("Samples.csv");
						Vector3d sample_to_write(m_nFeatures_Train[1] + m_nFeatures_Train[2], m_nFeatures_Train[1], m_nFeatures_Train[2]);
						if (file_Samples.is_open())
						{
							file_Samples << sample_to_write.format(CSVFormat);
							file_Samples.close();
						}


					}

				}
				return true;


			}



			Eigen::MatrixXd CBoxAlgorithmAdaptativeClassifier::classify(sample_t sample_proces, Eigen::MatrixXd Covariance)
			{
				//const Map<VectorXd> featureVec(const_cast<double*>(sample.getBuffer()), sample.getSize());
				typedef  Eigen::Matrix<double, 1, Dynamic> MatrixType;
				typedef Map<MatrixType> MapType;
				const size_t nFeature = sample_proces.sampleMatrix->getBufferElementCount();
				double* ibuffer = sample_proces.sampleMatrix->getBuffer();
				MapType X_vec(ibuffer, nFeature);
				Eigen::VectorXd x_vec = X_vec;
				//std::cout << "X_1" << x_vec << "\n";
				Eigen::MatrixXd weight = Covariance * (X_Mean_Initial_2 - X_Mean_Initial_1).transpose();
				//std::cout << "weight" << weight << "\n";
				Eigen::MatrixXd Mean_X = 0.5 * (X_Mean_Initial_1 + X_Mean_Initial_2);
				//std::cout << "mean" << Mean_X << "\n";
				Eigen::MatrixXd b = -weight.transpose() * Mean_X.transpose();
				//std::cout << "b" << b << "\n";
				Eigen::MatrixXd Covariance_final = Covariance;
				Eigen::MatrixXd Distance_test = b + weight.transpose() * x_vec;
				//std::cout << "D :" << Distance_test << "\n";

				Distance = Distance_test(0, 0);
				if (TrainingSupervised == 1)
				{
					Eigen::VectorXd v = Covariance * x_vec;
					//std::cout << "coeff :" << (1 - SupervisedCoeff) / SupervisedCoeff << "\n";
					//std::cout << "v :" <<  x_vec.transpose() * v << "\n";
					MatrixXd I = Covariance - (v * v.transpose()) / ((1 - SupervisedCoeff) / SupervisedCoeff + x_vec.transpose() * v);
					//std::cout << "vector" << I << "\n";
					Covariance_final = I / (1 - SupervisedCoeff);

				}


				/*
				std::vector<MatrixXd> classMeans = { X_Mean_Initial_1, X_Mean_Initial_2 };

				for (size_t i = 0; i < 2; ++i)
				{
					const double examplesInClass = Number_Features/2;
					if (examplesInClass > 0)
					{


						// This formula e.g. in Hastie, Tibshirani & Friedman: "Elements...", 2nd ed., p. 109
						const VectorXd weigth = (globalCovInv * classMeans[i]);

						const MatrixXd inter = -0.5 * classMeans[i].transpose() * globalCovInv * classMeans[i];

						const double bias = inter(0, 0) + std::log(examplesInClass / Number_Features);

						//this->getLogManager() << LogLevel_Debug << "Bias for " << i << " is " << bias << ", from " << examplesInClass / nSample
							//<< ", " << examplesInClass << "/" << nSample << ", int = " << inter(0, 0) << "\n";
						// dumpMatrix(this->getLogManager(), perClassMeans[i], "Means");

						//m_discriminantFunctions[i].setWeight(weigth);
						//m_discriminantFunctions[i].setBias(bias);
					}
					else { this->getLogManager() << LogLevel_Debug << "Class " << i << " has no examples\n"; }
				}
				*/




				return Covariance_final;
			}

			

			bool CBoxAlgorithmAdaptativeClassifier::balanceDataset()
			{
				const IBox& boxContext = this->getStaticBoxContext();
				const size_t nClass = boxContext.getInputCount() - 1;

				this->getLogManager() << LogLevel_Info << "Balancing dataset...\n";

				// Collect index set of feature vectors per class
				std::vector<std::vector<size_t>> classIndexes;
				classIndexes.resize(nClass);
				for (size_t i = 0; i < m_datasets_Train.size(); ++i) { classIndexes[m_datasets_Train[i].inputIdx].push_back(i); }

				// Count how many vectors the largest class has
				size_t nMax = 0;
				for (size_t i = 0; i < nClass; ++i) { nMax = std::max<size_t>(nMax, classIndexes[i].size()); }

				m_balancedDatasets_Train.clear();

				// Pad those classes with resampled examples (sampling with replacement) that have fewer examples than the largest class
				for (size_t i = 0; i < nClass; ++i)
				{
					const size_t examplesInClass = classIndexes[i].size();
					const size_t paddingNeeded = nMax - examplesInClass;
					if (examplesInClass == 0)
					{
						this->getLogManager() << LogLevel_Debug << "Cannot resample class " << i << ", 0 examples\n";
						continue;
					}
					if (paddingNeeded > 0) { this->getLogManager() << LogLevel_Debug << "Padding class " << i << " with " << paddingNeeded << " examples\n"; }

					// Copy all the examples first to a temporary array so we don't mess with the original data.
					// This is not too bad as instead of data, we copy the pointer. m_datasets owns the data pointer.
					const std::vector<size_t>& thisClassesIndexes = classIndexes[i];
					for (size_t j = 0; j < examplesInClass; ++j) { m_balancedDatasets_Train.push_back(m_datasets_Train[thisClassesIndexes[j]]); }

					for (size_t j = 0; j < paddingNeeded; ++j)
					{
						const size_t sampledIndex = System::Math::randomWithCeiling(examplesInClass);
						const sample_t& sourceVector = m_datasets_Train[thisClassesIndexes[sampledIndex]];
						m_balancedDatasets_Train.push_back(sourceVector);
					}
				}

				return true;
			}


			/*******************************************************************************/


			bool CBoxAlgorithmAdaptativeClassifier::loadConfig(XML::IXMLNode* configNode)
			{

				return true;
			}
#define LDA_DEBUG 0
#if LDA_DEBUG
			void CBoxAlgorithmAdaptativeClassifier::dumpMatrix(Kernel::ILogManager& pMgr, const MatrixXdRowMajor& mat, const CString& desc)
			{
				rMgr << LogLevel_Info << desc << "\n";
				for (int i = 0; i < mat.rows(); i++)
				{
					rMgr << LogLevel_Info << "Row " << i << ": ";
					for (int j = 0; j < mat.cols(); j++) { rMgr << mat(i, j) << " "; }
					rMgr << "\n";
				}
			}

#else
			void CBoxAlgorithmAdaptativeClassifier::dumpMatrix(ILogManager& /* rMgr */, const MatrixXdRowMajor& /*mat*/, const CString& /*desc*/) { }
#endif
			void CBoxAlgorithmAdaptativeClassifier::loadClassesFromNode(XML::IXMLNode* node)
			{
				std::stringstream ss(node->getPCData());
				double value;
				while (ss >> value) { m_labels.push_back(value); }
				m_nClasses = m_labels.size();
			}

			void CBoxAlgorithmAdaptativeClassifier::loadCoefsFromNode(XML::IXMLNode* node)
			{
				std::stringstream ss(node->getPCData());

				std::vector<double> coefs;
				double value;
				while (ss >> value) { coefs.push_back(value); }

				m_weights.resize(1, coefs.size());
				m_nCols = coefs.size();
				for (size_t i = 0; i < coefs.size(); ++i) { m_weights(0, i) = coefs[i]; }
			}


			Eigen::MatrixXd  CBoxAlgorithmAdaptativeClassifier::openData(std::string fileToOpen)
			{

				typedef  Eigen::Matrix<double, Dynamic, Dynamic> MatrixType;
				typedef Map<MatrixType> MapType;

				vector<double> matrixEntries;

				// in this object we store the data from the matrix
				ifstream matrixDataFile(fileToOpen);
				if (matrixDataFile.is_open())
				{
					this->getLogManager() << LogLevel_Info << "Opening\n";
					// this variable is used to store the row of the matrix that contains commas 
					string matrixRowString;

					// this variable is used to store the matrix entry;
					string matrixEntry;

					// this variable is used to track the number of rows
					size_t matrixRowNumber = 0;


					while (getline(matrixDataFile, matrixRowString)) // here we read a row by row of matrixDataFile and store every line into the string variable matrixRowString
					{
						stringstream matrixRowStringStream(matrixRowString); //convert matrixRowString that is a string to a stream variable.

						while (getline(matrixRowStringStream, matrixEntry, ',')) // here we read pieces of the stream matrixRowStringStream until every comma, and store the resulting character into the matrixEntry
						{
							matrixEntries.push_back(stod(matrixEntry));   //here we convert the string to double and fill in the row vector storing all the matrix entries
							//this->getLogManager() << LogLevel_Info << "mat"  << matrixEntry <<"\n";
						}
						matrixRowNumber++; //update the row numbers
					}
					//Eigen::MatrixXd Mat_Test(matrixRowNumber, matrixRowNumber);
					//Eigen::Map < Eigen::Matrix<double, 64, 64>> Mat_Cov(matrixEntries.data());
					//X_Mean_Initial_1 = Mat_Cov;
					// here we convet the vector variable into the matrix and return the resulting object, 
					// note that matrixEntries.data() is the pointer to the first memory location at which the entries of the vector matrixEntries are stored;
					MapType Mat_Test(matrixEntries.data(), matrixRowNumber, matrixEntries.size() / matrixRowNumber);
					return Mat_Test;
				}
				this->getLogManager() << LogLevel_Info << "No open \n";
				return  MatrixXd::Constant(2, 2, 0.0);

			}

		}
	}
}



#endif // TARGET_HAS_ThirdPartyEIGEN