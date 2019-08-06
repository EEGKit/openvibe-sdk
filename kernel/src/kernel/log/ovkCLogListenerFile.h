#pragma once

#include "../ovkTKernelObject.h"

#include <map>
#include <fstream>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerFile : public TKernelObject<ILogListener>
		{
		public:

			CLogListenerFile(const IKernelContext& rKernelContext, const CString& sApplicationName, const CString& sLogFilename);
			~CLogListenerFile();

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

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerFile)

		protected:

			std::map<ELogLevel, bool> m_vActiveLevel;
			CString m_sApplicationName;
			CString m_sLogFilename;
			std::fstream m_fsFileStream;

			// Log Settings
			bool m_bTimeInSeconds;
			bool m_bLogWithHexa;
			uint64_t m_ui64TimePrecision;

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
	}  // namespace Kernel
}  // namespace OpenViBE


