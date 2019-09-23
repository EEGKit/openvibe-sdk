#pragma once

#include "../ovkTKernelObject.h"

#include <map>
#include <fstream>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerFile final : public TKernelObject<ILogListener>
		{
		public:

			CLogListenerFile(const IKernelContext& ctx, const CString& sApplicationName, const CString& sLogFilename);
			~CLogListenerFile() override;
			bool isActive(ELogLevel eLogLevel) override;
			bool activate(ELogLevel eLogLevel, bool bActive) override;
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
			void log(const ELogColor /*color*/) override { }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerFile)

		protected:

			std::map<ELogLevel, bool> m_vActiveLevel;
			CString m_sApplicationName;
			CString m_sLogFilename;
			std::fstream m_fsFileStream;

			// Log Settings
			bool m_bTimeInSeconds        = false;
			bool m_bLogWithHexa          = false;
			uint64_t m_ui64TimePrecision = 0;

		private:
			template <typename T>
			void logInteger(T value)
			{
				m_fsFileStream << value << " ";
				if (m_bLogWithHexa)
				{
					m_fsFileStream.setf(std::ios::hex);
					m_fsFileStream << "(" << value << ")";
					m_fsFileStream.unsetf(std::ios::hex);
				}
			}
		};
	} // namespace Kernel
} // namespace OpenViBE
