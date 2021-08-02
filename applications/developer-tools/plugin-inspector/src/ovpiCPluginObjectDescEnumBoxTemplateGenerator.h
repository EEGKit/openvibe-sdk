#pragma once

#include "ovpiCPluginObjectDescEnum.h"

#include <map>
#include <vector>
#include <string>

namespace OpenViBE {
namespace PluginInspector {
class CPluginObjectDescEnumBoxTemplateGenerator final : public CPluginObjectDescEnum
{
public:

	CPluginObjectDescEnumBoxTemplateGenerator(const Kernel::IKernelContext& ctx, const std::string& docTemplateDir)
		: CPluginObjectDescEnum(ctx), m_docTemplateDirectory(docTemplateDir) {}

	bool callback(const Plugins::IPluginObjectDesc& pod) override;
	bool initialize();
	bool uninitialize();

private:
	// Return a string that correspond to the indexed categories under rst format
	std::string generateRstIndex(std::vector<std::pair<std::string, std::string>> categories) const;

protected:

	std::string m_docTemplateDirectory;
	std::vector<std::pair<std::string, std::string>> m_categories;
	std::vector<std::pair<std::string, std::string>> m_deprecatedBoxesCategories;
	CIdentifier m_scenarioID      = CIdentifier::undefined();
	Kernel::IScenario* m_scenario = nullptr;
};
}  // namespace PluginInspector
}  // namespace OpenViBE
