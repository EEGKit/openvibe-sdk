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

	XML::IXMLHandler* l_pHandler = XML::createXMLHandler();
	XML::IXMLNode* l_pRootNode   = l_pHandler->parseFile(sFilename);

	OV_ERROR_UNLESS_KRF(
		l_pRootNode,
		"Unable to get xml root node from file at " << sFilename,
		OpenViBE::Kernel::ErrorType::BadParsing);

	m_vStimulation.clear();

	// Check the version of the file
	OV_ERROR_UNLESS_KRF(
		l_pRootNode->hasAttribute(FORMAT_VERSION_ATTRIBUTE_NAME),
		"Configuration file [" << sFilename << "] has no version information",
		OpenViBE::Kernel::ErrorType::ResourceNotFound);

	string l_sVersion = l_pRootNode->getAttribute(FORMAT_VERSION_ATTRIBUTE_NAME);
	std::stringstream l_sData(l_sVersion);
	uint32_t l_ui32Version;
	l_sData >> l_ui32Version;

	OV_WARNING_UNLESS_K(
		l_ui32Version <= OVP_Classification_BoxTrainerFormatVersion,
		"Classifier configuration in [" << sFilename << "] saved using a newer version: saved version = [" << l_ui32Version
		<< "] vs current version = [" << OVP_Classification_BoxTrainerFormatVersion << "]"
	);

	OV_ERROR_UNLESS_KRF(
		l_ui32Version >= OVP_Classification_BoxTrainerFormatVersionRequired,
		"Classifier configuration in [" << sFilename << "] saved using an obsolete version [" << l_ui32Version
		<< "] (minimum expected version = " << OVP_Classification_BoxTrainerFormatVersionRequired << ")",
		OpenViBE::Kernel::ErrorType::BadVersion);

	CIdentifier l_oAlgorithmClassIdentifier = OV_UndefinedIdentifier;

	XML::IXMLNode* l_pTempNode = l_pRootNode->getChildByName(STRATEGY_NODE_NAME);

	OV_ERROR_UNLESS_KRF(
		l_pTempNode,
		"Configuration file [" << sFilename << "] has no node " << STRATEGY_NODE_NAME,
		OpenViBE::Kernel::ErrorType::BadParsing);

	l_oAlgorithmClassIdentifier.fromString(l_pTempNode->getAttribute(IDENTIFIER_ATTRIBUTE_NAME));

	//If the Identifier is undefined, that means we need to load a native algorithm
	if (l_oAlgorithmClassIdentifier == OV_UndefinedIdentifier)
	{
		l_pTempNode = l_pRootNode->getChildByName(ALGORITHM_NODE_NAME);

		OV_ERROR_UNLESS_KRF(
			l_pTempNode,
			"Configuration file [" << sFilename << "] has no node " << ALGORITHM_NODE_NAME,
			OpenViBE::Kernel::ErrorType::BadParsing);

		l_oAlgorithmClassIdentifier.fromString(l_pTempNode->getAttribute(IDENTIFIER_ATTRIBUTE_NAME));

		//If the algorithm is still unknown, that means that we face an error
		OV_ERROR_UNLESS_KRF(
			l_oAlgorithmClassIdentifier != OV_UndefinedIdentifier,
			"No classifier retrieved from configuration file [" << sFilename << "]",
			OpenViBE::Kernel::ErrorType::BadConfig);
	}

	//Now loading all stimulations output
	XML::IXMLNode* l_pStimulationsNode = l_pRootNode->getChildByName(STIMULATIONS_NODE_NAME);

	OV_ERROR_UNLESS_KRF(
		l_pStimulationsNode,
		"Configuration file [" << sFilename << "] has no node " << STIMULATIONS_NODE_NAME,
		OpenViBE::Kernel::ErrorType::BadParsing);

	//Now load every stimulation and store them in the map with the right class id
	for (uint32_t i = 0; i < l_pStimulationsNode->getChildCount(); i++)
	{
		l_pTempNode = l_pStimulationsNode->getChild(i);

		OV_ERROR_UNLESS_KRF(
			l_pTempNode,
			"Invalid NULL child node " << i << " for node [" << STIMULATIONS_NODE_NAME << "]",
			OpenViBE::Kernel::ErrorType::BadParsing);

		CString l_sStimulationName(l_pTempNode->getPCData());

		double l_f64ClassId;
		const char* l_sAttributeData = l_pTempNode->getAttribute(IDENTIFIER_ATTRIBUTE_NAME);

		OV_ERROR_UNLESS_KRF(
			l_sAttributeData,
			"Invalid child node " << i << " for node [" << STIMULATIONS_NODE_NAME << "]: attribute [" << IDENTIFIER_ATTRIBUTE_NAME << "] not found",
			OpenViBE::Kernel::ErrorType::BadParsing);

		std::stringstream l_sIdentifierData(l_sAttributeData);
		l_sIdentifierData >> l_f64ClassId;
		m_vStimulation[l_f64ClassId] = this->getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_Stimulation, l_sStimulationName);
	}

	const CIdentifier l_oClassifierAlgorithmIdentifier = this->getAlgorithmManager().createAlgorithm(l_oAlgorithmClassIdentifier);

	OV_ERROR_UNLESS_KRF(
		l_oClassifierAlgorithmIdentifier != OV_UndefinedIdentifier,
		"Invalid classifier algorithm with id [" << l_oAlgorithmClassIdentifier.toString() << "] in configuration file [" << sFilename << "]",
		OpenViBE::Kernel::ErrorType::BadConfig);

	m_pClassifier = &this->getAlgorithmManager().getAlgorithm(l_oClassifierAlgorithmIdentifier);
	m_pClassifier->initialize();

	// Connect the params to the new classifier

	TParameterHandler<IMatrix*> ip_oFeatureVector = m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_FeatureVector);
	ip_oFeatureVector.setReferenceTarget(m_oFeatureVectorDecoder.getOutputMatrix());

	m_oHyperplaneValuesEncoder.getInputMatrix().setReferenceTarget(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ClassificationValues));
	m_oProbabilityValuesEncoder.getInputMatrix().setReferenceTarget(m_pClassifier->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_ProbabilityValues));
	// note: labelsencoder cannot be directly bound here as the classifier returns a float, but we need to output a stimulation

	TParameterHandler<XML::IXMLNode*> ip_pClassificationConfiguration(m_pClassifier->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_Configuration));
	ip_pClassificationConfiguration = l_pRootNode->getChildByName(CLASSIFIER_ROOT)->getChild(0);

	OV_ERROR_UNLESS_KRF(
		m_pClassifier->process(OVTK_Algorithm_Classifier_InputTriggerId_LoadConfiguration),
		"Loading configuration failed for subclassifier [" << l_oClassifierAlgorithmIdentifier.toString() << "]",
		OpenViBE::Kernel::ErrorType::Internal);

	l_pRootNode->release();
	l_pHandler->release();

	return true;
}

bool CBoxAlgorithmClassifierProcessor::initialize()
{
	m_pClassifier = nullptr;

	//First of all, let's get the XML file for configuration
	CString l_sConfigurationFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

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

			IStimulationSet* l_pSet = m_oLabelsEncoder.getInputStimulationSet();

			l_pSet->setStimulationCount(1);
			l_pSet->setStimulationIdentifier(0, m_vStimulation[op_f64ClassificationStateClass]);
			l_pSet->setStimulationDate(0, tEnd);
			l_pSet->setStimulationDuration(0, 0);

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
