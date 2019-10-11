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
			bool activate(ELogLevel level, bool bActive) override;
			bool activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive) override;
			bool activate(bool bActive) override;

			void configure(const IConfigurationManager& rConfigurationManager);
			void log(const time64 value) override;
			void log(const uint64_t value) override;
			void log(const uint32_t value) override;
			void log(const uint16_t value) override;
			void log(const uint8_t value) override;
			void log(const int64_t value) override;
			void log(const int value) override;
			void log(const int16_t value) override;
			void log(const int8_t value) override;
			void log(const double value) override;
			void log(const float value) override;
			void log(const bool value) override;
			void log(const CIdentifier& value) override;
			void log(const CString& value) override;
			void log(const char* value) override;
			void log(const ELogLevel level) override;
			void log(const ELogColor color) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerConsole)

		protected:

			void applyColor();

			std::map<ELogLevel, bool> m_vActiveLevel;
			std::stack<ELogColor> m_vLogColor;
			ELogColor m_eLogColor;
			CString m_sApplicationName;

			bool m_bLogWithHexa          = false;
			bool m_bTimeInSeconds        = false;
			uint64_t m_timePrecision = 0;
			bool m_bUseColor             = false;
		};
	} // namespace Kernel
} // namespace OpenViBE
