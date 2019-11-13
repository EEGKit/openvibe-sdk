#pragma once

#include "ovkCSimulatedBox.h"
#include "ovkCPlayer.h"

#include <openvibe/ovIObjectVisitor.h>
#include <openvibe/kernel/ovIObjectVisitorContext.h>

#include <xml/IReader.h>

#define OVD_AttributeId_SettingOverrideFilename			OpenViBE::CIdentifier(0x8D21FF41, 0xDF6AFE7E)

class CBoxSettingModifierVisitor final : public OpenViBE::IObjectVisitor, public XML::IReaderCallback
{
public:

	explicit CBoxSettingModifierVisitor(OpenViBE::Kernel::IConfigurationManager* pConfigurationManager = nullptr)
		: IObjectVisitor(), m_ConfigManager(pConfigurationManager) {}

	void openChild(const char* name, const char** sAttributeName, const char** sAttributeValue, const size_t nAttribute) override;
	void processChildData(const char* data) override;
	void closeChild() override;
	bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& box) override;
	bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& box) override;

	OpenViBE::Kernel::IObjectVisitorContext* m_ObjectVisitorCtx = nullptr;
	OpenViBE::Kernel::IBox* m_Box                               = nullptr;
	size_t m_SettingIdx                                         = 0;
	bool m_IsParsingSettingValue                                = false;
	bool m_IsParsingSettingOverride                             = false;
	OpenViBE::Kernel::IConfigurationManager* m_ConfigManager    = nullptr;

	_IsDerivedFromClass_Final_(OpenViBE::IObjectVisitor, OV_UndefinedIdentifier)
};
