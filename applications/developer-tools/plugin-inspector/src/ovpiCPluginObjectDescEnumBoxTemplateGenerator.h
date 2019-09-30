#pragma once

#include "ovpiCPluginObjectDescEnum.h"

#include <map>
#include <vector>
#include <string>

// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------------------

class CPluginObjectDescEnumBoxTemplateGenerator final : public CPluginObjectDescEnum
{
public:

	CPluginObjectDescEnumBoxTemplateGenerator(const OpenViBE::Kernel::IKernelContext& ctx, const OpenViBE::CString& docTemplateDirectory);
	bool callback(const OpenViBE::Plugins::IPluginObjectDesc& pluginObjectDesc) override;
	bool initialize();
	bool uninitialize();

private:
	// Return a string that correspond to the indexed categories under rst format
	std::string generateRstIndex(std::vector<std::pair<std::string, std::string>> categories);

protected:

	std::string m_docTemplateDirectory;
	std::vector<std::pair<std::string, std::string>> m_categories;
	std::vector<std::pair<std::string, std::string>> m_deprecatedBoxesCategories;
	OpenViBE::CIdentifier m_scenarioID = OV_UndefinedIdentifier;
	OpenViBE::Kernel::IScenario* m_scenario    = nullptr;
};
