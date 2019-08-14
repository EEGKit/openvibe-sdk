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
			bool isActive(ELogLevel eLogLevel) override;
			bool activate(ELogLevel eLogLevel, bool bActive) override;
			bool activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive) override;
			bool activate(bool bActive) override;
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
			void log(const ELogLevel value) override;
			void log(const ELogColor value) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogListener>, OVK_ClassId_Kernel_Log_LogListenerNull)

		protected:

			std::map<ELogLevel, bool> m_vActiveLevel;
		};
	}  // namespace Kernel
}  // namespace OpenViBE


