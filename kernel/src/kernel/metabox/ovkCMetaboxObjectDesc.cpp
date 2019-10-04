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
	  , m_MetaboxID(metaboxScenario.getAttributeValue(OVP_AttributeId_Metabox_Identifier))
{
	for (uint32_t l_ui32ScenarioInputIdx = 0; l_ui32ScenarioInputIdx < metaboxScenario.getInputCount(); l_ui32ScenarioInputIdx++)
	{
		CString inputName;
		CIdentifier inputTypeID;
		CIdentifier InputID;

		metaboxScenario.getInputType(l_ui32ScenarioInputIdx, inputTypeID);
		metaboxScenario.getInputName(l_ui32ScenarioInputIdx, inputName);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Input, l_ui32ScenarioInputIdx, InputID);

		m_Inputs.push_back(SIOStream(inputName, inputTypeID, InputID));
	}

	for (uint32_t l_ui32ScenarioOutputIdx = 0; l_ui32ScenarioOutputIdx < metaboxScenario.getOutputCount(); l_ui32ScenarioOutputIdx++)
	{
		CString outputName;
		CIdentifier OutputTypeID;
		CIdentifier OutputID;

		metaboxScenario.getOutputType(l_ui32ScenarioOutputIdx, OutputTypeID);
		metaboxScenario.getOutputName(l_ui32ScenarioOutputIdx, outputName);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Output, l_ui32ScenarioOutputIdx, OutputID);

		m_Outputs.push_back(SIOStream(outputName, OutputTypeID, OutputID));
	}

	for (uint32_t l_ui32ScenarioSettingIdx = 0; l_ui32ScenarioSettingIdx < metaboxScenario.getSettingCount(); l_ui32ScenarioSettingIdx++)
	{
		CString l_sSettingName;
		CIdentifier l_oSettingTypeID;
		CString l_sSettingDefaultValue;
		CIdentifier settingID;

		metaboxScenario.getSettingName(l_ui32ScenarioSettingIdx, l_sSettingName);
		metaboxScenario.getSettingType(l_ui32ScenarioSettingIdx, l_oSettingTypeID);
		metaboxScenario.getSettingDefaultValue(l_ui32ScenarioSettingIdx, l_sSettingDefaultValue);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Setting, l_ui32ScenarioSettingIdx, settingID);


		m_Settings.push_back(SSetting(l_sSettingName, l_oSettingTypeID, l_sSettingDefaultValue, settingID));
	}
}

bool CMetaboxObjectDesc::getBoxPrototype(Kernel::IBoxProto& prototype) const
{
	for (auto& input : m_Inputs) { prototype.addInput(input.m_name, input.m_typeID, input.m_id); }

	for (auto& output : m_Outputs) { prototype.addOutput(output.m_name, output.m_typeID, output.m_id); }

	for (auto& setting : m_Settings)
	{
		prototype.addSetting(setting.m_name, setting.m_typeID, setting.m_defaultValue, false, setting.m_id);
	}

	return true;
}
