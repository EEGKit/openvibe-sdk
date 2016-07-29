#include "ovpCAlgorithmXMLScenarioImporter.h"

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;
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
		operator CString (void) { return CString(m_sValue.c_str()); }
		operator CIdentifier (void) { CIdentifier l_oResult; l_oResult.fromString(m_sValue.c_str()); return l_oResult; }
		operator uint32 (void) { return atoi(m_sValue.c_str()); }
	protected:
		const std::string& m_sValue;
	};
};

CAlgorithmXMLScenarioImporter::CAlgorithmXMLScenarioImporter(void)
	:m_pContext(NULL)
	,m_ui32Status(Status_ParsingNothing)
	,m_pReader(NULL)
	,m_bScenarioRecognized(false)
{
	m_pReader=XML::createReader(*this);
}

CAlgorithmXMLScenarioImporter::~CAlgorithmXMLScenarioImporter(void)
{
	m_pReader->release();
}

void CAlgorithmXMLScenarioImporter::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, XML::uint64 ui64AttributeCount)
{
	m_vNodes.push(sName);

	std::string& l_sTop=m_vNodes.top();

	if(false) { }

	else if(l_sTop=="OpenViBE-Scenario"   && m_ui32Status==Status_ParsingNothing)  { m_ui32Status=Status_ParsingScenario;          m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario); m_bScenarioRecognized = true; }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingScenarioAttribute; m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute); }
	else if(l_sTop=="Setting"             && m_ui32Status==Status_ParsingScenario)      { m_ui32Status=Status_ParsingScenarioSetting;        m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting); }
	else if(l_sTop=="Input"               && m_ui32Status==Status_ParsingScenario)      { m_ui32Status=Status_ParsingScenarioInput;        m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input); }
	else if(l_sTop=="Output"               && m_ui32Status==Status_ParsingScenario)      { m_ui32Status=Status_ParsingScenarioOutput;        m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output); }

	else if(l_sTop=="Box"                 && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingBox;               m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box); }
	else if(l_sTop=="Input"               && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxInput;          m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input); }
	else if(l_sTop=="Output"              && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxOutput;         m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output); }
	else if(l_sTop=="Setting"             && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxSetting;        m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingBox)      { m_ui32Status=Status_ParsingBoxAttribute;      m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute); }

	else if(l_sTop=="Comment"             && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingComment;           m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Comment); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingComment)  { m_ui32Status=Status_ParsingCommentAttribute;  m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute); }

	else if(l_sTop=="Link"                && m_ui32Status==Status_ParsingScenario) { m_ui32Status=Status_ParsingLink;              m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link); }
	else if(l_sTop=="Source"              && m_ui32Status==Status_ParsingLink)     { m_ui32Status=Status_ParsingLinkSource;        m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source); }
	else if(l_sTop=="Target"              && m_ui32Status==Status_ParsingLink)     { m_ui32Status=Status_ParsingLinkTarget;        m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingLink)     { m_ui32Status=Status_ParsingLinkAttribute;     m_pContext->processStart(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute); }
}

void CAlgorithmXMLScenarioImporter::processChildData(const char* sData)
{
	std::string& l_sTop=m_vNodes.top();

	switch(m_ui32Status)
	{
		case Status_ParsingBox:
			if(l_sTop=="Identifier")               m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier, _AutoBind_(sData));
			if(l_sTop=="AlgorithmClassIdentifier") m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier, _AutoBind_(sData));
			if(l_sTop=="Name")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name, _AutoBind_(sData));
			break;
		case Status_ParsingBoxInput:
			if(l_sTop=="TypeIdentifier")           m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier, _AutoBind_(sData));
			if(l_sTop=="Name")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name, _AutoBind_(sData));
			break;
		case Status_ParsingBoxOutput:
			if(l_sTop=="TypeIdentifier")           m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier, _AutoBind_(sData));
			if(l_sTop=="Name")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name, _AutoBind_(sData));
			break;
		case Status_ParsingBoxSetting:
			if(l_sTop=="TypeIdentifier")           m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier, _AutoBind_(sData));
			if(l_sTop=="Name")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name, _AutoBind_(sData));
			if(l_sTop=="DefaultValue")             m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue, _AutoBind_(sData));
			if(l_sTop=="Value")                    m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value, _AutoBind_(sData));
			if(l_sTop=="Modifiability")			   m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability, _AutoBind_(sData));
			break;
		case Status_ParsingBoxAttribute:
			if(l_sTop=="Identifier")               m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier, _AutoBind_(sData));
			if(l_sTop=="Value")                    m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value, _AutoBind_(sData));
			break;

		case Status_ParsingComment:
			if(l_sTop=="Identifier")               m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier, _AutoBind_(sData));
			if(l_sTop=="Text")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text, _AutoBind_(sData));
			break;
		case Status_ParsingCommentAttribute:
			if(l_sTop=="Identifier")               m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier, _AutoBind_(sData));
			if(l_sTop=="Value")                    m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value, _AutoBind_(sData));
			break;

		case Status_ParsingLink:
			if(l_sTop=="Identifier")               m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier, _AutoBind_(sData));
			break;
		case Status_ParsingLinkSource:
			if(l_sTop=="BoxIdentifier")            m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier, _AutoBind_(sData));
			if(l_sTop=="BoxOutputIndex")           m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex, _AutoBind_(sData));
			break;
		case Status_ParsingLinkTarget:
			if(l_sTop=="BoxIdentifier")            m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier, _AutoBind_(sData));
			if(l_sTop=="BoxInputIndex")            m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex, _AutoBind_(sData));
			break;
		case Status_ParsingLinkAttribute:
			if(l_sTop=="Identifier")               m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier, _AutoBind_(sData));
			if(l_sTop=="Value")                    m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioSetting:
			if(l_sTop=="TypeIdentifier")           m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier, _AutoBind_(sData));
			if(l_sTop=="Name")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name, _AutoBind_(sData));
			if(l_sTop=="DefaultValue")             m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue, _AutoBind_(sData));
			if(l_sTop=="Value")                    m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioInput:
			if(l_sTop=="TypeIdentifier")           m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier, _AutoBind_(sData));
			if(l_sTop=="Name")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name, _AutoBind_(sData));
			if(l_sTop=="LinkedBoxIdentifier")      m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier, _AutoBind_(sData));
			if(l_sTop=="LinkedBoxInputIndex")      m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioOutput:
			if(l_sTop=="TypeIdentifier")           m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier, _AutoBind_(sData));
			if(l_sTop=="Name")                     m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name, _AutoBind_(sData));
			if(l_sTop=="LinkedBoxIdentifier")      m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier, _AutoBind_(sData));
			if(l_sTop=="LinkedBoxOutputIndex")      m_pContext->processUInteger(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex, _AutoBind_(sData));
			break;

		case Status_ParsingScenarioAttribute:
			if(l_sTop=="Identifier")               m_pContext->processIdentifier(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier, _AutoBind_(sData));
			if(l_sTop=="Value")                    m_pContext->processString(OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value, _AutoBind_(sData));
			break;
	}
}

void CAlgorithmXMLScenarioImporter::closeChild(void)
{
	std::string& l_sTop=m_vNodes.top();

	if(false) { }

	else if(l_sTop=="OpenViBE-Scenario"   && m_ui32Status==Status_ParsingScenario)          { m_ui32Status=Status_ParsingNothing;  m_pContext->processStop(); }
	else if(l_sTop=="Setting"             && m_ui32Status==Status_ParsingScenarioSetting)   { m_ui32Status=Status_ParsingScenario; m_pContext->processStop(); }
	else if(l_sTop=="Input"               && m_ui32Status==Status_ParsingScenarioInput)     { m_ui32Status=Status_ParsingScenario; m_pContext->processStop(); }
	else if(l_sTop=="Output"              && m_ui32Status==Status_ParsingScenarioOutput)    { m_ui32Status=Status_ParsingScenario; m_pContext->processStop(); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingScenarioAttribute) { m_ui32Status=Status_ParsingScenario; m_pContext->processStop(); }

	else if(l_sTop=="Box"                 && m_ui32Status==Status_ParsingBox)               { m_ui32Status=Status_ParsingScenario; m_pContext->processStop(); }
	else if(l_sTop=="Input"               && m_ui32Status==Status_ParsingBoxInput)          { m_ui32Status=Status_ParsingBox;      m_pContext->processStop(); }

	else if(l_sTop=="Output"              && m_ui32Status==Status_ParsingBoxOutput)         { m_ui32Status=Status_ParsingBox;      m_pContext->processStop(); }
	else if(l_sTop=="Setting"             && m_ui32Status==Status_ParsingBoxSetting)        { m_ui32Status=Status_ParsingBox;      m_pContext->processStop(); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingBoxAttribute)      { m_ui32Status=Status_ParsingBox;      m_pContext->processStop(); }

	else if(l_sTop=="Comment"             && m_ui32Status==Status_ParsingComment)           { m_ui32Status=Status_ParsingScenario; m_pContext->processStop(); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingCommentAttribute)  { m_ui32Status=Status_ParsingComment;  m_pContext->processStop(); }

	else if(l_sTop=="Link"                && m_ui32Status==Status_ParsingLink)              { m_ui32Status=Status_ParsingScenario; m_pContext->processStop(); }
	else if(l_sTop=="Source"              && m_ui32Status==Status_ParsingLinkSource)        { m_ui32Status=Status_ParsingLink;     m_pContext->processStop(); }
	else if(l_sTop=="Target"              && m_ui32Status==Status_ParsingLinkTarget)        { m_ui32Status=Status_ParsingLink;     m_pContext->processStop(); }
	else if(l_sTop=="Attribute"           && m_ui32Status==Status_ParsingLinkAttribute)     { m_ui32Status=Status_ParsingLink;     m_pContext->processStop(); }

	m_vNodes.pop();
}

boolean CAlgorithmXMLScenarioImporter::import(IAlgorithmScenarioImporterContext& rContext, const IMemoryBuffer& rMemoryBuffer)
{
	m_pContext=&rContext;

	m_bScenarioRecognized = false;

	boolean m_bOk = m_pReader->processData(rMemoryBuffer.getDirectPointer(), rMemoryBuffer.getSize());

	if(!m_bScenarioRecognized)
	{
		// This is not a conforming openvibe XML scenario, lacking the <OpenViBE-Scenario> tag
		return false;
	}

	return m_bOk;
}
