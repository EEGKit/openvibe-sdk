#include "ovpCBoxAlgorithmClassifierProcessor.h"

#include <sstream>

#include <xml/IXMLHandler.h>
#include <xml/IXMLNode.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;
using namespace std;

bool CBoxAlgorithmClassifierProcessor::loadClassifier(const char* sFilename)
{
	if (m_pClassifier)
	{
		m_pClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pClassifier);
		m_pClassifier = nullptr;
	}

	XML::IXMLHandler* handler = XML::createXMLHandler();
	XML::IXMLNode* rootNode   = handler->parseFile(sFilename);

	OV_ERROR_UNLESS_KRF(rootNode, "Unable to get xml root node from file at " << sFilename, OpenViBE::Kernel::ErrorType::BadParsing);

	m_vStimulation.clear();

	// Check the version of the file
	OV_ERROR_UNLESS_KRF(rootNode->hasAttribute(FORMAT_VERSION_ATTRIBUTE_NAME),
						"Configuration file [" << sFilename << "] has no version information",
						OpenViBE::Kernel::ErrorType::ResourceNotFound);

	std::stringstream data(rootNode->getAttribute(FORMAT_VERSION_ATTRIBUTE_NAME));
	uint32_t version;
	data >> version;

	OV_WARNING_UNLESS_K(version <= OVP_Classification_BoxTrainerFormatVersion,
						"Classifier configuration in [" << sFilename << "] saved using a newer version: saved version = [" << version
						<< "] vs current version = [" << OVP_Classification_BoxTrainerFormatVersion << "]");

	OV_ERROR_UNLESS_KRF(version >= OVP_Classification_BoxTrainerFormatVersionRequired,
						"Classifier configuration in [" << sFilename << "] saved using an obsolete version [" << version
						<< "] (minimum expected version = " << OVP_Classification_BoxTrainerFormatVersionRequired << ")",
						OpenViBE::Kernel::ErrorType::BadVersion);

	CIdentifier algorithmClassID = OV_UndefinedIdentifier;

	XML::IXMLNode* tmp = rootNode->getChildByName(STRATEGY_NODE_NAME);

	OV_ERROR_UNLESS_KRF(
		tmp,
		"Configuration file [" << sFilename << "] has no node " << STRATEGY_NODE_NAME,
		OpenViBE::Kernel::ErrorType::BadParsing);

	algorithmClassID.fromString(tmp->getAttribute(IDENTIFIER_ATTRIBUTE_NAME));

	//If the Identifier is undefined, that means we need to load a native algorithm
	if (algorithmClassID == OV_UndefinedIdentifier)
	{
		tmp = rootNode->getChildByName(ALGORITHM_NODE_NAME);

		OV_ERROR_UNLESS_KRF(tmp,
							"Configuration file [" << sFilename << "] has no node " << ALGORITHM_NODE_NAME,
							OpenViBE::Kernel::ErrorType::BadParsing);

		algorithmClassID.fromString(tmp->getAttribute(IDENTIFIER_ATTRIBUTE_NAME));

		//If the algorithm is still unknown, that means that we face an error
		OV_ERROR_UNLESS_KRF(
			algorithmClassID != OV_UndefinedIdentifier,
			"No classifier retrieved from configuration file [" << sFilename << "]",
			OpenViBE::Kernel::ErrorType::BadConfig);
	}

	//Now loading all stimulations output
	XML::IXMLNode* l_pStimulationsNode = rootNode->getChildByName(STIMULATIONS_NODE_NAME);

	OV_ERROR_UNLESS_KRF(
		l_pStimulationsNode,
		"Configuration file [" << sFilename << "] has no node " << STIMULATIONS_NODE_NAME,
		OpenViBE::Kernel::ErrorType::BadParsing);

	//Now load every stimulation and store them in the map with the right class id
	for (uint32_t i = 0; i < l_pStimulationsNode->getChildCount(); i++)
	{
		tmp = l_pStimulationsNode->getChild(i);

		OV_ERROR_UNLESS_KRF(tmp,
							"Invalid NULL child node " << i << " for node [" << STIMULATIONS_NODE_NAME << "]",
							OpenViBE::Kernel::ErrorType::BadParsing);

		CString l_sStimulationName(tmp->getPCData());

		double classID;
		const char* l_sAttributeData = tmp->getAttribute(IDENTIFIER_ATTRIBUTE_NAME);

		OV_ERROR_UNLESS_KRF(l_sAttributeData,
							"Invalid child node " << i << " for node [" << STIMULATIONS_NODE_NAME << "]: attribute [" << IDENTIFIER_ATTRIBUTE_NAME << "] not found",
							OpenViBE::Kernel::ErrorType::BadParsing);

		std::stringstream identifierData(l_sAttributeData);
		identifierData >> classID;
		m_vStimulation[classID] = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sStimulationName);
	}

	const CIdentifier classifierAlgorithmID = this->getAlgorithmManager().createAlgorithm(algorithmClassID);

	OV_ERROR_UNLESS_KRF(classifierAlgorithmID != OV_UndefinedIdentifier,
						"Invalid classifier algorithm with id [" << algorithmClassID.toString() << "] in configuration file [" << sFilename << "]",
						OpenViBE::Kernel::ErrorType::BadConfig);

	m_pClassifier = &this->getAlgorithmManager().getAlgorithm(classifierAlgorithmID);
	m_pClassifier->initialize();

	// Connect the params to the new classifier

	TParameterHandler<IMatrix*> ip_oFeatureVector = m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector);
	ip_oFeatureVector.setReferenceTarget(m_oFeatureVectorDecoder.getOutputMatrix());

	m_oHyperplaneValuesEncoder.getInputMatrix().setReferenceTarget(
		m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));
	m_oProbabilityValuesEncoder.getInputMatrix().setReferenceTarget(
		m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
	// note: labelsencoder cannot be directly bound here as the classifier returns a float, but we need to output a stimulation

	TParameterHandler<XML::IXMLNode*> ip_pClassificationConfiguration(
		m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
	ip_pClassificationConfiguration = rootNode->getChildByName(CLASSIFIER_ROOT)->getChild(0);

	OV_ERROR_UNLESS_KRF(
		m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration),
		"Loading configuration failed for subclassifier [" << classifierAlgorithmID.toString() << "]",
		OpenViBE::Kernel::ErrorType::Internal);

	rootNode->release();
	handler->release();

	return true;
}

bool CBoxAlgorithmClassifierProcessor::initialize()
{
	m_pClassifier = nullptr;

	//First of all, let's get the XML file for configuration
	const CString l_sConfigurationFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	OV_ERROR_UNLESS_KRF(
		l_sConfigurationFilename != CString(""),
		"Invalid empty configuration file name",
		OpenViBE::Kernel::ErrorType::BadConfig);

	m_oFeatureVectorDecoder.initialize(*this, 0);
	m_oStimulationDecoder.initialize(*this, 1);

	m_oLabelsEncoder.initialize(*this, 0);
	m_oHyperplaneValuesEncoder.initialize(*this, 1);
	m_oProbabilityValuesEncoder.initialize(*this, 2);

	return loadClassifier(l_sConfigurationFilename.toASCIIString());
}

bool CBoxAlgorithmClassifierProcessor::uninitialize()
{
	if (m_pClassifier)
	{
		m_pClassifier->uninitialize();
		this->getAlgorithmManager().releaseAlgorithm(*m_pClassifier);
		m_pClassifier = nullptr;
	}

	m_oProbabilityValuesEncoder.uninitialize();
	m_oHyperplaneValuesEncoder.uninitialize();
	m_oLabelsEncoder.uninitialize();

	m_oStimulationDecoder.uninitialize();
	m_oFeatureVectorDecoder.uninitialize();

	return true;
}

bool CBoxAlgorithmClassifierProcessor::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmClassifierProcessor::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	// Check if we have a command first
	for (uint32_t i = 0; i < boxContext.getInputChunkCount(1); i++)
	{
		m_oStimulationDecoder.decode(i);
		if (m_oStimulationDecoder.isHeaderReceived()) { }
		if (m_oStimulationDecoder.isBufferReceived())
		{
			for (uint64_t j = 0; j < m_oStimulationDecoder.getOutputStimulationSet()->getStimulationCount(); j++)
			{
				if (m_oStimulationDecoder.getOutputStimulationSet()->getStimulationIdentifier(j) == OVTK_StimulationId_TrainCompleted)
				{
					CString l_sConfigurationFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

					if (!loadClassifier(l_sConfigurationFilename.toASCIIString())) { return false; }
				}
			}
		}
		if (m_oStimulationDecoder.isEndReceived()) { }
	}

	// Classify data
	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		const uint64_t tStart = boxContext.getInputChunkStartTime(0, i);
		const uint64_t tEnd   = boxContext.getInputChunkEndTime(0, i);

		m_oFeatureVectorDecoder.decode(i);
		if (m_oFeatureVectorDecoder.isHeaderReceived())
		{
			m_oLabelsEncoder.encodeHeader();
			m_oHyperplaneValuesEncoder.encodeHeader();
			m_oProbabilityValuesEncoder.encodeHeader();

			boxContext.markOutputAsReadyToSend(0, tStart, tEnd);
			boxContext.markOutputAsReadyToSend(1, tStart, tEnd);
			boxContext.markOutputAsReadyToSend(2, tStart, tEnd);
		}
		if (m_oFeatureVectorDecoder.isBufferReceived())
		{
			OV_ERROR_UNLESS_KRF(
				m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_Classify) &&
				m_pClassifier->isOutputTriggerActive(OVTK_Algorithm_Classifier_OutputTriggerId_Success),
				"Classification failed",
				OpenViBE::Kernel::ErrorType::Internal);

			TParameterHandler<double> op_f64ClassificationStateClass(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Class));

			IStimulationSet* set = m_oLabelsEncoder.getInputStimulationSet();

			set->setStimulationCount(1);
			set->setStimulationIdentifier(0, m_vStimulation[op_f64ClassificationStateClass]);
			set->setStimulationDate(0, tEnd);
			set->setStimulationDuration(0, 0);

			m_oLabelsEncoder.encodeBuffer();
			m_oHyperplaneValuesEncoder.encodeBuffer();
			m_oProbabilityValuesEncoder.encodeBuffer();

			boxContext.markOutputAsReadyToSend(0, tStart, tEnd);
			boxContext.markOutputAsReadyToSend(1, tStart, tEnd);
			boxContext.markOutputAsReadyToSend(2, tStart, tEnd);
		}

		if (m_oFeatureVectorDecoder.isEndReceived())
		{
			m_oLabelsEncoder.encodeEnd();
			m_oHyperplaneValuesEncoder.encodeEnd();
			m_oProbabilityValuesEncoder.encodeEnd();

			boxContext.markOutputAsReadyToSend(0, tStart, tEnd);
			boxContext.markOutputAsReadyToSend(1, tStart, tEnd);
			boxContext.markOutputAsReadyToSend(2, tStart, tEnd);
		}
	}

	return true;
}
