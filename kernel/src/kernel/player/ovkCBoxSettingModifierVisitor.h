#pragma once

#include "ovkCSimulatedBox.h"
#include "ovkCPlayer.h"

#include <openvibe/ovIObjectVisitor.h>
#include <openvibe/kernel/ovIObjectVisitorContext.h>

#include <xml/IReader.h>

#define OVD_AttributeId_SettingOverrideFilename             OpenViBE::CIdentifier(0x8D21FF41, 0xDF6AFE7E)

class CBoxSettingModifierVisitor final : public OpenViBE::IObjectVisitor, public XML::IReaderCallback
{
public:

	explicit CBoxSettingModifierVisitor(OpenViBE::Kernel::IConfigurationManager* pConfigurationManager = nullptr) : OpenViBE::IObjectVisitor(), m_pConfigurationManager(pConfigurationManager) {}

	void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount) override;
	void processChildData(const char* sData) override;
	void closeChild() override;
	bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox) override;
	bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox) override;

	OpenViBE::Kernel::IObjectVisitorContext* m_pObjectVisitorContext = nullptr;
	OpenViBE::Kernel::IBox* m_pBox = nullptr;
	uint32_t m_ui32SettingIndex = 0;
	bool m_bIsParsingSettingValue = false;
	bool m_bIsParsingSettingOverride = false;
	OpenViBE::Kernel::IConfigurationManager* m_pConfigurationManager = nullptr;

	_IsDerivedFromClass_Final_(OpenViBE::IObjectVisitor, OV_UndefinedIdentifier)
};
