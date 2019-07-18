#ifndef __OpenViBEKernel_Kernel_Log_CLogListenerConsole_H__
#define __OpenViBEKernel_Kernel_Log_CLogListenerConsole_H__

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

			virtual void log(const time64 time64Value);

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

			virtual void log(const CIdentifier& rValue);
			virtual void log(const CString& rValue);
			virtual void log(const char* pValue);

			virtual void log(const ELogLevel eLogLevel);
			virtual void log(const ELogColor eLogColor);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerConsole);

		protected:

			virtual void applyColor(void);

		protected:

			std::map<ELogLevel, bool> m_vActiveLevel;
			std::stack<ELogColor> m_vLogColor;
			ELogColor m_eLogColor;
			CString m_sApplicationName;

			bool m_bLogWithHexa;
			bool m_bTimeInSeconds;
			uint64_t m_ui64TimePrecision;
			bool m_bUseColor;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Log_CLogListenerConsole_H__
