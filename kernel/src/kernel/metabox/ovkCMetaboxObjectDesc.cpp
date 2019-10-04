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
	for (uint32_t scenarioInputIdx = 0; scenarioInputIdx < metaboxScenario.getInputCount(); scenarioInputIdx++)
	{
		CString inputName;
		CIdentifier inputTypeID;
		CIdentifier InputID;

		metaboxScenario.getInputType(scenarioInputIdx, inputTypeID);
		metaboxScenario.getInputName(scenarioInputIdx, inputName);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Input, scenarioInputIdx, InputID);

		m_Inputs.push_back(SIOStream(inputName, inputTypeID, InputID));
	}

	for (uint32_t scenarioOutputIdx = 0; scenarioOutputIdx < metaboxScenario.getOutputCount(); scenarioOutputIdx++)
	{
		CString outputName;
		CIdentifier OutputTypeID;
		CIdentifier OutputID;

		metaboxScenario.getOutputType(scenarioOutputIdx, OutputTypeID);
		metaboxScenario.getOutputName(scenarioOutputIdx, outputName);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Output, scenarioOutputIdx, OutputID);

		m_Outputs.push_back(SIOStream(outputName, OutputTypeID, OutputID));
	}

	for (uint32_t scenarioSettingIdx = 0; scenarioSettingIdx < metaboxScenario.getSettingCount(); scenarioSettingIdx++)
	{
		CString l_sSettingName;
		CIdentifier l_oSettingTypeID;
		CString l_sSettingDefaultValue;
		CIdentifier settingID;

		metaboxScenario.getSettingName(scenarioSettingIdx, l_sSettingName);
		metaboxScenario.getSettingType(scenarioSettingIdx, l_oSettingTypeID);
		metaboxScenario.getSettingDefaultValue(scenarioSettingIdx, l_sSettingDefaultValue);
		metaboxScenario.getInterfacorIdentifier(Kernel::EBoxInterfacorType::Setting, scenarioSettingIdx, settingID);


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
