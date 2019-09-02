#include <cstdio>
#include <cstring>
#include "ovpCAlgorithmXMLScenarioExporter.h"

//___________________________________________________________________//
//                                                                   //

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace FileIO;
using namespace std;

//___________________________________________________________________//
//                                                                   //

CAlgorithmXMLScenarioExporter::CAlgorithmXMLScenarioExporter() { m_pWriter = createWriter(*this); }

CAlgorithmXMLScenarioExporter::~CAlgorithmXMLScenarioExporter() { m_pWriter->release(); }

void CAlgorithmXMLScenarioExporter::write(const char* sString) { m_pMemoryBuffer->append(reinterpret_cast<const uint8_t*>(sString), strlen(sString)); }

bool CAlgorithmXMLScenarioExporter::exportStart(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier)
{
	m_pMemoryBuffer = &rMemoryBuffer;

	CString l_sNodeName;

	if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario) l_sNodeName = "OpenViBE-Scenario";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Settings) l_sNodeName = "Settings";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting) l_sNodeName = "Setting";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier) l_sNodeName = "TypeIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name) l_sNodeName = "Name";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue) l_sNodeName = "DefaultValue";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value) l_sNodeName = "Value";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Inputs) l_sNodeName = "Inputs";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input) l_sNodeName = "Input";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier) l_sNodeName = "TypeIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name) l_sNodeName = "Name";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier) l_sNodeName = "LinkedBoxIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex) l_sNodeName = "LinkedBoxInputIndex";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdentifier) l_sNodeName = "LinkedBoxInputIdentifier";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Outputs) l_sNodeName = "Outputs";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output) l_sNodeName = "Output";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier) l_sNodeName = "TypeIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name) l_sNodeName = "Name";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier) l_sNodeName = "LinkedBoxIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex) l_sNodeName = "LinkedBoxOutputIndex";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdentifier) l_sNodeName = "LinkedBoxOutputIdentifier";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_FormatVersion) l_sNodeName = "FormatVersion";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Creator) l_sNodeName = "Creator";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_CreatorVersion) l_sNodeName = "CreatorVersion";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Boxes) l_sNodeName = "Boxes";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box) l_sNodeName = "Box";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name) l_sNodeName = "Name";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier) l_sNodeName = "AlgorithmClassIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs) l_sNodeName = "Inputs";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input) l_sNodeName = "Input";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier) l_sNodeName = "TypeIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name) l_sNodeName = "Name";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs) l_sNodeName = "Outputs";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output) l_sNodeName = "Output";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier) l_sNodeName = "TypeIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name) l_sNodeName = "Name";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings) l_sNodeName = "Settings";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting) l_sNodeName = "Setting";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier) l_sNodeName = "TypeIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name) l_sNodeName = "Name";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue) l_sNodeName = "DefaultValue";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value) l_sNodeName = "Value";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability) l_sNodeName = "Modifiability";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attributes) l_sNodeName = "Attributes";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute) l_sNodeName = "Attribute";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value) l_sNodeName = "Value";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comments) l_sNodeName = "Comments";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment) l_sNodeName = "Comment";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text) l_sNodeName = "Text";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attributes) l_sNodeName = "Attributes";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute) l_sNodeName = "Attribute";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value) l_sNodeName = "Value";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Metadata) l_sNodeName = "Metadata";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry) l_sNodeName = "Entry";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type) l_sNodeName = "Type";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data) l_sNodeName = "Data";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Links) l_sNodeName = "Links";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link) l_sNodeName = "Link";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source) l_sNodeName = "Source";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier) l_sNodeName = "BoxIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex) l_sNodeName = "BoxOutputIndex";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdentifier) l_sNodeName = "BoxOutputIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target) l_sNodeName = "Target";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier) l_sNodeName = "BoxIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex) l_sNodeName = "BoxInputIndex";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdentifier) l_sNodeName = "BoxInputIdentifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attributes) l_sNodeName = "Attributes";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute) l_sNodeName = "Attribute";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value) l_sNodeName = "Value";

	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attributes) l_sNodeName = "Attributes";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute) l_sNodeName = "Attribute";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier) l_sNodeName = "Identifier";
	else if (rIdentifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value) l_sNodeName = "Value";
		//
	else
		OV_ERROR_KRF("(start) Unexpected node identifier " << rIdentifier.toString(), OpenViBE::Kernel::ErrorType::BadArgument);

	m_pWriter->openChild(l_sNodeName.toASCIIString());

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportIdentifier(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, const CIdentifier& rValue)
{
	m_pMemoryBuffer = &rMemoryBuffer;

	OV_ERROR_UNLESS_KRF(this->exportStart(rMemoryBuffer, rIdentifier), "Exporting identifier failed", OpenViBE::Kernel::ErrorType::Internal);

	m_pWriter->setChildData(rValue.toString().toASCIIString());
	this->exportStop(rMemoryBuffer);

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportString(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, const CString& rValue)
{
	m_pMemoryBuffer = &rMemoryBuffer;

	OV_ERROR_UNLESS_KRF(this->exportStart(rMemoryBuffer, rIdentifier), "Exporting string failed", OpenViBE::Kernel::ErrorType::Internal);

	m_pWriter->setChildData(rValue.toASCIIString());
	this->exportStop(rMemoryBuffer);

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportUInteger(IMemoryBuffer& memoryBuffer, const CIdentifier& identifier, const uint64_t value)
{
	char l_sValue[1024];
	sprintf(l_sValue, "%lu", value);

	m_pMemoryBuffer = &memoryBuffer;

	OV_ERROR_UNLESS_KRF(this->exportStart(memoryBuffer, identifier), "Exporting uint failed", OpenViBE::Kernel::ErrorType::Internal);

	m_pWriter->setChildData(l_sValue);
	this->exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportStop(IMemoryBuffer& rMemoryBuffer)
{
	m_pMemoryBuffer = &rMemoryBuffer;

	m_pWriter->closeChild();

	return true;
}
