#include "ovkCScenarioSettingKeywordParserCallback.h"
#include <openvibe/ov_all.h>

using namespace OpenViBE;
using namespace Kernel;

#include <iostream>

bool CScenarioSettingKeywordParserCallback::expand(const CString& rStringToExpand, CString& rExpandedString) const
{
	// In the case there is no value present we return an empty string
	rExpandedString = "";

	// Expand the scenario directory
	if (rStringToExpand == CString("ScenarioDirectory"))
	{
		if (m_rScenario.hasAttribute(OV_AttributeId_ScenarioFilename))
		{
			std::string l_sFilename = m_rScenario.getAttributeValue(OV_AttributeId_ScenarioFilename).toASCIIString();

			size_t iDir = l_sFilename.rfind("/");
			if (iDir != std::string::npos)
			{
				rExpandedString = CString(l_sFilename.substr(0, iDir).c_str());
			}
		}
		return true;
	}
		// Expand settings from the scenario
	if (m_rScenario.hasSettingWithName(rStringToExpand))
	{
		m_rScenario.getSettingValue(rStringToExpand, rExpandedString);
		return true;
	}

	return false;
}
