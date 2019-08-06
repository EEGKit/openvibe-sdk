#include "ovkCLogManager.h"

#include <iostream>

using namespace OpenViBE;
using namespace Kernel;
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
	for (int i = eStartLogLevel; i <= eEndLogLevel; i++) { m_vActiveLevel[ELogLevel(i)] = bActive; }
	return true;
}

bool CLogManager::activate(bool bActive)
{
	return activate(LogLevel_First, LogLevel_Last, bActive);
}

void CLogManager::log(const time64 value) { logForEach<const time64>(value); }
void CLogManager::log(const uint64_t value) { logForEach<const uint64_t>(value); }
void CLogManager::log(const uint32_t value) { logForEach<const uint32_t>(value); }
void CLogManager::log(const uint16_t value) { logForEach<const uint16_t>(value); }
void CLogManager::log(const uint8_t value) { logForEach<const uint8_t>(value); }
void CLogManager::log(const int64_t value) { logForEach<const int64_t>(value); }
void CLogManager::log(const int32_t value) { logForEach<const int32_t>(value); }
void CLogManager::log(const int16_t value) { logForEach<const int16_t>(value); }
void CLogManager::log(const int8_t value) { logForEach<const int8_t>(value); }
void CLogManager::log(const double value) { logForEach<const double>(value); }
void CLogManager::log(const float value) { logForEach<const float>(value); }
void CLogManager::log(const bool value) { logForEach<const bool>(value); }
void CLogManager::log(const CIdentifier& value) { logForEach<const CIdentifier&>(value); }
void CLogManager::log(const CString& value) { log(value.toASCIIString()); }

void CLogManager::log(const char* value)
{
	logForEach<const char*>(value);

	{
		GRAB_OWNERSHIP;

		std::string l_sCopy(value);
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
