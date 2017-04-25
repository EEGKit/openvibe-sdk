#include "ovkCMetaboxObjectDesc.h"

using namespace OpenViBE::Metabox;

CMetaboxObjectDesc::CMetaboxObjectDesc(const OpenViBE::CString& rMetaboxDescriptor, OpenViBE::Kernel::IScenario& metaboxScenario)
	: m_MetaboxDescriptor(rMetaboxDescriptor)
	, m_Name(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Name))
	, m_AuthorName(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Author))
	, m_AuthorCompanyName(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Company))
	, m_ShortDescription(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_ShortDescription))
	, m_DetailedDescription(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_DetailedDescription))
	, m_Category( metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Category))
	, m_Version(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Version))
	, m_StockItemName("")
	, m_AddedSoftwareVersion(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_AddedSoftwareVersion))
	, m_UpdatedSoftwareVersion(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_UpdatedSoftwareVersion))
	, m_MetaboxIdentifier(metaboxScenario.getAttributeValue(OVP_AttributeId_Metabox_Identifier))
{

	for (uint32 l_ui32ScenarioInputIndex = 0; l_ui32ScenarioInputIndex < metaboxScenario.getInputCount(); l_ui32ScenarioInputIndex++)
	{
		CString l_sInputName;
		CIdentifier l_oInputTypeIdentifier;

		metaboxScenario.getInputType(l_ui32ScenarioInputIndex, l_oInputTypeIdentifier);
		metaboxScenario.getInputName(l_ui32ScenarioInputIndex, l_sInputName);

		m_Inputs.push_back(SIOStream(l_sInputName, l_oInputTypeIdentifier));
	}

	for (uint32 l_ui32ScenarioOutputIndex = 0; l_ui32ScenarioOutputIndex < metaboxScenario.getOutputCount(); l_ui32ScenarioOutputIndex++)
	{
		CString l_sOutputName;
		CIdentifier l_oOutputTypeIdentifier;

		metaboxScenario.getOutputType(l_ui32ScenarioOutputIndex, l_oOutputTypeIdentifier);
		metaboxScenario.getOutputName(l_ui32ScenarioOutputIndex, l_sOutputName);

		m_Outputs.push_back(SIOStream(l_sOutputName, l_oOutputTypeIdentifier));
	}

	for (uint32 l_ui32ScenarioSettingIndex = 0; l_ui32ScenarioSettingIndex < metaboxScenario.getSettingCount(); l_ui32ScenarioSettingIndex++)
	{
		CString l_sSettingName;
		CIdentifier l_oSettingTypeIdentifier;
		CString l_sSettingDefaultValue;

		metaboxScenario.getSettingName(l_ui32ScenarioSettingIndex, l_sSettingName);
		metaboxScenario.getSettingType(l_ui32ScenarioSettingIndex, l_oSettingTypeIdentifier);
		metaboxScenario.getSettingDefaultValue(l_ui32ScenarioSettingIndex, l_sSettingDefaultValue);

		m_Settings.push_back(SSetting(l_sSettingName, l_oSettingTypeIdentifier, l_sSettingDefaultValue));
	}
}

bool CMetaboxObjectDesc::getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
{
	for (auto& input : m_Inputs)
	{
		rBoxAlgorithmPrototype.addInput(input.m_sName, input.m_oTypeIdentifier);
	}

	for (auto& output : m_Outputs)
	{
		rBoxAlgorithmPrototype.addOutput(output.m_sName, output.m_oTypeIdentifier);
	}

	for (auto& setting : m_Settings)
	{
		rBoxAlgorithmPrototype.addSetting(setting.m_sName, setting.m_oTypeIdentifier, setting.m_sDefaultValue);
	}

	return true;
}

