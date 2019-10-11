#pragma once

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>

#include <mutex>
#include <condition_variable>
#include <thread>

namespace OpenViBE
{
	namespace Kernel
	{
		class CLogManager final : public TKernelObject<ILogManager>
		{
		public:

			explicit CLogManager(const IKernelContext& ctx) : TKernelObject<ILogManager>(ctx) {}
			bool isActive(ELogLevel level) override;
			bool activate(ELogLevel level, bool active) override;
			bool activate(ELogLevel startLevel, ELogLevel endLevel, bool active) override;
			bool activate(bool active) override { return activate(LogLevel_First, LogLevel_Last, active); }
			void log(const time64 value) override { logForEach<const time64>(value); }
			void log(const uint64_t value) override { logForEach<const uint64_t>(value); }
			void log(const uint32_t value) override { logForEach<const uint32_t>(value); }
			void log(const int64_t value) override { logForEach<const int64_t>(value); }
			void log(const int value) override { logForEach<const int>(value); }
			void log(const double value) override { logForEach<const double>(value); }
			void log(const float value) override { logForEach<const float>(value); }
			void log(const bool value) override { logForEach<const bool>(value); }
			void log(const CIdentifier& value) override { logForEach<const CIdentifier&>(value); }
			void log(const CString& value) override { log(value.toASCIIString()); }
			void log(const std::string& value) override { log(value.c_str()); }
			void log(const char* value) override;
			void log(const ELogLevel level) override;
			void log(const ELogColor color) override { logForEach<ELogColor>(color); }
			bool addListener(ILogListener* listener) override;
			bool removeListener(ILogListener* listener) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogManager>, OVK_ClassId_Kernel_Log_LogManager)

		protected:

			// This macro waits until m_oHolder is either the current thread id or unassigned
#define GRAB_OWNERSHIP std::unique_lock<std::mutex> lock(m_oMutex); \
	m_oCondition.wait(lock, [this]() { return (this->m_oOwner == std::thread::id() || this->m_oOwner == std::this_thread::get_id() ); } ); \
	m_oOwner = std::this_thread::get_id();

			template <class T>
			void logForEach(T tValue)
			{
				GRAB_OWNERSHIP;

				if (m_eCurrentLogLevel != LogLevel_None && this->isActive(m_eCurrentLogLevel))
				{
					for (auto i = m_vListener.begin(); i != m_vListener.end(); ++i)
					{
						if ((*i)->isActive(m_eCurrentLogLevel)) { (*i)->log(tValue); }
					}
				}
			}

			std::vector<ILogListener*> m_vListener;
			std::map<ELogLevel, bool> m_vActiveLevel;
			ELogLevel m_eCurrentLogLevel = LogLevel_Info;

			// Variables to make sure only one thread writes to the LogManager at a time.
			// Concurrency control operating logic:
			//   Calling log() will wait until it obtains logmanager ownership for the calling thread
			//   Only thread having the ownership can write to the log
			//   After a thread logs an entry ending in an EOL character, the ownership is freed.
			std::mutex m_oMutex;
			std::condition_variable m_oCondition;
			std::thread::id m_oOwner;
		};
	} // namespace Kernel
} // namespace OpenViBE
