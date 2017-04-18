#include "ovkCBoxProto.h"

#include <cstring>
#include <cstdlib>
#include <cstdio>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

CBoxProto::CBoxProto(const IKernelContext& rKernelContext, IBox& rBox)
	:TKernelObject<IBoxProto>(rKernelContext)
	,m_rBox(rBox)
{
}

bool CBoxProto::addInput(
	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	if(!m_rBox.addInput(sName, rTypeIdentifier))
	{
		return false;
	}

	char l_sBuffer[1024];
	::sprintf(l_sBuffer, "%d", m_rBox.getInputCount());
	if(m_rBox.hasAttribute(OV_AttributeId_Box_InitialInputCount))
	{
		m_rBox.setAttributeValue(OV_AttributeId_Box_InitialInputCount, l_sBuffer);
	}
	else
	{
		m_rBox.addAttribute(OV_AttributeId_Box_InitialInputCount, l_sBuffer);
	}

	return true;
}

bool CBoxProto::addOutput(

	const CString& sName,
	const CIdentifier& rTypeIdentifier)
{
	if(!m_rBox.addOutput(sName, rTypeIdentifier))
	{
		return false;
	}

	char l_sBuffer[1024];
	::sprintf(l_sBuffer, "%d", m_rBox.getOutputCount());
	if(m_rBox.hasAttribute(OV_AttributeId_Box_InitialOutputCount))
	{
		m_rBox.setAttributeValue(OV_AttributeId_Box_InitialOutputCount, l_sBuffer);
	}
	else
	{
		m_rBox.addAttribute(OV_AttributeId_Box_InitialOutputCount, l_sBuffer);
	}

	return true;
}

bool CBoxProto::addSetting(
	const CString& sName,
	const CIdentifier& rTypeIdentifier,
	const CString& sDefaultValue,
	const bool bModifiable)
{
	if(!m_rBox.addSetting(sName, rTypeIdentifier, sDefaultValue, -1, bModifiable))
	{
		return false;
	}

	char l_sBuffer[1024];
	::sprintf(l_sBuffer, "%d", m_rBox.getSettingCount());
	if(m_rBox.hasAttribute(OV_AttributeId_Box_InitialSettingCount))
	{
		m_rBox.setAttributeValue(OV_AttributeId_Box_InitialSettingCount, l_sBuffer);
	}
	else
	{
		m_rBox.addAttribute(OV_AttributeId_Box_InitialSettingCount, l_sBuffer);
	}

	return true;
}
/*
uint32 CBoxProto::addSetting(
	const OpenViBE::CString& sName,
	const OpenViBE::CIdentifier& rTypeIdentifier,
	const OpenViBE::CString& sDefaultValue,
	const bool bModifiable)
{
	addSetting(sName, rTypeIdentifier, sDefaultValue);
	uint32 l_ui32LastSetting = m_rBox.getSettingCount();
	m_rBox.setSettingMod(l_ui32LastSetting, bModifiable);
	return true;

}
/*/

bool CBoxProto::addFlag(const EBoxFlag eBoxFlag)
{
	switch (eBoxFlag)
	{
		case BoxFlag_CanAddInput:      m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddInput,      ""); break;
		case BoxFlag_CanModifyInput:   m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifyInput,   ""); break;
		case BoxFlag_CanAddOutput:     m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddOutput,     ""); break;
		case BoxFlag_CanModifyOutput:  m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifyOutput,  ""); break;
		case BoxFlag_CanAddSetting:    m_rBox.addAttribute(OV_AttributeId_Box_FlagCanAddSetting,    ""); break;
		case BoxFlag_CanModifySetting: m_rBox.addAttribute(OV_AttributeId_Box_FlagCanModifySetting, ""); break;
		case BoxFlag_IsDeprecated:
			break;
		default:
			return false;
	}
	return true;
}

bool CBoxProto::addFlag(const OpenViBE::CIdentifier& cIdentifierFlag)
{
	uint64_t flagValue = getKernelContext().getTypeManager().getEnumerationEntryValueFromName(OV_TypeId_BoxAlgorithmFlag, cIdentifierFlag.toString());
	if (flagValue == OV_UndefinedIdentifier)
	{
		return false;
	}
	m_rBox.addAttribute(cIdentifierFlag, "");
	return true;
}

bool CBoxProto::addInputSupport(const OpenViBE::CIdentifier &rTypeIdentifier)
{
	return m_rBox.addInputSupport(rTypeIdentifier);
}

bool CBoxProto::addOutputSupport(const OpenViBE::CIdentifier &rTypeIdentifier)
{
	return m_rBox.addOutputSupport(rTypeIdentifier);
}
