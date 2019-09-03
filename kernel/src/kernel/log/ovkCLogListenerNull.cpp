#include "ovkCLogListenerNull.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

bool CLogListenerNull::isActive(ELogLevel eLogLevel)
{
	const auto itLogLevel = m_vActiveLevel.find(eLogLevel);
	if (itLogLevel == m_vActiveLevel.end()) { return true; }
	return itLogLevel->second;
}

bool CLogListenerNull::activate(ELogLevel eLogLevel, bool bActive)
{
	m_vActiveLevel[eLogLevel] = bActive;
	return true;
}

bool CLogListenerNull::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive)
{
	for (size_t i = eStartLogLevel; i <= eEndLogLevel; ++i) { m_vActiveLevel[ELogLevel(i)] = bActive; }
	return true;
}
