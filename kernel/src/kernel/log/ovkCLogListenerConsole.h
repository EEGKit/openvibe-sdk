#pragma once

#include "../ovkTKernelObject.h"

#include <stack>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerConsole final : public TKernelObject<ILogListener>
		{
		public:

			CLogListenerConsole(const IKernelContext& ctx, const CString& sApplicationName);
			bool isActive(ELogLevel level) override;
			bool activate(ELogLevel level, bool active) override;
			bool activate(ELogLevel startLevel, ELogLevel endLevel, bool active) override;
			bool activate(bool active) override;

			void configure(const IConfigurationManager& configManager);
			void log(const time64 value) override;
			void log(const uint64_t value) override;
			void log(const uint32_t value) override;
			void log(const int64_t value) override;
			void log(const int value) override;
			void log(const double value) override;
			void log(const bool value) override;
			void log(const CIdentifier& value) override;
			void log(const CString& value) override;
			void log(const std::string& value) override;
			void log(const char* value) override;
			void log(const ELogLevel level) override;
			void log(const ELogColor color) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerConsole)

		protected:

			void applyColor();

			std::map<ELogLevel, bool> m_activeLevels;
			std::stack<ELogColor> m_colors;
			ELogColor m_color;
			CString m_applicationName;

			bool m_logWithHexa       = false;
			bool m_timeInSeconds    = false;
			uint64_t m_timePrecision = 0;
			bool m_useColor         = false;
		};
	} // namespace Kernel
} // namespace OpenViBE
