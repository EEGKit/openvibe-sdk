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

			virtual bool addListener(ILogListener* pListener);
			virtual bool removeListener(ILogListener* pListener);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::ILogManager>, OVK_ClassId_Kernel_Log_LogManager);

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
					std::vector<ILogListener*>::iterator i;
					for (i = m_vListener.begin(); i != m_vListener.end(); ++i)
					{
						if ((*i)->isActive(m_eCurrentLogLevel))
						{
							(*i)->log(tValue);
						}
					}
				}
			}

		protected:

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
	};
};
