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
		explicit _AutoBind_(const std::string& value) : m_value(value) { }
		operator CString() { return CString(m_value.c_str()); }

		operator CIdentifier()
		{
			CIdentifier res;
			res.fromString(m_value.c_str());
			return res;
		}

		operator uint32_t() { return atoi(m_value.c_str()); }
	protected:
		const std::string& m_value;
	};

	std::string xercesToString(const XMLCh* xercesString)
	{
		const std::unique_ptr<char[]> charArray(XMLString::transcode(xercesString));
		return std::string(charArray.get());
	}

	class CErrorHandler final : public HandlerBase
	{
	public:

		explicit CErrorHandler(IAlgorithmContext& algorithmCtx)
			: m_algorithmContext(algorithmCtx) { }

		void fatalError(const SAXParseException& exception) override { this->error(exception); }

		void error(const SAXParseException& exception) override
		{
			// we just issue a trace here because the calling method
			// implements a fallback mechanism and we don't want to populate
			// the error manager if the importer returns gracefully.
			m_algorithmContext.getLogManager() << LogLevel_Trace
					<< "Failed to validate xml: error ["
					<< xercesToString(exception.getMessage()).c_str()
					<< "], line number [" << uint64_t(exception.getLineNumber()) << "]"
					<< "\n";
		}

		void warning(const SAXParseException& exception) override
		{
			OV_WARNING(
				"Warning while validating xml: warning [" << xercesToString(exception.getMessage()).c_str() << "], line number [" << uint64_t(exception.
					getLineNumber()) << "]",
				m_algorithmContext.getLogManager());
		}

	private:
		IAlgorithmContext& m_algorithmContext;
	};
}

CAlgorithmXMLScenarioImporter::CAlgorithmXMLScenarioImporter() { m_pReader = createReader(*this); }

CAlgorithmXMLScenarioImporter::~CAlgorithmXMLScenarioImporter() { m_pReader->release(); }

void CAlgorithmXMLScenarioImporter::openChild(const char* name, const char** /*attributeName*/, const char** /*attributeValue*/, uint64_t /*nAttribute*/)
{
	m_nodes.push(name);

	std::string& top = m_nodes.top();

	if (top == "OpenViBE-Scenario" && m_status == Status_ParsingNothing)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario);
	}
	else if (top == "Attribute" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingScenarioAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute);
	}
	else if (top == "Setting" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingScenarioSetting;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting);
	}
	else if (top == "Input" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingScenarioInput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input);
	}
	else if (top == "Output" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingScenarioOutput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output);
	}

	else if (top == "Box" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingBox;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box);
	}
	else if (top == "Input" && m_status == Status_ParsingBox)
	{
		m_status = Status_ParsingBoxInput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input);
	}
	else if (top == "Output" && m_status == Status_ParsingBox)
	{
		m_status = Status_ParsingBoxOutput;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output);
	}
	else if (top == "Setting" && m_status == Status_ParsingBox)
	{
		m_status = Status_ParsingBoxSetting;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting);
	}
	else if (top == "Attribute" && m_status == Status_ParsingBox)
	{
		m_status = Status_ParsingBoxAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute);
	}

	else if (top == "Comment" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingComment;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Comment);
	}
	else if (top == "Attribute" && m_status == Status_ParsingComment)
	{
		m_status = Status_ParsingCommentAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute);
	}

	else if (top == "Entry" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingMetadataEntry;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry);
	}

	else if (top == "Link" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingLink;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link);
	}
	else if (top == "Source" && m_status == Status_ParsingLink)
	{
		m_status = Status_ParsingLinkSource;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source);
	}
	else if (top == "Target" && m_status == Status_ParsingLink)
	{
		m_status = Status_ParsingLinkTarget;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target);
	}
	else if (top == "Attribute" && m_status == Status_ParsingLink)
	{
		m_status = Status_ParsingLinkAttribute;
		m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute);
	}
}

void CAlgorithmXMLScenarioImporter::processChildData(const char* sData)
{
	std::string& top = m_nodes.top();

	switch (m_status)
	{
		case Status_ParsingBox:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier, _AutoBind_(sData)); }
			if (top == "AlgorithmClassIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier, _AutoBind_(sData)); }
			if (top == "Name") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name, _AutoBind_(sData)); }
			break;
		case Status_ParsingBoxInput:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Identifier, _AutoBind_(sData)); }
			if (top == "TypeIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier, _AutoBind_(sData)); }
			if (top == "Name") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name, _AutoBind_(sData)); }
			break;
		case Status_ParsingBoxOutput:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Identifier, _AutoBind_(sData)); }
			if (top == "TypeIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier, _AutoBind_(sData)); }
			if (top == "Name") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name, _AutoBind_(sData)); }
			break;
		case Status_ParsingBoxSetting:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Identifier, _AutoBind_(sData)); }
			if (top == "TypeIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier, _AutoBind_(sData)); }
			if (top == "Name") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name, _AutoBind_(sData)); }
			if (top == "DefaultValue") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue, _AutoBind_(sData)); }
			if (top == "Value") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value, _AutoBind_(sData)); }
			if (top == "Modifiability") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability, _AutoBind_(sData)); }
			break;
		case Status_ParsingBoxAttribute:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier, _AutoBind_(sData)); }
			if (top == "Value") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value, _AutoBind_(sData)); }
			break;

		case Status_ParsingComment:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier, _AutoBind_(sData)); }
			if (top == "Text") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text, _AutoBind_(sData)); }
			break;

		case Status_ParsingMetadataEntry:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Identifier, _AutoBind_(sData)); }
			if (top == "Type") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type, _AutoBind_(sData)); }
			if (top == "Data") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data, _AutoBind_(sData)); }
			break;

		case Status_ParsingCommentAttribute:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier, _AutoBind_(sData)); }
			if (top == "Value") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value, _AutoBind_(sData)); }
			break;

		case Status_ParsingLink:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier, _AutoBind_(sData)); }
			break;
		case Status_ParsingLinkSource:
			if (top == "BoxIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier, _AutoBind_(sData)); }
			if (top == "BoxOutputIndex") { m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex, _AutoBind_(sData)); }
			if (top == "BoxOutputIdentifier") m_pContext->processIdentifier(
				OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdentifier, _AutoBind_(sData));
			break;
		case Status_ParsingLinkTarget:
			if (top == "BoxIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier, _AutoBind_(sData)); }
			if (top == "BoxInputIndex") { m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex, _AutoBind_(sData)); }
			if (top == "BoxInputIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdentifier, _AutoBind_(sData)); }
			break;
		case Status_ParsingLinkAttribute:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier, _AutoBind_(sData)); }
			if (top == "Value") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value, _AutoBind_(sData)); }
			break;

		case Status_ParsingScenarioSetting:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Identifier, _AutoBind_(sData)); }
			if (top == "TypeIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier, _AutoBind_(sData)); }
			if (top == "Name") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name, _AutoBind_(sData)); }
			if (top == "DefaultValue") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue, _AutoBind_(sData)); }
			if (top == "Value") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value, _AutoBind_(sData)); }
			break;

		case Status_ParsingScenarioInput:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Identifier, _AutoBind_(sData)); }
			if (top == "TypeIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier, _AutoBind_(sData)); }
			if (top == "Name") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name, _AutoBind_(sData)); }
			if (top == "LinkedBoxIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier, _AutoBind_(sData)); }
			if (top == "LinkedBoxInputIndex") { m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex, _AutoBind_(sData)); }
			if (top == "LinkedBoxInputIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdentifier, _AutoBind_(sData)); }
			break;

		case Status_ParsingScenarioOutput:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Identifier, _AutoBind_(sData)); }
			if (top == "TypeIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier, _AutoBind_(sData)); }
			if (top == "Name") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name, _AutoBind_(sData)); }
			if (top == "LinkedBoxIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier, _AutoBind_(sData)); }
			if (top == "LinkedBoxOutputIndex") { m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex, _AutoBind_(sData)); }
			if (top == "LinkedBoxOutputIdentifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdentifier, _AutoBind_(sData)); }
			break;

		case Status_ParsingScenarioAttribute:
			if (top == "Identifier") { m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier, _AutoBind_(sData)); }
			if (top == "Value") { m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value, _AutoBind_(sData)); }
			break;
		default: break;
	}
}

void CAlgorithmXMLScenarioImporter::closeChild()
{
	std::string& top = m_nodes.top();

	if (false) { }
	if (top == "OpenViBE-Scenario" && m_status == Status_ParsingScenario)
	{
		m_status = Status_ParsingNothing;
		m_pContext->processStop();
	}
	else if (top == "Setting" && m_status == Status_ParsingScenarioSetting)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (top == "Input" && m_status == Status_ParsingScenarioInput)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (top == "Output" && m_status == Status_ParsingScenarioOutput)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (top == "Attribute" && m_status == Status_ParsingScenarioAttribute)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}

	else if (top == "Box" && m_status == Status_ParsingBox)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (top == "Input" && m_status == Status_ParsingBoxInput)
	{
		m_status = Status_ParsingBox;
		m_pContext->processStop();
	}

	else if (top == "Output" && m_status == Status_ParsingBoxOutput)
	{
		m_status = Status_ParsingBox;
		m_pContext->processStop();
	}
	else if (top == "Setting" && m_status == Status_ParsingBoxSetting)
	{
		m_status = Status_ParsingBox;
		m_pContext->processStop();
	}
	else if (top == "Attribute" && m_status == Status_ParsingBoxAttribute)
	{
		m_status = Status_ParsingBox;
		m_pContext->processStop();
	}

	else if (top == "Comment" && m_status == Status_ParsingComment)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (top == "Attribute" && m_status == Status_ParsingCommentAttribute)
	{
		m_status = Status_ParsingComment;
		m_pContext->processStop();
	}

	else if (top == "Entry" && m_status == Status_ParsingMetadataEntry)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}

	else if (top == "Link" && m_status == Status_ParsingLink)
	{
		m_status = Status_ParsingScenario;
		m_pContext->processStop();
	}
	else if (top == "Source" && m_status == Status_ParsingLinkSource)
	{
		m_status = Status_ParsingLink;
		m_pContext->processStop();
	}
	else if (top == "Target" && m_status == Status_ParsingLinkTarget)
	{
		m_status = Status_ParsingLink;
		m_pContext->processStop();
	}
	else if (top == "Attribute" && m_status == Status_ParsingLinkAttribute)
	{
		m_status = Status_ParsingLink;
		m_pContext->processStop();
	}

	m_nodes.pop();
}

bool CAlgorithmXMLScenarioImporter::validateXML(const unsigned char* xmlBuffer, const unsigned long xmlBufferSize)
{
	// implementation of the fallback mechanism

	// error manager is used to differentiate errors from invalid xml
	this->getErrorManager().releaseErrors();

	if (this->validateXMLAgainstSchema((Directories::getDataDir() + "/kernel/openvibe-scenario-v2.xsd"), xmlBuffer, xmlBufferSize)) { return true; }
	if (this->getErrorManager().hasError())
	{
		// this is not a validation error thus we return directly
		return false;
	}

	if (this->validateXMLAgainstSchema((Directories::getDataDir() + "/kernel/openvibe-scenario-v1.xsd"), xmlBuffer, xmlBufferSize))
	{
		this->getLogManager() << LogLevel_Trace <<
				"Importing scenario with legacy format: v1 scenario might be deprecated in the future so upgrade to v2 format when possible\n";
		return true;
	}
	if (this->getErrorManager().hasError())
	{
		// this is not a validation error thus we return directly
		return false;
	}

	if (this->validateXMLAgainstSchema((Directories::getDataDir() + "/kernel/openvibe-scenario-legacy.xsd"), xmlBuffer, xmlBufferSize))
	{
		OV_WARNING_K("Importing scenario with legacy format: legacy scenario might be deprecated in the future so upgrade to v2 format when possible");
		return true;
	}
	if (this->getErrorManager().hasError())
	{
		// this is not a validation error thus we return directly
		return false;
	}

	OV_ERROR_KRF("Failed to validate scenario against XSD schemas", OpenViBE::Kernel::ErrorType::BadXMLSchemaValidation);
}

bool CAlgorithmXMLScenarioImporter::validateXMLAgainstSchema(const char* validationSchema, const unsigned char* xmlBuffer, const unsigned long xmlBufferSize)
{
	this->getLogManager() << LogLevel_Trace << "Validating XML against schema [" << validationSchema << "]\n";

	size_t errorCount;
	XMLPlatformUtils::Initialize();

	{ // scope the content here to ensure unique_ptr contents are destroyed before the call to XMLPlatformUtils::Terminate();
		const std::unique_ptr<MemBufInputSource> xercesBuffer(new MemBufInputSource(xmlBuffer, xmlBufferSize, "xml memory buffer"));

		std::unique_ptr<XercesDOMParser> parser(new XercesDOMParser());
		parser->setValidationScheme(XercesDOMParser::Val_Always);
		parser->setDoNamespaces(true);
		parser->setDoSchema(true);
		parser->setValidationConstraintFatal(true);
		parser->setValidationSchemaFullChecking(true);
		parser->setExternalNoNamespaceSchemaLocation(validationSchema);

		const std::unique_ptr<ErrorHandler> errorHandler(new CErrorHandler(this->getAlgorithmContext()));
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
