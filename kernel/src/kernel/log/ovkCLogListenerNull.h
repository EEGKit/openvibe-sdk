#ifndef __OpenViBEKernel_Kernel_Log_CLogListenerNull_H__
#define __OpenViBEKernel_Kernel_Log_CLogListenerNull_H__

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerNull : public TKernelObject<ILogListener>
		{
		public:

			explicit CLogListenerNull(const IKernelContext& rKernelContext);

			virtual bool isActive(ELogLevel eLogLevel);
			virtual bool activate(ELogLevel eLogLevel, bool bActive);
			virtual bool activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive);
			virtual bool activate(bool bActive);

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

			virtual void log(const ELogLevel value);
			virtual void log(const ELogColor value);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerNull);

		protected:

			std::map<ELogLevel, bool> m_vActiveLevel;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Log_CLogListenerNull_H__
