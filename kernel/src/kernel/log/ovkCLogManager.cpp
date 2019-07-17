#include "ovkCLogManager.h"

#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

CLogManager::CLogManager(const IKernelContext& rKernelContext)
	: TKernelObject<ILogManager>(rKernelContext)
	  , m_eCurrentLogLevel(LogLevel_Info) {}

bool CLogManager::isActive(ELogLevel eLogLevel)
{
	map<ELogLevel, bool>::iterator itLogLevel = m_vActiveLevel.find(eLogLevel);
	if (itLogLevel == m_vActiveLevel.end()) { return true; }
	return itLogLevel->second;
}

bool CLogManager::activate(ELogLevel eLogLevel, bool bActive)
{
	m_vActiveLevel[eLogLevel] = bActive;
	return true;
}

bool CLogManager::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive)
{
	for (int i = eStartLogLevel; i <= eEndLogLevel; i++)
	{
		m_vActiveLevel[ELogLevel(i)] = bActive;
	}
	return true;
}

bool CLogManager::activate(bool bActive)
{
	return activate(LogLevel_First, LogLevel_Last, bActive);
}

void CLogManager::log(const time64 time64Value) { logForEach<const time64>(time64Value); }
void CLogManager::log(const uint64 ui64Value) { logForEach<const uint64>(ui64Value); }
void CLogManager::log(const uint32 ui32Value) { logForEach<const uint32>(ui32Value); }
void CLogManager::log(const uint16 ui16Value) { logForEach<const uint16>(ui16Value); }
void CLogManager::log(const uint8 ui8Value) { logForEach<const uint8>(ui8Value); }
void CLogManager::log(const int64 i64Value) { logForEach<const int64>(i64Value); }
void CLogManager::log(const int32 i32Value) { logForEach<const int32>(i32Value); }
void CLogManager::log(const int16 i16Value) { logForEach<const int16>(i16Value); }
void CLogManager::log(const int8 i8Value) { logForEach<const int8>(i8Value); }
void CLogManager::log(const double f64Value) { logForEach<const double>(f64Value); }
void CLogManager::log(const float f32Value) { logForEach<const float>(f32Value); }
void CLogManager::log(const bool bValue) { logForEach<const bool>(bValue); }
void CLogManager::log(const CIdentifier& rValue) { logForEach<const CIdentifier&>(rValue); }
void CLogManager::log(const CString& rValue) { log(rValue.toASCIIString()); }

void CLogManager::log(const char* rValue)
{
	logForEach<const char*>(rValue);

	{
		GRAB_OWNERSHIP;

		std::string l_sCopy(rValue);
		if (l_sCopy.length() > 0 && l_sCopy[l_sCopy.length() - 1] == '\n')
		{
			// we are done, release
			m_oOwner = std::thread::id();
			m_oCondition.notify_one();
		}
	}
}

void CLogManager::log(const ELogLevel eLogLevel)
{
	{
		GRAB_OWNERSHIP;

		m_eCurrentLogLevel = eLogLevel;
	}

	logForEach<ELogLevel>(eLogLevel);
}

void CLogManager::log(const ELogColor eLogColor)
{
	logForEach<ELogColor>(eLogColor);
}

bool CLogManager::addListener(ILogListener* pListener)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	if (pListener == NULL) { return false; }

	vector<ILogListener*>::iterator itLogListener = m_vListener.begin();
	while (itLogListener != m_vListener.end())
	{
		if ((*itLogListener) == pListener) { return false; }
		++itLogListener;
	}

	m_vListener.push_back(pListener);
	return true;
}

bool CLogManager::removeListener(ILogListener* pListener)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	vector<ILogListener*>::iterator itLogListener = m_vListener.begin();
	while (itLogListener != m_vListener.end())
	{
		if ((*itLogListener) == pListener)
		{
			m_vListener.erase(itLogListener);
			return true;	// due to constraint in addListener(), pListener can be in the array only once, so we can return
		}
		++itLogListener;
	}
	return false;
}
