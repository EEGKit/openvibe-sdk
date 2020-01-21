#pragma once

#include "../ovkTKernelObject.h"

#include <sstream>
#include <iostream>
#include <map>
#include <fstream>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerFile final : public TKernelObject<ILogListener>
		{
		public:

			CLogListenerFile(const IKernelContext& ctx, const CString& applicationName, const CString& logFilename);
			~CLogListenerFile() override { m_fsFileStream.close(); }
			bool isActive(ELogLevel logLevel) override;
			bool activate(ELogLevel level, bool active) override;
			bool activate(ELogLevel startLevel, ELogLevel endLevel, bool active) override;
			bool activate(bool active) override;

			void configure(const IConfigurationManager& configurationManager);
			void log(const time64 value) override;
			void log(const uint64_t value) override { logInteger(value); }
			void log(const uint32_t value) override { logInteger(value); }
			void log(const int64_t value) override { logInteger(value); }
			void log(const int value) override { logInteger(value); }
			void log(const double value) override { m_fsFileStream << value; }
			void log(const bool value) override { m_fsFileStream << (value ? "true" : "false"); }
			void log(const CIdentifier& value) override { m_fsFileStream << value.str(); }
			void log(const CString& value) override { m_fsFileStream << value << std::flush; }
			void log(const std::string& value) override { m_fsFileStream << value << std::flush; }
			void log(const char* value) override { m_fsFileStream << value << std::flush; }
			void log(const ELogLevel level) override;
			void log(const ELogColor /*color*/) override { }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerFile)

		protected:

			std::map<ELogLevel, bool> m_activeLevels;
			CString m_applicationName;
			CString m_logFilename;
			std::fstream m_fsFileStream;

			// Log Settings
			bool m_timeInSeconds    = true;
			bool m_logWithHexa       = false;
			uint64_t m_timePrecision = 3;

		private:
			template <typename T>
			void logInteger(T value)
			{
				m_fsFileStream << value << " ";
				if (m_logWithHexa)
				{
					m_fsFileStream.setf(std::ios::hex);
					m_fsFileStream << "(" << value << ")";
					m_fsFileStream.unsetf(std::ios::hex);
				}
			}
		};
	} // namespace Kernel
} // namespace OpenViBE
