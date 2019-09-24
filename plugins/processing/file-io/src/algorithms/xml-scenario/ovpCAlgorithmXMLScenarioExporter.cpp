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

void CAlgorithmXMLScenarioExporter::write(const char* str) { m_pMemoryBuffer->append(reinterpret_cast<const uint8_t*>(str), strlen(str)); }

bool CAlgorithmXMLScenarioExporter::exportStart(IMemoryBuffer& memoryBuffer, const CIdentifier& id)
{
	m_pMemoryBuffer = &memoryBuffer;

	CString name;

	if (id == OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario) { name = "OpenViBE-Scenario"; }

	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Settings) {  name = "Settings"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting) { name = "Setting"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier) { name = "TypeIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name) { name = "Name"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue) { name = "DefaultValue"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value) { name = "Value"; }

	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Inputs) { name = "Inputs"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input) { name = "Input"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier) { name = "TypeIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name) { name = "Name"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier) { name = "LinkedBoxIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex) { name = "LinkedBoxInputIndex"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdentifier) { name = "LinkedBoxInputIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Outputs) { name = "Outputs"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output) { name = "Output"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier) { name = "TypeIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name) { name = "Name"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier) { name = "LinkedBoxIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex) { name = "LinkedBoxOutputIndex"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdentifier) { name = "LinkedBoxOutputIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_FormatVersion) { name = "FormatVersion"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Creator) { name = "Creator"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_CreatorVersion) { name = "CreatorVersion"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Boxes) { name = "Boxes"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box) { name = "Box"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name) { name = "Name"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier) { name = "AlgorithmClassIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs) { name = "Inputs"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input) { name = "Input"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier) { name = "TypeIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name) { name = "Name"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs) { name = "Outputs"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output) { name = "Output"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier) { name = "TypeIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name) { name = "Name"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings) { name = "Settings"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting) { name = "Setting"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier) { name = "TypeIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name) { name = "Name"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue) { name = "DefaultValue"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value) { name = "Value"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability) { name = "Modifiability"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attributes) { name = "Attributes"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute) { name = "Attribute"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value) { name = "Value"; }

	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comments) { name = "Comments"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comment) { name = "Comment"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text) { name = "Text"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attributes) { name = "Attributes"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute) { name = "Attribute"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value) { name = "Value"; }

	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Metadata) { name = "Metadata"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry) { name = "Entry"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type) { name = "Type"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data) { name = "Data"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Links) { name = "Links"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link) { name = "Link"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source) { name = "Source"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier) { name = "BoxIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex) { name = "BoxOutputIndex"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdentifier) { name = "BoxOutputIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target) { name = "Target"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier) { name = "BoxIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex) { name = "BoxInputIndex"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdentifier) { name = "BoxInputIdentifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attributes) { name = "Attributes"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute) { name = "Attribute"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value) { name = "Value"; }

	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attributes) { name = "Attributes"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute) { name = "Attribute"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier) { name = "Identifier"; }
	else if (id == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value) { name = "Value"; }
		//
	else { OV_ERROR_KRF("(start) Unexpected node identifier " << id.toString(), OpenViBE::Kernel::ErrorType::BadArgument); }

	m_pWriter->openChild(name.toASCIIString());

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportIdentifier(IMemoryBuffer& memoryBuffer, const CIdentifier& id, const CIdentifier& value)
{
	m_pMemoryBuffer = &memoryBuffer;

	OV_ERROR_UNLESS_KRF(this->exportStart(memoryBuffer, id), "Exporting identifier failed", OpenViBE::Kernel::ErrorType::Internal);

	m_pWriter->setChildData(value.toString().toASCIIString());
	this->exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportString(IMemoryBuffer& memoryBuffer, const CIdentifier& id, const CString& value)
{
	m_pMemoryBuffer = &memoryBuffer;

	OV_ERROR_UNLESS_KRF(this->exportStart(memoryBuffer, id), "Exporting string failed", OpenViBE::Kernel::ErrorType::Internal);

	m_pWriter->setChildData(value.toASCIIString());
	this->exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportUInteger(IMemoryBuffer& memoryBuffer, const CIdentifier& id, const uint64_t value)
{
	char str[1024];
	sprintf(str, "%llu", value);

	m_pMemoryBuffer = &memoryBuffer;

	OV_ERROR_UNLESS_KRF(this->exportStart(memoryBuffer, id), "Exporting uint failed", OpenViBE::Kernel::ErrorType::Internal);

	m_pWriter->setChildData(str);
	this->exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmXMLScenarioExporter::exportStop(IMemoryBuffer& memoryBuffer)
{
	m_pMemoryBuffer = &memoryBuffer;
	m_pWriter->closeChild();
	return true;
}
