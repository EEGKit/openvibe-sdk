#include "ovkCLogListenerNull.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace std;

CLogListenerNull::CLogListenerNull(const IKernelContext& rKernelContext)
	: TKernelObject<ILogListener>(rKernelContext) {}

bool CLogListenerNull::isActive(ELogLevel eLogLevel)
{
	map<ELogLevel, bool>::iterator itLogLevel = m_vActiveLevel.find(eLogLevel);
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
	for (int i = eStartLogLevel; i <= eEndLogLevel; i++)
	{
		m_vActiveLevel[ELogLevel(i)] = bActive;
	}
	return true;
}

bool CLogListenerNull::activate(bool bActive) { return activate(LogLevel_First, LogLevel_Last, bActive); }

void CLogListenerNull::log(const time64 value) {}
void CLogListenerNull::log(const uint64_t value) {}
void CLogListenerNull::log(const uint32_t value) {}
void CLogListenerNull::log(const uint16_t value) {}
void CLogListenerNull::log(const uint8_t value) {}
void CLogListenerNull::log(const int64_t value) {}
void CLogListenerNull::log(const int32_t value) {}
void CLogListenerNull::log(const int16_t value) {}
void CLogListenerNull::log(const int8_t value) {}
void CLogListenerNull::log(const float value) {}
void CLogListenerNull::log(const double value) {}
void CLogListenerNull::log(const bool value) {}
void CLogListenerNull::log(const CIdentifier& value) {}
void CLogListenerNull::log(const CString& value) {}
void CLogListenerNull::log(const char* value) {}
void CLogListenerNull::log(const ELogLevel value) {}
void CLogListenerNull::log(const ELogColor value) {}
