#include "ovkCMetaboxObjectDesc.h"

using namespace OpenViBE;
using namespace Metabox;

CMetaboxObjectDesc::CMetaboxObjectDesc(const CString& rMetaboxDescriptor, Kernel::IScenario& metaboxScenario)
	: m_MetaboxDescriptor(rMetaboxDescriptor)
	  , m_Name(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Name))
	  , m_AuthorName(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Author))
	  , m_AuthorCompanyName(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Company))
	  , m_ShortDescription(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_ShortDescription))
	  , m_DetailedDescription(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_DetailedDescription))
	  , m_Category(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Category))
	  , m_Version(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Version))
	  , m_StockItemName("")
	  , m_AddedSoftwareVersion(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_AddedSoftwareVersion))
	  , m_UpdatedSoftwareVersion(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_UpdatedSoftwareVersion))
	  , m_MetaboxIdentifier(metaboxScenario.getAttributeValue(OVP_AttributeId_Metabox_Identifier))
{
	for (uint32_t l_ui32ScenarioInputIndex = 0; l_ui32ScenarioInputIndex < metaboxScenario.getInputCount(); l_ui32ScenarioInputIndex++)
	{
		CString inputName;
		CIdentifier inputTypeID;
		CIdentifier InputID;

		metaboxScenario.getInputType(l_ui32ScenarioInputIndex, inputTypeID);
		metaboxScenario.getInputName(l_ui32ScenarioInputIndex, inputName);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Input, l_ui32ScenarioInputIndex, InputID);

		m_Inputs.push_back(SIOStream(inputName, inputTypeID, InputID));
	}

	for (uint32_t l_ui32ScenarioOutputIndex = 0; l_ui32ScenarioOutputIndex < metaboxScenario.getOutputCount(); l_ui32ScenarioOutputIndex++)
	{
		CString outputName;
		CIdentifier OutputTypeID;
		CIdentifier OutputID;

		metaboxScenario.getOutputType(l_ui32ScenarioOutputIndex, OutputTypeID);
		metaboxScenario.getOutputName(l_ui32ScenarioOutputIndex, outputName);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Output, l_ui32ScenarioOutputIndex, OutputID);

		m_Outputs.push_back(SIOStream(outputName, OutputTypeID, OutputID));
	}

	for (uint32_t l_ui32ScenarioSettingIndex = 0; l_ui32ScenarioSettingIndex < metaboxScenario.getSettingCount(); l_ui32ScenarioSettingIndex++)
	{
		CString l_sSettingName;
		CIdentifier l_oSettingTypeIdentifier;
		CString l_sSettingDefaultValue;
		CIdentifier l_oSettingIdentifier;

		metaboxScenario.getSettingName(l_ui32ScenarioSettingIndex, l_sSettingName);
		metaboxScenario.getSettingType(l_ui32ScenarioSettingIndex, l_oSettingTypeIdentifier);
		metaboxScenario.getSettingDefaultValue(l_ui32ScenarioSettingIndex, l_sSettingDefaultValue);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Setting, l_ui32ScenarioSettingIndex, l_oSettingIdentifier);


		m_Settings.push_back(SSetting(l_sSettingName, l_oSettingTypeIdentifier, l_sSettingDefaultValue, l_oSettingIdentifier));
	}
}

bool CMetaboxObjectDesc::getBoxPrototype(Kernel::IBoxProto& prototype) const
{
	for (auto& input : m_Inputs) { prototype.addInput(input.m_sName, input.m_typeID, input.m_oIdentifier); }

	for (auto& output : m_Outputs) { prototype.addOutput(output.m_sName, output.m_typeID, output.m_oIdentifier); }

	for (auto& setting : m_Settings)
	{
		prototype.addSetting(setting.m_sName, setting.m_typeID, setting.m_sDefaultValue, false, setting.m_oIdentifier);
	}

	return true;
}
