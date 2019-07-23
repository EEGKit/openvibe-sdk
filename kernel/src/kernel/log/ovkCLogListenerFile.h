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

			virtual void log(time64 time64Value);

			virtual void log(uint64_t ui64Value);
			virtual void log(uint32_t ui32Value);
			virtual void log(uint16_t ui16Value);
			virtual void log(uint8_t ui8Value);

			virtual void log(int64_t i64Value);
			virtual void log(int32_t i32Value);
			virtual void log(int16_t i16Value);
			virtual void log(int8_t i8Value);

			virtual void log(double f64Value);
			virtual void log(float f32Value);

			virtual void log(bool bValue);

			virtual void log(const CIdentifier& rValue);
			virtual void log(const CString& rValue);
			virtual void log(const char* pValue);

			virtual void log(ELogLevel eLogLevel);
			virtual void log(ELogColor eLogColor);

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


