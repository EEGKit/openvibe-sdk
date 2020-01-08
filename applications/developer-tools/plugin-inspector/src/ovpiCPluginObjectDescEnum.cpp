#include "ovpiCPluginObjectDescEnum.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;
using namespace std;

// Enumerate plugins by iterating over a user defined list of descriptors, used for metaboxes
bool CPluginObjectDescEnum::enumeratePluginObjectDesc(std::vector<const IPluginObjectDesc*>& pluginDescriptors)
{
	for (auto* plugin : pluginDescriptors) { this->callback(*plugin); }
	return true;
}

bool CPluginObjectDescEnum::enumeratePluginObjectDesc(const CIdentifier& parentClassIdentifier)
{
	CIdentifier id;
	while ((id = m_kernelCtx.getPluginManager().getNextPluginObjectDescIdentifier(id, parentClassIdentifier)) != OV_UndefinedIdentifier)
	{
		this->callback(*m_kernelCtx.getPluginManager().getPluginObjectDesc(id));
	}
	return true;
}

std::string CPluginObjectDescEnum::transform(const std::string& in, const bool removeSlash)
{
	std::string out;
	bool wasLastASeparator = true;

	for (size_t i = 0; i < in.length(); ++i)
	{
		if ((in[i] >= 'a' && in[i] <= 'z') || (in[i] >= 'A' && in[i] <= 'Z') || (in[i] >= '0' && in[i] <= '9') || (!removeSlash && in[i] == '/'))
		{
			if (in[i] == '/') { out += "_"; }
			else
			{
				if (wasLastASeparator)
				{
					if ('a' <= in[i] && in[i] <= 'z') { out += in[i] + 'A' - 'a'; }
					else { out += in[i]; }
				}
				else { out += in[i]; }
			}
			wasLastASeparator = false;
		}
		else { wasLastASeparator = true; }
	}
	return out;
}
