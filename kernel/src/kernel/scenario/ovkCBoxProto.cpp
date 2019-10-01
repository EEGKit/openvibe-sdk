#include "ovkCBoxProto.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

bool CBoxProto::addInput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify)
{
	if (!m_box.addInput(name, typeID, identifier, bNotify)) { return false; }

	const char* buffer = std::to_string(m_box.getInputCount()).c_str();
	if (m_box.hasAttribute(OV_AttributeId_Box_InitialInputCount)) { m_box.setAttributeValue(OV_AttributeId_Box_InitialInputCount, buffer); }
	else { m_box.addAttribute(OV_AttributeId_Box_InitialInputCount, buffer); }

	return true;
}

bool CBoxProto::addOutput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify)
{
	if (!m_box.addOutput(name, typeID, identifier, bNotify)) { return false; }

	const char* buffer = std::to_string(m_box.getOutputCount()).c_str();
	if (m_box.hasAttribute(OV_AttributeId_Box_InitialOutputCount)) { m_box.setAttributeValue(OV_AttributeId_Box_InitialOutputCount, buffer); }
	else { m_box.addAttribute(OV_AttributeId_Box_InitialOutputCount, buffer); }

	return true;
}

bool CBoxProto::addSetting(const CString& name, const CIdentifier& typeID, const CString& sDefaultValue, const bool bModifiable,
						   const CIdentifier& identifier, const bool bNotify)
{
	if (!m_box.addSetting(name, typeID, sDefaultValue, OV_Value_UndefinedIndexUInt, bModifiable, identifier, bNotify)) { return false; }

	const char* buffer = std::to_string(m_box.getSettingCount()).c_str();
	if (m_box.hasAttribute(OV_AttributeId_Box_InitialSettingCount)) { m_box.setAttributeValue(OV_AttributeId_Box_InitialSettingCount, buffer); }
	else { m_box.addAttribute(OV_AttributeId_Box_InitialSettingCount, buffer); }

	return true;
}
/*
uint32_t CBoxProto::addSetting(const OpenViBE::CString& name, const OpenViBE::CIdentifier& typeID, const OpenViBE::CString& sDefaultValue, const bool bModifiable)
{
	addSetting(name, typeID, sDefaultValue);
	uint32_t l_ui32LastSetting = m_box.getSettingCount();
	m_box.setSettingMod(l_ui32LastSetting, bModifiable);
	return true;
}
/*/

bool CBoxProto::addFlag(const EBoxFlag eBoxFlag)
{
	switch (eBoxFlag)
	{
		case BoxFlag_CanAddInput:		m_box.addAttribute(OV_AttributeId_Box_FlagCanAddInput, "");			break;
		case BoxFlag_CanModifyInput:	m_box.addAttribute(OV_AttributeId_Box_FlagCanModifyInput, "");		break;
		case BoxFlag_CanAddOutput:		m_box.addAttribute(OV_AttributeId_Box_FlagCanAddOutput, "");		break;
		case BoxFlag_CanModifyOutput:	m_box.addAttribute(OV_AttributeId_Box_FlagCanModifyOutput, "");		break;
		case BoxFlag_CanAddSetting:		m_box.addAttribute(OV_AttributeId_Box_FlagCanAddSetting, "");		break;
		case BoxFlag_CanModifySetting:	m_box.addAttribute(OV_AttributeId_Box_FlagCanModifySetting, "");	break;
		case BoxFlag_ManualUpdate:		m_box.addAttribute(OV_AttributeId_Box_FlagNeedsManualUpdate, "");	break;
		case BoxFlag_IsDeprecated: break;
		default: return false;
	}
	return true;
}

bool CBoxProto::addFlag(const CIdentifier& cIdentifierFlag)
{
	const uint64_t flagValue = getKernelContext().getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_BoxAlgorithmFlag, cIdentifierFlag.toString());
	if (flagValue == OV_UndefinedIdentifier) { return false; }
	m_box.addAttribute(cIdentifierFlag, "");
	return true;
}
