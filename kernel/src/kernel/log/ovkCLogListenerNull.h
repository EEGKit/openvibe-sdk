#pragma once

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

			virtual void log(time64 value);

			virtual void log(uint64_t value);
			virtual void log(uint32_t value);
			virtual void log(uint16_t value);
			virtual void log(uint8_t value);

			virtual void log(int64_t value);
			virtual void log(int32_t value);
			virtual void log(int16_t value);
			virtual void log(int8_t value);

			virtual void log(double value);
			virtual void log(float value);

			virtual void log(bool value);

			virtual void log(const CIdentifier& value);
			virtual void log(const CString& value);
			virtual void log(const char* value);

			virtual void log(ELogLevel value);
			virtual void log(ELogColor value);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerNull);

		protected:

			std::map<ELogLevel, bool> m_vActiveLevel;
		};
	};
};


