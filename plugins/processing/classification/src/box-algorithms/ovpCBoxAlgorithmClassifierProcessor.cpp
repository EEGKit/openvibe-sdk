#include "ovpCBoxAlgorithmClassifierProcessor.h"

#include <sstream>

#include <xml/IXMLHandler.h>
#include <xml/IXMLNode.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace Classification;
using namespace std;

bool CBoxAlgorithmClassifierProcessor::loadClassifier(const char* filename)
{
	if (m_classifier)
	{
		m_classifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_classifier);
		m_classifier = nullptr;
	}

	XML::IXMLHandler* handler = XML::createXMLHandler();
	XML::IXMLNode* rootNode   = handler->parseFile(filename);

	OV_ERROR_UNLESS_KRF(rootNode, "Unable to get xml root node from file at " << filename, Kernel::ErrorType::BadParsing);

	m_stimulations.clear();

	// Check the version of the file
	OV_ERROR_UNLESS_KRF(rootNode->hasAttribute(FORMAT_VERSION_ATTRIBUTE_NAME), "Configuration file [" << filename << "] has no version information",
						Kernel::ErrorType::ResourceNotFound);

	std::stringstream data(rootNode->getAttribute(FORMAT_VERSION_ATTRIBUTE_NAME));
	size_t version;
	data >> version;

	OV_WARNING_UNLESS_K(version <= OVP_Classification_BoxTrainerFormatVersion,
						"Classifier configuration in [" << filename << "] saved using a newer version: saved version = [" << version
						<< "] vs current version = [" << OVP_Classification_BoxTrainerFormatVersion << "]");

	OV_ERROR_UNLESS_KRF(version >= OVP_Classification_BoxTrainerFormatVersionRequired,
						"Classifier configuration in [" << filename << "] saved using an obsolete version [" << version << "] (minimum expected version = "
						<< OVP_Classification_BoxTrainerFormatVersionRequired << ")", Kernel::ErrorType::BadVersion);

	CIdentifier algorithmClassID = CIdentifier::undefined();

	XML::IXMLNode* tmp = rootNode->getChildByName(STRATEGY_NODE_NAME);

	OV_ERROR_UNLESS_KRF(tmp, "Configuration file [" << filename << "] has no node " << STRATEGY_NODE_NAME, Kernel::ErrorType::BadParsing);

	algorithmClassID.fromString(tmp->getAttribute(IDENTIFIER_ATTRIBUTE_NAME));

	//If the Identifier is undefined, that means we need to load a native algorithm
	if (algorithmClassID == CIdentifier::undefined())
	{
		tmp = rootNode->getChildByName(ALGORITHM_NODE_NAME);

		OV_ERROR_UNLESS_KRF(tmp, "Configuration file [" << filename << "] has no node " << ALGORITHM_NODE_NAME, Kernel::ErrorType::BadParsing);

		algorithmClassID.fromString(tmp->getAttribute(IDENTIFIER_ATTRIBUTE_NAME));

		//If the algorithm is still unknown, that means that we face an error
		OV_ERROR_UNLESS_KRF(algorithmClassID != CIdentifier::undefined(), "No classifier retrieved from configuration file [" << filename << "]",
							Kernel::ErrorType::BadConfig);
	}

	//Now loading all stimulations output
	XML::IXMLNode* stimNode = rootNode->getChildByName(STIMULATIONS_NODE_NAME);

	OV_ERROR_UNLESS_KRF(stimNode, "Configuration file [" << filename << "] has no node " << STIMULATIONS_NODE_NAME, Kernel::ErrorType::BadParsing);

	//Now load every stimulation and store them in the map with the right class id
	for (size_t i = 0; i < stimNode->getChildCount(); ++i)
	{
		tmp = stimNode->getChild(i);

		OV_ERROR_UNLESS_KRF(tmp, "Invalid NULL child node " << i << " for node [" << STIMULATIONS_NODE_NAME << "]", Kernel::ErrorType::BadParsing);

		CString name(tmp->getPCData());

		double classID;
		const char* att = tmp->getAttribute(IDENTIFIER_ATTRIBUTE_NAME);

		OV_ERROR_UNLESS_KRF(att, "Invalid child node " << i << " for node [" << STIMULATIONS_NODE_NAME << "]: attribute ["
							<< IDENTIFIER_ATTRIBUTE_NAME << "] not found", Kernel::ErrorType::BadParsing);

		std::stringstream ss(att);
		ss >> classID;
		m_stimulations[classID] = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, name);
	}

	const CIdentifier id = this->getAlgorithmManager().createAlgorithm(algorithmClassID);

	OV_ERROR_UNLESS_KRF(id != CIdentifier::undefined(),
						"Invalid classifier algorithm with id [" << algorithmClassID.str() << "] in configuration file [" << filename << "]",
						Kernel::ErrorType::BadConfig);

	m_classifier = &this->getAlgorithmManager().getAlgorithm(id);
	m_classifier->initialize();

	// Connect the params to the new classifier

	TParameterHandler<CMatrix*> ip_sample = m_classifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector);
	ip_sample.setReferenceTarget(m_sampleDecoder.getOutputMatrix());

	m_hyperplanesEncoder.getInputMatrix().
						 setReferenceTarget(m_classifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));
	m_probabilitiesEncoder.getInputMatrix().setReferenceTarget(m_classifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
	// note: labelsencoder cannot be directly bound here as the classifier returns a float, but we need to output a stimulation

	TParameterHandler<XML::IXMLNode*> ip_classificationConfig(m_classifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Config));
	ip_classificationConfig = rootNode->getChildByName(CLASSIFIER_ROOT)->getChild(0);

	OV_ERROR_UNLESS_KRF(m_classifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfig),
						"Loading configuration failed for subclassifier [" << id.str() << "]", Kernel::ErrorType::Internal);

	rootNode->release();
	handler->release();

	return true;
}

bool CBoxAlgorithmClassifierProcessor::initialize()
{
	m_classifier = nullptr;

	//First of all, let's get the XML file for configuration
	const CString configFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	OV_ERROR_UNLESS_KRF(configFilename != CString(""), "Invalid empty configuration file name", Kernel::ErrorType::BadConfig);

	m_sampleDecoder.initialize(*this, 0);
	m_stimDecoder.initialize(*this, 1);

	m_labelsEncoder.initialize(*this, 0);
	m_hyperplanesEncoder.initialize(*this, 1);
	m_probabilitiesEncoder.initialize(*this, 2);

	return loadClassifier(configFilename.toASCIIString());
}

bool CBoxAlgorithmClassifierProcessor::uninitialize()
{
	if (m_classifier)
	{
		m_classifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_classifier);
		m_classifier = nullptr;
	}

	m_probabilitiesEncoder.uninitialize();
	m_hyperplanesEncoder.uninitialize();
	m_labelsEncoder.uninitialize();

	m_stimDecoder.uninitialize();
	m_sampleDecoder.uninitialize();

	return true;
}

bool CBoxAlgorithmClassifierProcessor::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmClassifierProcessor::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	// Check if we have a command first
	for (size_t i = 0; i < boxContext.getInputChunkCount(1); ++i)
	{
		m_stimDecoder.decode(i);
		if (m_stimDecoder.isHeaderReceived()) { }
		if (m_stimDecoder.isBufferReceived())
		{
			CStimulationSet& set = *m_stimDecoder.getOutputStimulationSet();
			for (size_t j = 0; j < set.size(); ++j)
			{
				if (set[j].m_ID == OVTK_StimulationId_TrainCompleted)
				{
					CString configFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
					if (!loadClassifier(configFilename.toASCIIString())) { return false; }
				}
			}
		}
		if (m_stimDecoder.isEndReceived()) { }
	}

	// Classify data
	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		const CTime startTime = boxContext.getInputChunkStartTime(0, i);
		const CTime endTime   = boxContext.getInputChunkEndTime(0, i);

		m_sampleDecoder.decode(i);
		if (m_sampleDecoder.isHeaderReceived())
		{
			m_labelsEncoder.encodeHeader();
			m_hyperplanesEncoder.encodeHeader();
			m_probabilitiesEncoder.encodeHeader();

			boxContext.markOutputAsReadyToSend(0, startTime, endTime);
			boxContext.markOutputAsReadyToSend(1, startTime, endTime);
			boxContext.markOutputAsReadyToSend(2, startTime, endTime);
		}
		if (m_sampleDecoder.isBufferReceived())
		{
			OV_ERROR_UNLESS_KRF(m_classifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify)
								&& m_classifier->isOutputTriggerActive(OVTK_Algorithm_Classifier_OutputTriggerId_Success),
								"Classification failed", Kernel::ErrorType::Internal);

			TParameterHandler<double> op_classificationState(m_classifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));

			CStimulationSet& set = *m_labelsEncoder.getInputStimulationSet();

			set.clear();
			set.append(m_stimulations[op_classificationState], endTime, 0);

			m_labelsEncoder.encodeBuffer();
			m_hyperplanesEncoder.encodeBuffer();
			m_probabilitiesEncoder.encodeBuffer();

			boxContext.markOutputAsReadyToSend(0, startTime, endTime);
			boxContext.markOutputAsReadyToSend(1, startTime, endTime);
			boxContext.markOutputAsReadyToSend(2, startTime, endTime);
		}

		if (m_sampleDecoder.isEndReceived())
		{
			m_labelsEncoder.encodeEnd();
			m_hyperplanesEncoder.encodeEnd();
			m_probabilitiesEncoder.encodeEnd();

			boxContext.markOutputAsReadyToSend(0, startTime, endTime);
			boxContext.markOutputAsReadyToSend(1, startTime, endTime);
			boxContext.markOutputAsReadyToSend(2, startTime, endTime);
		}
	}

	return true;
}
