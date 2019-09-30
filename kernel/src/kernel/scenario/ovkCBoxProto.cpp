#include "ovkCBoxProto.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

CBoxProto::CBoxProto(const IKernelContext& ctx, IBox& box)
	: TKernelObject<IBoxProto>(ctx), m_rBox(box) {}

bool CBoxProto::addInput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify)
{
	if (!m_rBox.addInput(name, typeID, identifier, bNotify)) { return false; }

	char buffer[1024];
	sprintf(buffer, "%d", m_rBox.getInputCount());
	if (m_rBox.hasAttribute(OV_AttributeId_Box_InitialInputCount)) { m_rBox.setAttributeValue(OV_AttributeId_Box_InitialInputCount, buffer); }
	else { m_rBox.addAttribute(OV_AttributeId_Box_InitialInputCount, buffer); }

	return true;
}

bool CBoxProto::addOutput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify)
{
	if (!m_rBox.addOutput(name, typeID, identifier, bNotify)) { return false; }

	char buffer[1024];
	sprintf(buffer, "%d", m_rBox.getOutputCount());
	if (m_rBox.hasAttribute(OV_AttributeId_Box_InitialOutputCount)) { m_rBox.setAttributeValue(OV_AttributeId_Box_InitialOutputCount, buffer); }
	else { m_rBox.addAttribute(OV_AttributeId_Box_InitialOutputCount, buffer); }

	return true;
}

bool CBoxProto::addSetting(const CString& name, const CIdentifier& typeID, const CString& sDefaultValue, const bool bModifiable,
						   const CIdentifier& identifier, const bool bNotify)
{
	if (!m_rBox.addSetting(name, typeID, sDefaultValue, OV_Value_UndefinedIndexUInt, bModifiable, identifier, bNotify)) { return false; }

	char buffer[1024];
	sprintf(buffer, "%d", m_rBox.getSettingCount());
	if (m_rBox.hasAttribute(OV_AttributeId_Box_InitialSettingCount)) { m_rBox.setAttributeValue(OV_AttributeId_Box_InitialSettingCount, buffer); }
	else { m_rBox.addAttribute(OV_AttributeId_Box_InitialSettingCount, buffer); }

	return true;
}
/*
uint32_t CBoxProto::addSetting(const OpenViBE::CString& name, const OpenViBE::CIdentifier& typeID, const OpenViBE::CString& sDefaultValue, const bool bModifiable)
{
	addSetting(name, typeID, sDefaultValue);
	uint32_t l_ui32LastSetting = m_rBox.getSettingCount();
	m_rBox.setSettingMod(l_ui32LastSetting, bModifiable);
	return true;
}
/*/

bool CBoxProto::addFlag(const EBoxFlag eBoxFlag)
{
	switch (eBoxFlag)
	{
		case BoxFlag_CanAddInput: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddInput, "");
			break;
		case BoxFlag_CanModifyInput: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifyInput, "");
			break;
		case BoxFlag_CanAddOutput: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddOutput, "");
			break;
		case BoxFlag_CanModifyOutput: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifyOutput, "");
			break;
		case BoxFlag_CanAddSetting: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddSetting, "");
			break;
		case BoxFlag_CanModifySetting: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifySetting, "");
			break;
		case BoxFlag_ManualUpdate: m_rBox.addAttribute(OV_AttributeId_Box_FlagNeedsManualUpdate, "");
			break;
		case BoxFlag_IsDeprecated:
			break;
		default:
			return false;
	}
	return true;
}

bool CBoxProto::addFlag(const CIdentifier& cIdentifierFlag)
{
	uint64_t flagValue = getKernelContext().getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_BoxAlgorithmFlag, cIdentifierFlag.toString());
	if (flagValue == OV_UndefinedIdentifier) { return false; }
	m_rBox.addAttribute(cIdentifierFlag, "");
	return true;
}

bool CBoxProto::addInputSupport(const CIdentifier& typeID) { return m_rBox.addInputSupport(typeID); }

bool CBoxProto::addOutputSupport(const CIdentifier& typeID) { return m_rBox.addOutputSupport(typeID); }
