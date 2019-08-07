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
		class CLogManager : public TKernelObject<ILogManager>
		{
		public:

			explicit CLogManager(const IKernelContext& rKernelContext);

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

			virtual void log(const ELogLevel eLogLevel);
			virtual void log(const ELogColor eLogColor);

			virtual bool addListener(ILogListener* pListener);
			virtual bool removeListener(ILogListener* pListener);

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
					for (std::vector<ILogListener*>::iterator i = m_vListener.begin(); i != m_vListener.end(); ++i)
					{
						if ((*i)->isActive(m_eCurrentLogLevel))
						{
							(*i)->log(tValue);
						}
					}
				}
			}

			std::vector<ILogListener*> m_vListener;
			std::map<ELogLevel, bool> m_vActiveLevel;
			ELogLevel m_eCurrentLogLevel;

			// Variables to make sure only one thread writes to the LogManager at a time.
			// Concurrency control operating logic:
			//   Calling log() will wait until it obtains logmanager ownership for the calling thread
			//   Only thread having the ownership can write to the log
			//   After a thread logs an entry ending in an EOL character, the ownership is freed.
			std::mutex m_oMutex;
			std::condition_variable m_oCondition;
			std::thread::id m_oOwner;
		};
	}  // namespace Kernel
}  // namespace OpenViBE
