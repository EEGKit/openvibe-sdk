#ifndef __OpenViBEKernel_Kernel_Log_CLogListenerNull_H__
#define __OpenViBEKernel_Kernel_Log_CLogListenerNull_H__

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogListenerNull : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>
		{
		public:

			explicit CLogListenerNull(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual bool isActive(OpenViBE::Kernel::ELogLevel eLogLevel);
			virtual bool activate(OpenViBE::Kernel::ELogLevel eLogLevel, bool bActive);
			virtual bool activate(OpenViBE::Kernel::ELogLevel eStartLogLevel, OpenViBE::Kernel::ELogLevel eEndLogLevel, bool bActive);
			virtual bool activate(bool bActive);

			virtual void log(const OpenViBE::time64 value);

			virtual void log(const OpenViBE::uint64 value);
			virtual void log(const OpenViBE::uint32 value);
			virtual void log(const OpenViBE::uint16 value);
			virtual void log(const OpenViBE::uint8 value);

			virtual void log(const OpenViBE::int64 value);
			virtual void log(const OpenViBE::int32 value);
			virtual void log(const OpenViBE::int16 value);
			virtual void log(const OpenViBE::int8 value);

			virtual void log(const double value);
			virtual void log(const float value);

			virtual void log(const bool value);

			virtual void log(const OpenViBE::CIdentifier& value);
			virtual void log(const OpenViBE::CString& value);
			virtual void log(const char* value);

			virtual void log(const OpenViBE::Kernel::ELogLevel value);
			virtual void log(const OpenViBE::Kernel::ELogColor value);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerNull);

		protected:

			std::map<OpenViBE::Kernel::ELogLevel, bool> m_vActiveLevel;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Log_CLogListenerNull_H__
