#pragma once

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

#include <map>

namespace
{
	template <class T>
	OpenViBE::CIdentifier getNextIdentifier(const std::map<OpenViBE::CIdentifier, T>& rMap, const OpenViBE::CIdentifier& previousID)
	{
		typename std::map<OpenViBE::CIdentifier, T>::const_iterator it;

		if (previousID == OV_UndefinedIdentifier) { it = rMap.begin(); }
		else
		{
			it = rMap.find(previousID);
			if (it == rMap.end()) { return OV_UndefinedIdentifier; }
			++it;
		}

		return it != rMap.end() ? it->first : OV_UndefinedIdentifier;
	}
} // namespace
