#include "ovkCLogManager.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace std;

bool CLogManager::isActive(const ELogLevel level)
{
	const auto it = m_activeLevels.find(level);
	if (it == m_activeLevels.end()) { return true; }
	return it->second;
}

bool CLogManager::activate(const ELogLevel level, const bool active)
{
	m_activeLevels[level] = active;
	return true;
}

bool CLogManager::activate(const ELogLevel startLevel, const ELogLevel endLevel, const bool active)
{
	for (int i = startLevel; i <= endLevel; ++i) { m_activeLevels[ELogLevel(i)] = active; }
	return true;
}

void CLogManager::log(const char* value)
{
	logForEach<const char*>(value);
	{
		GRAB_OWNERSHIP;

		std::string copy(value);
		if (copy.length() > 0 && copy[copy.length() - 1] == '\n')
		{
			// we are done, release
			m_owner = std::thread::id();
			m_condition.notify_one();
		}
	}
}

void CLogManager::log(const ELogLevel level)
{
	{
		GRAB_OWNERSHIP;
		m_currentLogLevel = level;
	}

	logForEach<ELogLevel>(level);
}

bool CLogManager::addListener(ILogListener* listener)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	if (listener == nullptr) { return false; }

	auto itLogListener = m_listeners.begin();
	while (itLogListener != m_listeners.end())
	{
		if ((*itLogListener) == listener) { return false; }
		++itLogListener;
	}

	m_listeners.push_back(listener);
	return true;
}

bool CLogManager::removeListener(ILogListener* listener)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	auto itLogListener = m_listeners.begin();
	while (itLogListener != m_listeners.end())
	{
		if ((*itLogListener) == listener)
		{
			m_listeners.erase(itLogListener);
			return true;	// due to constraint in addListener(), listener can be in the array only once, so we can return
		}
		++itLogListener;
	}
	return false;
}
