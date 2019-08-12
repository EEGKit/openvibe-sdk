#pragma once

#include "ovkCSimulatedBox.h"
#include "ovkCPlayer.h"

#include <openvibe/ovIObjectVisitor.h>
#include <openvibe/kernel/ovIObjectVisitorContext.h>

#include <xml/IReader.h>

#define OVD_AttributeId_SettingOverrideFilename             OpenViBE::CIdentifier(0x8D21FF41, 0xDF6AFE7E)

class CBoxSettingModifierVisitor : public OpenViBE::IObjectVisitor, public XML::IReaderCallback
{
public:


	explicit CBoxSettingModifierVisitor(OpenViBE::Kernel::IConfigurationManager* pConfigurationManager = nullptr) : IObjectVisitor(),
																												 m_pObjectVisitorContext(nullptr),
																												 m_pBox(nullptr),
																												 m_pConfigurationManager(pConfigurationManager) {}

	void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount) override;
	void processChildData(const char* sData) override;
	void closeChild() override;
	bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox) override;
	bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox) override;

	OpenViBE::Kernel::IObjectVisitorContext* m_pObjectVisitorContext;
	OpenViBE::Kernel::IBox* m_pBox;
	uint32_t m_ui32SettingIndex = 0;
	bool m_bIsParsingSettingValue;
	bool m_bIsParsingSettingOverride;
	OpenViBE::Kernel::IConfigurationManager* m_pConfigurationManager;

	_IsDerivedFromClass_Final_(OpenViBE::IObjectVisitor, OV_UndefinedIdentifier)
};
