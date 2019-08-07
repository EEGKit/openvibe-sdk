#pragma once

#include "../ovkTKernelObject.h"

#include <stack>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerConsole : public TKernelObject<ILogListener>
		{
		public:

			CLogListenerConsole(const IKernelContext& rKernelContext, const CString& sApplicationName);

			virtual bool isActive(ELogLevel eLogLevel);
			virtual bool activate(ELogLevel eLogLevel, bool bActive);
			virtual bool activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive);
			virtual bool activate(bool bActive);

			void configure(const IConfigurationManager& rConfigurationManager);

			virtual void log(const time64 value);

			virtual void log(const uint64_t value);
			virtual void log(const uint32_t value);
			virtual void log(const uint16_t value);
			virtual void log(const uint8_t value);

			virtual void log(const int64_t value);
			virtual void log(const int32_t value);
			virtual void log(const int16_t value);
			virtual void log(const int8_t value);

			virtual void log(const double value);
			virtual void log(const float value);

			virtual void log(const bool value);

			virtual void log(const CIdentifier& value);
			virtual void log(const CString& value);
			virtual void log(const char* value);

			virtual void log(const ELogLevel eLogLevel);
			virtual void log(const ELogColor eLogColor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerConsole)

		protected:

			virtual void applyColor();

			std::map<ELogLevel, bool> m_vActiveLevel;
			std::stack<ELogColor> m_vLogColor;
			ELogColor m_eLogColor;
			CString m_sApplicationName;

			bool m_bLogWithHexa;
			bool m_bTimeInSeconds;
			uint64_t m_ui64TimePrecision;
			bool m_bUseColor;
		};
	}  // namespace Kernel
}  // namespace OpenViBE


