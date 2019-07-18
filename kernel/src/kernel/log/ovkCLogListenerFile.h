#ifndef __OpenViBEKernel_Kernel_Log_CLogListenerFile_H__
#define __OpenViBEKernel_Kernel_Log_CLogListenerFile_H__

#include "../ovkTKernelObject.h"

#include <map>
#include <fstream>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerFile : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>
		{
		public:

			CLogListenerFile(const OpenViBE::Kernel::IKernelContext& rKernelContext, const CString& sApplicationName, const OpenViBE::CString& sLogFilename);
			~CLogListenerFile();

			virtual bool isActive(OpenViBE::Kernel::ELogLevel eLogLevel);
			virtual bool activate(OpenViBE::Kernel::ELogLevel eLogLevel, bool bActive);
			virtual bool activate(OpenViBE::Kernel::ELogLevel eStartLogLevel, OpenViBE::Kernel::ELogLevel eEndLogLevel, bool bActive);
			virtual bool activate(bool bActive);

			void configure(const OpenViBE::Kernel::IConfigurationManager& rConfigurationManager);

			virtual void log(const OpenViBE::time64 time64Value);

			virtual void log(const uint64_t ui64Value);
			virtual void log(const uint32_t ui32Value);
			virtual void log(const uint16_t ui16Value);
			virtual void log(const uint8_t ui8Value);

			virtual void log(const int64_t i64Value);
			virtual void log(const int32_t i32Value);
			virtual void log(const int16_t i16Value);
			virtual void log(const int8_t i8Value);

			virtual void log(const double f64Value);
			virtual void log(const float f32Value);

			virtual void log(const bool bValue);

			virtual void log(const OpenViBE::CIdentifier& rValue);
			virtual void log(const OpenViBE::CString& rValue);
			virtual void log(const char* pValue);

			virtual void log(const OpenViBE::Kernel::ELogLevel eLogLevel);
			virtual void log(const OpenViBE::Kernel::ELogColor eLogColor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerFile);

		protected:

			std::map<OpenViBE::Kernel::ELogLevel, bool> m_vActiveLevel;
			OpenViBE::CString m_sApplicationName;
			OpenViBE::CString m_sLogFilename;
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
	};
};

#endif // __OpenViBEKernel_Kernel_Log_CLogListenerFile_H__
