#pragma once

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenarioSettingKeywordParserCallback : public IConfigurationKeywordExpandCallback
		{
		public:
			explicit CScenarioSettingKeywordParserCallback(const IScenario& rScenario)
				: m_rScenario(rScenario) {}

			virtual ~CScenarioSettingKeywordParserCallback() {}

			virtual bool expand(const CString& rStringToExpand, CString& rExpandedString) const;

		private:
			const IScenario& m_rScenario;
		};
	}  // namespace Kernel
}  // namespace OpenViBE


