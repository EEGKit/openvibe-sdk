#include "ovpCAlgorithmXMLScenarioImporter.h"

#include <iostream>
#include <cstdlib>
#include <string>
#include <memory>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/validators/common/Grammar.hpp>

XERCES_CPP_NAMESPACE_USE

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace FileIO;
using namespace OpenViBEToolkit;

enum
{
	Status_ParsingNothing,

	Status_ParsingScenario,
	Status_ParsingScenarioAttribute,
	Status_ParsingScenarioInput,
	Status_ParsingScenarioOutput,
	Status_ParsingScenarioSetting,

	Status_ParsingBox,
	Status_ParsingBoxInput,
	Status_ParsingBoxOutput,
	Status_ParsingBoxSetting,
	Status_ParsingBoxAttribute,

	Status_ParsingComment,
	Status_ParsingCommentAttribute,

	Status_ParsingMetadataEntry,

	Status_ParsingLink,
	Status_ParsingLinkSource,
	Status_ParsingLinkTarget,
	Status_ParsingLinkAttribute,
};

namespace
{
	class _AutoBind_
	{
	public:
		explicit _AutoBind_(const std::string& sValue) : m_sValue(sValue) { }
		operator CString(void) { return CString(m_sValue.c_str()); }

		operator CIdentifier(void)
		{
			CIdentifier l_oResult;
			l_oResult.fromString(m_sValue.c_str());
			return l_oResult;
		}

		operator uint32_t(void) { return atoi(m_sValue.c_str()); }
	protected:
		const std::string& m_sValue;
	};

	std::string xercesToString(const XMLCh* xercesString)
	{
		std::unique_ptr<char[]> charArray(XMLString::transcode(xercesString));
		return std::string(charArray.get());
	}

	class CErrorHandler final : public HandlerBase
	{
	public:

		explicit CErrorHandler(IAlgorithmContext& rAlgorithmContext)
			: m_rAlgorithmContext(rAlgorithmContext) { }

		void fatalError(const SAXParseException& exception) override
		{
			this->error(exception);
		}

		void error(const SAXParseException& exception) override
		{
			// we just issue a trace here because the calling method
			// implements a fallback mechanism and we don't want to populate
			// the error manager if the importer returns gracefully.
			m_rAlgorithmContext.getLogManager() << LogLevel_Trace
					<< "Failed to validate xml: error ["
					<< xercesToString(exception.getMessage()).c_str()
					<< "], line number [" << static_cast<uint64_t>(exception.getLineNumber()) << "]"
					<< "\n";
		}

		void warning(const SAXParseException& exception) override
		{
			OV_WARNING(
				"Warning while validating xml: warning [" << xercesToString(exception.getMessage()).c_str() << "], line number [" << static_cast<uint64_t>(exception.getLineNumber()) << "]",
				m_rAlgorithmContext.getLogManager()
			);
		}

	private:
		IAlgorithmContext& m_rAlgorithmContext;
	};
};

CAlgorithmXMLScenarioImporter::CAlgorithmXMLScenarioImporter(void)
	: m_pContext(NULL)
	  , m_ui32Status(Status_ParsingNothing)
	  , m_pReader(NULL)
{
	m_pReader = createReader(*this);
}

CAlgorithmXMLScenarioImporter::~CAlgorithmXMLScenarioImporter(void)
{
	m_pReader->release();
}

void CAlgorithmXMLScenarioImporter::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount)
{
	m_vNodes.push(sName);

	std::string& l_sTop = m_vNodes.top();

	if (false) { }

	else if (l_sTop == "OpenViBE-Scenario" && m_ui32Status == Status_ParsingNothing)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario);
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingScenarioAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute);
	}
	else if (l_sTop == "Setting" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingScenarioSetting;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting);
	}
	else if (l_sTop == "Input" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingScenarioInput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input);
	}
	else if (l_sTop == "Output" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingScenarioOutput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output);
	}

	else if (l_sTop == "Box" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingBox;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box);
	}
	else if (l_sTop == "Input" && m_ui32Status == Status_ParsingBox)
	{
		m_ui32Status = Status_ParsingBoxInput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input);
	}
	else if (l_sTop == "Output" && m_ui32Status == Status_ParsingBox)
	{
		m_ui32Status = Status_ParsingBoxOutput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output);
	}
	else if (l_sTop == "Setting" && m_ui32Status == Status_ParsingBox)
	{
		m_ui32Status = Status_ParsingBoxSetting;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting);
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingBox)
	{
		m_ui32Status = Status_ParsingBoxAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute);
	}

	else if (l_sTop == "Comment" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingComment;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Comment);
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingComment)
	{
		m_ui32Status = Status_ParsingCommentAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute);
	}

	else if (l_sTop == "Entry" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingMetadataEntry;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry);
	}

	else if (l_sTop == "Link" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingLink;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link);
	}
	else if (l_sTop == "Source" && m_ui32Status == Status_ParsingLink)
	{
		m_ui32Status = Status_ParsingLinkSource;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source);
	}
	else if (l_sTop == "Target" && m_ui32Status == Status_ParsingLink)
	{
		m_ui32Status = Status_ParsingLinkTarget;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target);
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingLink)
	{
		m_ui32Status = Status_ParsingLinkAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute);
	}
}

void CAlgorithmXMLScenarioImporter::processChildData(const char* sData)
{
	std::string& l_sTop = m_vNodes.top();

	switch (m_ui32Status)
	{
		case Status_ParsingBox:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier, _AutoBind_(sData));
			if (l_sTop == "AlgorithmClassIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier, _AutoBind_(sData));
			if (l_sTop == "Name") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name, _AutoBind_(sData));
			break;
		case Status_ParsingBoxInput:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Identifier, _AutoBind_(sData));
			if (l_sTop == "TypeIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier, _AutoBind_(sData));
			if (l_sTop == "Name") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name, _AutoBind_(sData));
			break;
		case Status_ParsingBoxOutput:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Identifier, _AutoBind_(sData));
			if (l_sTop == "TypeIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier, _AutoBind_(sData));
			if (l_sTop == "Name") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name, _AutoBind_(sData));
			break;
		case Status_ParsingBoxSetting:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Identifier, _AutoBind_(sData));
			if (l_sTop == "TypeIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier, _AutoBind_(sData));
			if (l_sTop == "Name") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name, _AutoBind_(sData));
			if (l_sTop == "DefaultValue") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue, _AutoBind_(sData));
			if (l_sTop == "Value") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value, _AutoBind_(sData));
			if (l_sTop == "Modifiability") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability, _AutoBind_(sData));
			break;
		case Status_ParsingBoxAttribute:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier, _AutoBind_(sData));
			if (l_sTop == "Value") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value, _AutoBind_(sData));
			break;

		case Status_ParsingComment:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier, _AutoBind_(sData));
			if (l_sTop == "Text") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text, _AutoBind_(sData));
			break;

		case Status_ParsingMetadataEntry:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Identifier, _AutoBind_(sData));
			if (l_sTop == "Type") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type, _AutoBind_(sData));
			if (l_sTop == "Data") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data, _AutoBind_(sData));
			break;

		case Status_ParsingCommentAttribute:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier, _AutoBind_(sData));
			if (l_sTop == "Value") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value, _AutoBind_(sData));
			break;

		case Status_ParsingLink:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier, _AutoBind_(sData));
			break;
		case Status_ParsingLinkSource:
			if (l_sTop == "BoxIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier, _AutoBind_(sData));
			if (l_sTop == "BoxOutputIndex") m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex, _AutoBind_(sData));
			if (l_sTop == "BoxOutputIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdentifier, _AutoBind_(sData));
			break;
		case Status_ParsingLinkTarget:
			if (l_sTop == "BoxIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier, _AutoBind_(sData));
			if (l_sTop == "BoxInputIndex") m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex, _AutoBind_(sData));
			if (l_sTop == "BoxInputIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdentifier, _AutoBind_(sData));
			break;
		case Status_ParsingLinkAttribute:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier, _AutoBind_(sData));
			if (l_sTop == "Value") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioSetting:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Identifier, _AutoBind_(sData));
			if (l_sTop == "TypeIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier, _AutoBind_(sData));
			if (l_sTop == "Name") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name, _AutoBind_(sData));
			if (l_sTop == "DefaultValue") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue, _AutoBind_(sData));
			if (l_sTop == "Value") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioInput:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Identifier, _AutoBind_(sData));
			if (l_sTop == "TypeIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier, _AutoBind_(sData));
			if (l_sTop == "Name") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name, _AutoBind_(sData));
			if (l_sTop == "LinkedBoxIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier, _AutoBind_(sData));
			if (l_sTop == "LinkedBoxInputIndex") m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex, _AutoBind_(sData));
			if (l_sTop == "LinkedBoxInputIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdentifier, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioOutput:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Identifier, _AutoBind_(sData));
			if (l_sTop == "TypeIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier, _AutoBind_(sData));
			if (l_sTop == "Name") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name, _AutoBind_(sData));
			if (l_sTop == "LinkedBoxIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier, _AutoBind_(sData));
			if (l_sTop == "LinkedBoxOutputIndex") m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex, _AutoBind_(sData));
			if (l_sTop == "LinkedBoxOutputIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdentifier, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioAttribute:
			if (l_sTop == "Identifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier, _AutoBind_(sData));
			if (l_sTop == "Value") m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value, _AutoBind_(sData));
			break;
	}
}

void CAlgorithmXMLScenarioImporter::closeChild(void)
{
	std::string& l_sTop = m_vNodes.top();

	if (false) { }

	else if (l_sTop == "OpenViBE-Scenario" && m_ui32Status == Status_ParsingScenario)
	{
		m_ui32Status = Status_ParsingNothing;
		m_pContext->processStop();
	}
	else if (l_sTop == "Setting" && m_ui32Status == Status_ParsingScenarioSetting)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (l_sTop == "Input" && m_ui32Status == Status_ParsingScenarioInput)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (l_sTop == "Output" && m_ui32Status == Status_ParsingScenarioOutput)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingScenarioAttribute)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}

	else if (l_sTop == "Box" && m_ui32Status == Status_ParsingBox)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (l_sTop == "Input" && m_ui32Status == Status_ParsingBoxInput)
	{
		m_ui32Status = Status_ParsingBox;
		m_pContext->processStop();
	}

	else if (l_sTop == "Output" && m_ui32Status == Status_ParsingBoxOutput)
	{
		m_ui32Status = Status_ParsingBox;
		m_pContext->processStop();
	}
	else if (l_sTop == "Setting" && m_ui32Status == Status_ParsingBoxSetting)
	{
		m_ui32Status = Status_ParsingBox;
		m_pContext->processStop();
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingBoxAttribute)
	{
		m_ui32Status = Status_ParsingBox;
		m_pContext->processStop();
	}

	else if (l_sTop == "Comment" && m_ui32Status == Status_ParsingComment)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingCommentAttribute)
	{
		m_ui32Status = Status_ParsingComment;
		m_pContext->processStop();
	}

	else if (l_sTop == "Entry" && m_ui32Status == Status_ParsingMetadataEntry)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}

	else if (l_sTop == "Link" && m_ui32Status == Status_ParsingLink)
	{
		m_ui32Status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (l_sTop == "Source" && m_ui32Status == Status_ParsingLinkSource)
	{
		m_ui32Status = Status_ParsingLink;
		m_pContext->processStop();
	}
	else if (l_sTop == "Target" && m_ui32Status == Status_ParsingLinkTarget)
	{
		m_ui32Status = Status_ParsingLink;
		m_pContext->processStop();
	}
	else if (l_sTop == "Attribute" && m_ui32Status == Status_ParsingLinkAttribute)
	{
		m_ui32Status = Status_ParsingLink;
		m_pContext->processStop();
	}

	m_vNodes.pop();
}

bool CAlgorithmXMLScenarioImporter::validateXML(const unsigned char* xmlBuffer, unsigned long xmlBufferSize)
{
	// implementation of the fallback mechanism

	// error manager is used to differentiate errors from invalid xml
	this->getErrorManager().releaseErrors();

	if (this->validateXMLAgainstSchema((Directories::getDataDir() + "/kernel/openvibe-scenario-v2.xsd"), xmlBuffer, xmlBufferSize)) { return true; }
	else if (this->getErrorManager().hasError())
	{
		// this is not a validation error thus we return directly
		return false;
	}

	if (this->validateXMLAgainstSchema((Directories::getDataDir() + "/kernel/openvibe-scenario-v1.xsd"), xmlBuffer, xmlBufferSize))
	{
		this->getLogManager() << LogLevel_Trace << "Importing scenario with legacy format: v1 scenario might be deprecated in the future so upgrade to v2 format when possible\n";
		return true;
	}
	else if (this->getErrorManager().hasError())
	{
		// this is not a validation error thus we return directly
		return false;
	}

	if (this->validateXMLAgainstSchema((Directories::getDataDir() + "/kernel/openvibe-scenario-legacy.xsd"), xmlBuffer, xmlBufferSize))
	{
		OV_WARNING_K("Importing scenario with legacy format: legacy scenario might be deprecated in the future so upgrade to v2 format when possible");
		return true;
	}
	else if (this->getErrorManager().hasError())
	{
		// this is not a validation error thus we return directly
		return false;
	}

	OV_ERROR_KRF(
		"Failed to validate scenario against XSD schemas",
		OpenViBE::Kernel::ErrorType::BadXMLSchemaValidation
	);
}

bool CAlgorithmXMLScenarioImporter::validateXMLAgainstSchema(const char* validationSchema, const unsigned char* xmlBuffer, unsigned long xmlBufferSize)
{
	this->getLogManager() << LogLevel_Trace << "Validating XML against schema [" << validationSchema << "]\n";

	size_t errorCount = 0;

	XMLPlatformUtils::Initialize();

	{ // scope the content here to ensure unique_ptr contents are destroyed before the call to XMLPlatformUtils::Terminate();
		std::unique_ptr<MemBufInputSource> xercesBuffer(new MemBufInputSource(xmlBuffer, xmlBufferSize, "xml memory buffer"));

		std::unique_ptr<XercesDOMParser> parser(new XercesDOMParser());
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(true);
		parser->setDoSchema(true);
		parser->setValidationConstraintFatal(true);
		parser->setValidationSchemaFullChecking(true);
		parser->setExternalNoNamespaceSchemaLocation(validationSchema);

		std::unique_ptr<ErrorHandler> errorHandler(new CErrorHandler(this->getAlgorithmContext()));
		parser->setErrorHandler(errorHandler.get());

		parser->parse(*xercesBuffer);
		errorCount = parser->getErrorCount();
	}

	XMLPlatformUtils::Terminate();

	return (errorCount == 0);
}

bool CAlgorithmXMLScenarioImporter::import(IAlgorithmScenarioImporterContext& rContext, const IMemoryBuffer& rMemoryBuffer)
{
	m_pContext = &rContext;

	if (!this->validateXML(rMemoryBuffer.getDirectPointer(), static_cast<unsigned long>(rMemoryBuffer.getSize())))
	{
		return false; // error handling is handled in validateXML
	}

	return m_pReader->processData(rMemoryBuffer.getDirectPointer(), rMemoryBuffer.getSize());
}
