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

	CPluginObjectDescEnumBoxTemplateGenerator(const OpenViBE::Kernel::IKernelContext& kernelContext, const OpenViBE::CString& docTemplateDirectory);
	bool callback(const OpenViBE::Plugins::IPluginObjectDesc& pluginObjectDesc) override;
	bool initialize();
	bool uninitialize();

private:
	// Return a string that correspond to the indexed categories under rst format
	std::string generateRstIndex(std::vector<std::pair<std::string, std::string>> categories);

protected:

	std::string m_DocTemplateDirectory;
	std::vector<std::pair<std::string, std::string>> m_Categories;
	std::vector<std::pair<std::string, std::string>> m_DeprecatedBoxesCategories;
	OpenViBE::CIdentifier m_ScenarioIdentifier = OV_UndefinedIdentifier;
	OpenViBE::Kernel::IScenario* m_Scenario    = nullptr;
};
