#include "ovpCBoxAlgorithmClassifierTrainer.h"

#include <system/ovCMemory.h>
#include <system/ovCMath.h>

#include <sstream>
#include <cmath>
#include <algorithm>

#include <map>

#include <iomanip> // setw

#include <xml/IXMLHandler.h>
#include <xml/IXMLNode.h>

//This needs to reachable from outside
const char* const CLASSIFIER_ROOT                = "OpenViBE-Classifier";
const char* const FORMAT_VERSION_ATTRIBUTE_NAME  = "FormatVersion";
const char* const CREATOR_ATTRIBUTE_NAME         = "Creator";
const char* const CREATOR_VERSION_ATTRIBUTE_NAME = "CreatorVersion";
const char* const IDENTIFIER_ATTRIBUTE_NAME      = "class-id";
const char* const STRATEGY_NODE_NAME             = "Strategy-Identifier";
const char* const ALGORITHM_NODE_NAME            = "Algorithm-Identifier";
const char* const STIMULATIONS_NODE_NAME         = "Stimulations";
const char* const REJECTED_CLASS_NODE_NAME       = "Rejected-Class";
const char* const CLASS_STIMULATION_NODE_NAME    = "Class-Stimulation";
const char* const CLASSIFICATION_BOX_ROOT        = "OpenViBE-Classifier-Box";

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;
using namespace std;

bool CBoxAlgorithmClassifierTrainer::initialize()
{
	m_pClassifier = nullptr;
	m_pParameter  = nullptr;

	const IBox& boxContext = this->getStaticBoxContext();
	//As we add some parameter in the middle of "static" parameters, we cannot rely on settings index.
	m_pParameter = new map<CString, CString>();
	for (uint32_t i = 0; i < boxContext.getSettingCount(); ++i)
	{
		CString name;
		boxContext.getSettingName(i, name);
		const CString value           = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i);
		(*m_pParameter)[name] = value;
	}

	bool isPairing = false;

	const CString configurationFilename(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2));

	OV_ERROR_UNLESS_KRF(configurationFilename != CString(""), "Invalid empty configuration filename", OpenViBE::Kernel::ErrorType::BadSetting);

	CIdentifier l_oClassifierAlgorithmClassIdentifier;

	CIdentifier l_oStrategyClassIdentifier = this->getTypeManager().getEnumerationEntryValueFromName(OVTK_TypeId_ClassificationStrategy, (*m_pParameter)[MULTICLASS_STRATEGY_SETTING_NAME]);
	l_oClassifierAlgorithmClassIdentifier  = this->getTypeManager().getEnumerationEntryValueFromName(OVTK_TypeId_ClassificationAlgorithm, (*m_pParameter)[ALGORITHM_SETTING_NAME]);

	if (l_oStrategyClassIdentifier == OV_UndefinedIdentifier)
	{
		//That means that we want to use a classical algorithm so just let's create it
		const CIdentifier l_oClassifierAlgorithmIdentifier = this->getAlgorithmManager().createAlgorithm(l_oClassifierAlgorithmClassIdentifier);

		OV_ERROR_UNLESS_KRF(l_oClassifierAlgorithmIdentifier != OV_UndefinedIdentifier,
							"Unable to instantiate classifier for class [" << l_oClassifierAlgorithmIdentifier.toString() << "]",
							OpenViBE::Kernel::ErrorType::BadConfig);

		m_pClassifier = &this->getAlgorithmManager().getAlgorithm(l_oClassifierAlgorithmIdentifier);
		m_pClassifier->initialize();
	}
	else
	{
		isPairing  = true;
		m_pClassifier = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(l_oStrategyClassIdentifier));
		m_pClassifier->initialize();
	}
	m_ui64TrainStimulation = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, (*m_pParameter)[TRAIN_TRIGGER_SETTING_NAME]);

	int64_t nPartition = this->getConfigurationManager().expandAsInteger((*m_pParameter)[FOLD_SETTING_NAME]);

	OV_ERROR_UNLESS_KRF(nPartition >= 0, "Invalid partition count [" << nPartition << "] (expected value >= 0)", OpenViBE::Kernel::ErrorType::BadSetting);

	m_ui64PartitionCount = uint64_t(nPartition);

	m_oStimulationDecoder.initialize(*this, 0);
	for (uint32_t i = 1; i < boxContext.getInputCount(); i++)
	{
		m_vFeatureVectorDecoder.push_back(new OpenViBEToolkit::TFeatureVectorDecoder<CBoxAlgorithmClassifierTrainer>());
		m_vFeatureVectorDecoder.back()->initialize(*this, i);
	}

	//We link the parameters to the extra parameters input parameter to transmit them
	TParameterHandler<map<CString, CString> *> ip_pExtraParameter(m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter));
	ip_pExtraParameter = m_pParameter;

	m_oStimulationEncoder.initialize(*this, 0);

	m_vFeatureCount.clear();

	OV_ERROR_UNLESS_KRF(boxContext.getInputCount() >= 2, "Invalid input count [" << boxContext.getInputCount() << "] (at least 2 input expected)", OpenViBE::Kernel::ErrorType::BadSetting);

	// Provide the number of classes to the classifier
	const uint32_t nClass = boxContext.getInputCount() - 1;
	TParameterHandler<uint64_t> ip_NumClasses(m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));
	ip_NumClasses = nClass;

	//If we have to deal with a pairing strategy we have to pass argument
	if (isPairing)
	{
		TParameterHandler<CIdentifier*> ip_oClassId(m_pClassifier->getInputParameter(OVTK_Algorithm_PairingStrategy_InputParameterId_SubClassifierAlgorithm));
		ip_oClassId = &l_oClassifierAlgorithmClassIdentifier;

		OV_ERROR_UNLESS_KRF(m_pClassifier->process(OVTK_Algorithm_PairingStrategy_InputTriggerId_DesignArchitecture), "Failed to design architecture", OpenViBE::Kernel::ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmClassifierTrainer::uninitialize()
{
	m_oStimulationDecoder.uninitialize();
	m_oStimulationEncoder.uninitialize();

	if (m_pClassifier)
	{
		m_pClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pClassifier);
	}

	for (uint32_t i = 0; i < m_vFeatureVectorDecoder.size(); i++)
	{
		m_vFeatureVectorDecoder[i]->uninitialize();
		delete m_vFeatureVectorDecoder[i];
	}
	m_vFeatureVectorDecoder.clear();

	m_oStimulationEncoder.uninitialize();

	m_oStimulationDecoder.uninitialize();

	for (uint32_t i = 0; i < m_vDataset.size(); i++)
	{
		delete m_vDataset[i].m_pFeatureVectorMatrix;
		m_vDataset[i].m_pFeatureVectorMatrix = nullptr;
	}
	m_vDataset.clear();

	if (m_pParameter)
	{
		delete m_pParameter;
		m_pParameter = nullptr;
	}

	// @fixme who frees this? freeing here -> crash
	/*
	if(m_pExtraParameter != nullptr)
	{
		delete m_pExtraParameter;
		m_pExtraParameter = NULL;
	}
	*/

	return true;
}

bool CBoxAlgorithmClassifierTrainer::processInput(const uint32_t index)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

// Find the most likely class and resample the dataset so that each class is as likely
bool CBoxAlgorithmClassifierTrainer::balanceDataset()
{
	const IBox& boxContext = this->getStaticBoxContext();

	const uint32_t nClass = boxContext.getInputCount() - 1;

	this->getLogManager() << LogLevel_Info << "Balancing dataset...\n";

	// Collect index set of feature vectors per class
	std::vector<std::vector<size_t>> classIndexes;
	classIndexes.resize(nClass);
	for (size_t i = 0; i < m_vDataset.size(); i++) { classIndexes[m_vDataset[i].m_ui32InputIndex].push_back(i); }

	// Count how many vectors the largest class has
	uint32_t nMax = 0;
	for (uint32_t i = 0; i < nClass; i++)
	{
		nMax = std::max<uint32_t>(nMax, classIndexes[i].size());
	}

	m_vBalancedDataset.clear();

	// Pad those classes with resampled examples (sampling with replacement) that have fewer examples than the largest class
	for (uint32_t i = 0; i < nClass; i++)
	{
		const uint32_t examplesInClass = classIndexes[i].size();
		const uint32_t paddingNeeded   = nMax - examplesInClass;
		if (examplesInClass == 0)
		{
			this->getLogManager() << LogLevel_Debug << "Cannot resample class " << i << ", 0 examples\n";
			continue;
		}
		if (paddingNeeded > 0)
		{
			this->getLogManager() << LogLevel_Debug << "Padding class " << i << " with " << paddingNeeded << " examples\n";
		}

		// Copy all the examples first to a temporary array so we don't mess with the original data.
		// This is not too bad as instead of data, we copy the pointer. m_vDataset owns the data pointer.
		const std::vector<size_t>& thisClassesIndexes = classIndexes[i];
		for (uint32_t j = 0; j < examplesInClass; j++) { m_vBalancedDataset.push_back(m_vDataset[thisClassesIndexes[j]]); }

		for (uint32_t j = 0; j < paddingNeeded; j++)
		{
			const uint32_t sampledIndex     = System::Math::randomUInteger32WithCeiling(examplesInClass);
			const SFeatureVector& sourceVector = m_vDataset[thisClassesIndexes[sampledIndex]];
			m_vBalancedDataset.push_back(sourceVector);
		}
	}

	return true;
}

bool CBoxAlgorithmClassifierTrainer::process()
{
	IBoxIO& boxContext  = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	bool startTrain = false;

	// Parses stimulations
	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		m_oStimulationDecoder.decode(i);

		if (m_oStimulationDecoder.isHeaderReceived())
		{
			m_oStimulationEncoder.encodeHeader();
			boxContext.markOutputAsReadyToSend(0, 0, 0);
		}
		if (m_oStimulationDecoder.isBufferReceived())
		{
			const IStimulationSet* iStimulationSet = m_oStimulationDecoder.getOutputStimulationSet();
			IStimulationSet* oStimulationSet      = m_oStimulationEncoder.getInputStimulationSet();
			oStimulationSet->clear();

			for (uint64_t j = 0; j < iStimulationSet->getStimulationCount(); j++)
			{
				if (iStimulationSet->getStimulationIdentifier(j) == m_ui64TrainStimulation)
				{
					startTrain = true;
					const uint64_t stimId = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, "OVTK_StimulationId_TrainCompleted");
					oStimulationSet->appendStimulation(stimId, iStimulationSet->getStimulationDate(j), 0);
				}
			}
			m_oStimulationEncoder.encodeBuffer();

			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
		if (m_oStimulationDecoder.isEndReceived())
		{
			m_oStimulationEncoder.encodeEnd();
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
	}

	// Parses feature vectors
	for (uint32_t i = 1; i < nInput; i++)
	{
		for (uint32_t j = 0; j < boxContext.getInputChunkCount(i); j++)
		{
			m_vFeatureVectorDecoder[i - 1]->decode(j);

			if (m_vFeatureVectorDecoder[i - 1]->isHeaderReceived()) { }
			if (m_vFeatureVectorDecoder[i - 1]->isBufferReceived())
			{
				const IMatrix* pFeatureVectorMatrix = m_vFeatureVectorDecoder[i - 1]->getOutputMatrix();

				SFeatureVector featureVector;
				featureVector.m_pFeatureVectorMatrix = new CMatrix();
				featureVector.m_ui64StartTime        = boxContext.getInputChunkStartTime(i, j);
				featureVector.m_ui64EndTime          = boxContext.getInputChunkEndTime(i, j);
				featureVector.m_ui32InputIndex       = i - 1;

				OpenViBEToolkit::Tools::Matrix::copy(*featureVector.m_pFeatureVectorMatrix, *pFeatureVectorMatrix);
				m_vDataset.push_back(featureVector);
				m_vFeatureCount[i]++;
			}
			if (m_vFeatureVectorDecoder[i - 1]->isEndReceived()) { }
		}
	}

	// On train stimulation reception, build up the labelled feature vector set matrix and go on training
	if (startTrain)
	{
		OV_ERROR_UNLESS_KRF(m_vDataset.size() >= m_ui64PartitionCount,
							"Received fewer examples (" << uint32_t(m_vDataset.size()) << ") than specified partition count (" << m_ui64PartitionCount << ")",
							OpenViBE::Kernel::ErrorType::BadInput);

		OV_ERROR_UNLESS_KRF(!m_vDataset.empty(), "No training example received", OpenViBE::Kernel::ErrorType::BadInput);

		this->getLogManager() << LogLevel_Info << "Received train stimulation. Data dim is [" << uint32_t(m_vDataset.size()) << "x"
				<< m_vDataset[0].m_pFeatureVectorMatrix->getBufferElementCount() << "]\n";
		for (uint32_t i = 1; i < nInput; i++)
		{
			this->getLogManager() << LogLevel_Info << "For information, we have " << m_vFeatureCount[i] << " feature vector(s) for input " << i << "\n";
		}

		const bool balancedDataset = this->getConfigurationManager().expandAsBoolean((*m_pParameter)[BALANCE_SETTING_NAME]);
		if (balancedDataset) { balanceDataset(); }

		const std::vector<SFeatureVector>& actualDataset = (balancedDataset ? m_vBalancedDataset : m_vDataset);

		vector<double> partitionAccuracies(static_cast<unsigned int>(m_ui64PartitionCount));

		const bool randomizeVectorOrder = this->getConfigurationManager().expandAsBoolean("${Plugin_Classification_RandomizeKFoldTestData}", false);

		// create a vector used for mapping feature vectors (initialize it as v[i] = i)
		std::vector<size_t> featurePermutation;
		for (size_t i = 0; i < actualDataset.size(); i++) { featurePermutation.push_back(i); }

		// randomize the vector if necessary
		if (randomizeVectorOrder)
		{
			this->getLogManager() << LogLevel_Info << "Randomizing the feature vector set\n";
			random_shuffle(featurePermutation.begin(), featurePermutation.end(), System::Math::randomUInteger32WithCeiling);
		}

		const uint32_t nClass = nInput - 1;
		CMatrix confusion;
		confusion.setDimensionCount(2);
		confusion.setDimensionSize(0, nClass);
		confusion.setDimensionSize(1, nClass);

		if (m_ui64PartitionCount >= 2)
		{
			double partitionAccuracy = 0;
			double finalAccuracy     = 0;

			OpenViBEToolkit::Tools::Matrix::clearContent(confusion);

			this->getLogManager() << LogLevel_Info << "k-fold test could take quite a long time, be patient\n";
			for (size_t i = 0; i < m_ui64PartitionCount; i++)
			{
				const size_t startIdx = size_t(((i) * actualDataset.size()) / m_ui64PartitionCount);
				const size_t stopIdx  = size_t(((i + 1) * actualDataset.size()) / m_ui64PartitionCount);

				this->getLogManager() << LogLevel_Trace << "Training on partition " << i << " (feature vectors " << uint32_t(startIdx) << " to " << uint32_t(stopIdx) - 1 << ")...\n";

				OV_ERROR_UNLESS_KRF(this->train(actualDataset, featurePermutation, startIdx, stopIdx),
									"Training failed: bailing out (from xval)", OpenViBE::Kernel::ErrorType::Internal);

				partitionAccuracy                  = this->getAccuracy(actualDataset, featurePermutation, startIdx, stopIdx, confusion);
				partitionAccuracies[i] = partitionAccuracy;
				finalAccuracy += partitionAccuracy;

				this->getLogManager() << LogLevel_Info << "Finished with partition " << i + 1 << " / " << m_ui64PartitionCount << " (performance : " << partitionAccuracy << "%)\n";
			}

			const double mean = finalAccuracy / m_ui64PartitionCount;
			double deviation  = 0;

			for (size_t i = 0; i < m_ui64PartitionCount; i++)
			{
				const double diff = partitionAccuracies[i] - mean;
				deviation += diff * diff;
			}
			deviation = sqrt(deviation / m_ui64PartitionCount);

			this->getLogManager() << LogLevel_Info << "Cross-validation test accuracy is " << mean << "% (sigma = " << deviation << "%)\n";

			printConfusionMatrix(confusion);
		}
		else
		{
			this->getLogManager() << LogLevel_Info << "Training without cross-validation.\n";
			this->getLogManager() << LogLevel_Info << "*** Reported training set accuracy will be optimistic ***\n";
		}


		this->getLogManager() << LogLevel_Trace << "Training final classifier on the whole set...\n";

		OV_ERROR_UNLESS_KRF(this->train(actualDataset, featurePermutation, 0, 0),
							"Training failed: bailing out (from whole set training)", OpenViBE::Kernel::ErrorType::Internal);

		OpenViBEToolkit::Tools::Matrix::clearContent(confusion);
		const double l_f64TrainAccuracy = this->getAccuracy(actualDataset, featurePermutation, 0, actualDataset.size(), confusion);

		this->getLogManager() << LogLevel_Info << "Training set accuracy is " << l_f64TrainAccuracy << "% (optimistic)\n";

		printConfusionMatrix(confusion);

		OV_ERROR_UNLESS_KRF(this->saveConfiguration(), "Failed to save configuration", OpenViBE::Kernel::ErrorType::Internal);
	}

	return true;
}

bool CBoxAlgorithmClassifierTrainer::train(const std::vector<SFeatureVector>& rDataset, const std::vector<size_t>& rPermutation, const size_t startIndex, const size_t stopIndex)
{
	OV_ERROR_UNLESS_KRF(stopIndex - startIndex != 1, "Invalid indexes: stopIndex - trainIndex = 1", OpenViBE::Kernel::ErrorType::BadArgument);

	const uint32_t nFeatureVector = rDataset.size() - (stopIndex - startIndex);
	const uint32_t featureVectorSize  = rDataset[0].m_pFeatureVectorMatrix->getBufferElementCount();

	TParameterHandler<IMatrix*> ip_pFeatureVectorSet(m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet));

	ip_pFeatureVectorSet->setDimensionCount(2);
	ip_pFeatureVectorSet->setDimensionSize(0, nFeatureVector);
	ip_pFeatureVectorSet->setDimensionSize(1, featureVectorSize + 1);

	double* l_pFeatureVectorSetBuffer = ip_pFeatureVectorSet->getBuffer();
	for (size_t j = 0; j < rDataset.size() - (stopIndex - startIndex); j++)
	{
		const size_t k       = rPermutation[(j < startIndex ? j : j + (stopIndex - startIndex))];
		const double classId = double(rDataset[k].m_ui32InputIndex);
		System::Memory::copy(l_pFeatureVectorSetBuffer, rDataset[k].m_pFeatureVectorMatrix->getBuffer(), featureVectorSize * sizeof(double));

		l_pFeatureVectorSetBuffer[featureVectorSize] = classId;
		l_pFeatureVectorSetBuffer += (featureVectorSize + 1);
	}

	OV_ERROR_UNLESS_KRF(m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Train), "Training failed", OpenViBE::Kernel::ErrorType::Internal);

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	XML::IXMLNode* l_pTempNode = static_cast<XML::IXMLNode*>(op_pConfiguration);

	if (l_pTempNode != nullptr) { l_pTempNode->release(); }
	op_pConfiguration = nullptr;

	return m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_SaveConfiguration);
}

// Note that this function is incremental for oConfusionMatrix and can be called many times; so we don't clear the matrix
double CBoxAlgorithmClassifierTrainer::getAccuracy(const std::vector<SFeatureVector>& rDataset, const std::vector<size_t>& rPermutation,
												   const size_t uiStartIndex, const size_t uiStopIndex, CMatrix& oConfusionMatrix)
{
	OV_ERROR_UNLESS_KRF(uiStopIndex != uiStartIndex, "Invalid indexes: start index equals stop index", OpenViBE::Kernel::ErrorType::BadArgument);

	const uint32_t l_ui32FeatureVectorSize = rDataset[0].m_pFeatureVectorMatrix->getBufferElementCount();

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	XML::IXMLNode* l_pNode = op_pConfiguration;//Requested for affectation
	TParameterHandler<XML::IXMLNode*> ip_pConfiguration(m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
	ip_pConfiguration = l_pNode;

	m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration);

	TParameterHandler<IMatrix*> ip_pFeatureVector(m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector));
	TParameterHandler<double> op_f64ClassificationStateClass(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));
	ip_pFeatureVector->setDimensionCount(1);
	ip_pFeatureVector->setDimensionSize(0, l_ui32FeatureVectorSize);

	size_t l_iSuccessCount = 0;

	for (size_t j = uiStartIndex; j < uiStopIndex; j++)
	{
		const size_t k = rPermutation[j];

		double* featureVectorBuffer = ip_pFeatureVector->getBuffer();
		const double correctValue = double(rDataset[k].m_ui32InputIndex);

		this->getLogManager() << LogLevel_Debug << "Try to recognize " << correctValue << "\n";

		System::Memory::copy(featureVectorBuffer, rDataset[k].m_pFeatureVectorMatrix->getBuffer(), l_ui32FeatureVectorSize * sizeof(double));

		m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify);

		const double predictedValue = op_f64ClassificationStateClass;

		this->getLogManager() << LogLevel_Debug << "Recognize " << predictedValue << "\n";

		if (predictedValue == correctValue) { l_iSuccessCount++; }

		if (predictedValue < oConfusionMatrix.getDimensionSize(0) && correctValue < oConfusionMatrix.getDimensionSize(0))
		{
			double* buf = oConfusionMatrix.getBuffer();
			buf[uint32_t(correctValue) * oConfusionMatrix.getDimensionSize(1) + uint32_t(predictedValue)] += 1.0;
		}
		else { std::cout << "errorn\n"; }
	}

	return double((l_iSuccessCount * 100.0) / (uiStopIndex - uiStartIndex));
}

bool CBoxAlgorithmClassifierTrainer::printConfusionMatrix(const CMatrix& oMatrix)
{
	OV_ERROR_UNLESS_KRF(oMatrix.getDimensionCount() == 2 && oMatrix.getDimensionSize(0) == oMatrix.getDimensionSize(1),
						"Invalid confution matrix [dim count = " << oMatrix.getDimensionCount() << ", dim size 0 = "
						<< oMatrix.getDimensionSize(0) << ", dim size 1 = "<< oMatrix.getDimensionSize(1) << "] (expected 2 dimensions with same size)",
						OpenViBE::Kernel::ErrorType::BadArgument);

	const uint32_t rows = oMatrix.getDimensionSize(0);

	if (rows > 10 && !this->getConfigurationManager().expandAsBoolean("${Plugin_Classification_ForceConfusionMatrixPrint}"))
	{
		this->getLogManager() << LogLevel_Info << "Over 10 classes, not printing the confusion matrix. If needed, override with setting Plugin_Classification_ForceConfusionMatrixPrint token to true.\n";
		return true;
	}

	// Normalize
	CMatrix tmp, rowSum;
	OpenViBEToolkit::Tools::Matrix::copy(tmp, oMatrix);
	rowSum.setDimensionCount(1);
	rowSum.setDimensionSize(0, rows);
	OpenViBEToolkit::Tools::Matrix::clearContent(rowSum);

	for (uint32_t i = 0; i < rows; i++)
	{
		for (uint32_t j = 0; j < rows; j++) { rowSum[i] += tmp[i * rows + j]; }
		for (uint32_t j = 0; j < rows; j++) { tmp[i * rows + j] /= rowSum[i]; }
	}

	std::stringstream ss;
	ss << std::fixed;

	ss << "  Cls vs cls ";
	for (uint32_t i = 0; i < rows; i++) { ss << setw(6) << (i + 1); }
	this->getLogManager() << LogLevel_Info << ss.str().c_str() << "\n";

	ss.precision(1);
	for (uint32_t i = 0; i < rows; i++)
	{
		ss.str("");
		ss << "  Target " << setw(2) << (i + 1) << ": ";
		for (uint32_t j = 0; j < rows; j++)
		{
			ss << setw(6) << tmp[i * rows + j] * 100;
		}
		this->getLogManager() << LogLevel_Info << ss.str().c_str() << " %, " << uint32_t(rowSum[i]) << " examples\n";
	}

	return true;
}

bool CBoxAlgorithmClassifierTrainer::saveConfiguration()
{
	const IBox& boxContext = this->getStaticBoxContext();

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	XML::IXMLNode* algorithmConfigurationNode = XML::createNode(CLASSIFIER_ROOT);
	algorithmConfigurationNode->addChild(static_cast<XML::IXMLNode*>(op_pConfiguration));

	XML::IXMLHandler* handler = XML::createXMLHandler();
	CString configurationFilename(this->getConfigurationManager().expand((*m_pParameter)[FILENAME_SETTING_NAME]));

	const CString strategyClassIdentifier            = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const CString classifierAlgorithmClassIdentifier = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	XML::IXMLNode* root = XML::createNode(CLASSIFICATION_BOX_ROOT);
	std::stringstream version;
	version << OVP_Classification_BoxTrainerFormatVersion;
	root->addAttribute(FORMAT_VERSION_ATTRIBUTE_NAME, version.str().c_str());

	const auto cleanup = [&]()
	{
		handler->release();
		root->release();
		op_pConfiguration = nullptr;
	};
	root->addAttribute(CREATOR_ATTRIBUTE_NAME, this->getConfigurationManager().expand("${Application_Name}"));
	root->addAttribute(CREATOR_VERSION_ATTRIBUTE_NAME, this->getConfigurationManager().expand("${Application_Version}"));

	XML::IXMLNode* tempNode             = XML::createNode(STRATEGY_NODE_NAME);
	const CIdentifier strategyClassId = this->getTypeManager().getEnumerationEntryValueFromName(OVTK_TypeId_ClassificationStrategy, (*m_pParameter)[MULTICLASS_STRATEGY_SETTING_NAME]);
	tempNode->addAttribute(IDENTIFIER_ATTRIBUTE_NAME, strategyClassId.toString());
	tempNode->setPCData((*m_pParameter)[MULTICLASS_STRATEGY_SETTING_NAME].toASCIIString());
	root->addChild(tempNode);

	tempNode                            = XML::createNode(ALGORITHM_NODE_NAME);
	const CIdentifier classifierClassId = this->getTypeManager().getEnumerationEntryValueFromName(OVTK_TypeId_ClassificationAlgorithm, (*m_pParameter)[ALGORITHM_SETTING_NAME]);
	tempNode->addAttribute(IDENTIFIER_ATTRIBUTE_NAME, classifierClassId.toString());
	tempNode->setPCData((*m_pParameter)[ALGORITHM_SETTING_NAME].toASCIIString());
	root->addChild(tempNode);


	XML::IXMLNode* stimulationsNode = XML::createNode(STIMULATIONS_NODE_NAME);

	for (uint32_t i = 1; i < boxContext.getInputCount(); ++i)
	{
		char bufferSettingName[64];
		sprintf(bufferSettingName, "Class %d label", i);

		tempNode = XML::createNode(CLASS_STIMULATION_NODE_NAME);
		std::stringstream buffer;
		buffer << (i - 1);
		tempNode->addAttribute(IDENTIFIER_ATTRIBUTE_NAME, buffer.str().c_str());
		tempNode->setPCData((*m_pParameter)[bufferSettingName].toASCIIString());
		stimulationsNode->addChild(tempNode);
	}
	root->addChild(stimulationsNode);

	root->addChild(algorithmConfigurationNode);

	if (!handler->writeXMLInFile(*root, configurationFilename.toASCIIString()))
	{
		cleanup();
		OV_ERROR_KRF("Failed saving configuration to file [" << configurationFilename << "]", OpenViBE::Kernel::ErrorType::BadFileWrite);
	}

	cleanup();
	return true;
}
