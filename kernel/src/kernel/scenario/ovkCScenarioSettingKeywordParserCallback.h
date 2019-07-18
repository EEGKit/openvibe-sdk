#ifndef __OpenViBEKernel_Kernel_Player_CScenarioSettingKeywordParserCallback_H__
#define __OpenViBEKernel_Kernel_Player_CScenarioSettingKeywordParserCallback_H__

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
	}
}

#endif // __OpenViBEKernel_Kernel_Player_CScenarioSettingKeywordParserCallback_H__
