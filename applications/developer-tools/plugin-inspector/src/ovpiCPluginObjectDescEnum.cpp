#include "ovpiCPluginObjectDescEnum.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace std;

CPluginObjectDescEnum::CPluginObjectDescEnum(const IKernelContext& ctx)
	: m_kernelCtx(ctx) {}

CPluginObjectDescEnum::~CPluginObjectDescEnum() {}

// Enumerate plugins by iterating over a user defined list of descriptors, used for metaboxes
bool CPluginObjectDescEnum::enumeratePluginObjectDesc(std::vector<const IPluginObjectDesc*>& pluginDescriptors)
{
	for (auto plugin : pluginDescriptors) { this->callback(*plugin); }
	return true;
}

bool CPluginObjectDescEnum::enumeratePluginObjectDesc(const CIdentifier& parentClassIdentifier)
{
	CIdentifier identifier;
	while ((identifier = m_kernelCtx.getPluginManager().getNextPluginObjectDescIdentifier(identifier, parentClassIdentifier)) != OV_UndefinedIdentifier)
	{
		this->callback(*m_kernelCtx.getPluginManager().getPluginObjectDesc(identifier));
	}
	return true;
}

std::string CPluginObjectDescEnum::transform(const std::string& sInput, const bool bRemoveSlash)
{
	std::string input(sInput);
	std::string output;
	bool wasLastASeparator = true;

	for (size_t i = 0; i < input.length(); ++i)
	{
		if ((input[i] >= 'a' && input[i] <= 'z') || (input[i] >= 'A' && input[i] <= 'Z') || (input[i] >= '0' && input[i] <= '9') || (
				!bRemoveSlash && input[i] == '/'))
		{
			if (input[i] == '/') { output += "_"; }
			else
			{
				if (wasLastASeparator)
				{
					if ('a' <= input[i] && input[i] <= 'z') { output += input[i] + 'A' - 'a'; }
					else { output += input[i]; }
				}
				else { output += input[i]; }
			}
			wasLastASeparator = false;
		}
		else { wasLastASeparator = true; }
	}
	return output;
}
