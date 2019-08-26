#include "ovkCLogListenerFile.h"

#include <cstdio>
#include <sstream>
#include <iostream>

#include <openvibe/ovITimeArithmetics.h>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

CLogListenerFile::CLogListenerFile(const IKernelContext& rKernelContext, const CString& sApplicationName, const CString& sLogFilename)
	: TKernelObject<ILogListener>(rKernelContext)
	  , m_sApplicationName(sApplicationName)
	  , m_sLogFilename(sLogFilename)
	  , m_bTimeInSeconds(true), m_ui64TimePrecision(3)
{

	// Create the path to the log file
	FS::Files::createParentPath(sLogFilename.toASCIIString());

	FS::Files::openFStream(m_fsFileStream, sLogFilename.toASCIIString(), ios_base::out);

	if (!m_fsFileStream.is_open())
	{
		std::cout << "[  ERR  ] Error while creating FileLogListener into '" << sLogFilename << "'" << std::endl;
		return;
	}
	m_fsFileStream << flush;
}

CLogListenerFile::~CLogListenerFile() { m_fsFileStream.close(); }

void CLogListenerFile::configure(const IConfigurationManager& rConfigurationManager)
{
	m_bTimeInSeconds    = rConfigurationManager.expandAsBoolean("${Kernel_FileLogTimeInSecond}", false);
	m_bLogWithHexa      = rConfigurationManager.expandAsBoolean("${Kernel_FileLogWithHexa}", true);
	m_ui64TimePrecision = rConfigurationManager.expandAsUInteger("${Kernel_FileLogTimePrecision}", 3);
}

bool CLogListenerFile::isActive(ELogLevel eLogLevel)
{
	map<ELogLevel, bool>::iterator itLogLevel = m_vActiveLevel.find(eLogLevel);
	if (itLogLevel == m_vActiveLevel.end()) { return true; }
	return itLogLevel->second;
}

bool CLogListenerFile::activate(ELogLevel eLogLevel, bool bActive)
{
	m_vActiveLevel[eLogLevel] = bActive;
	return true;
}

bool CLogListenerFile::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive)
{
	for (int i = eStartLogLevel; i <= eEndLogLevel; i++) { m_vActiveLevel[ELogLevel(i)] = bActive; }
	return true;
}

bool CLogListenerFile::activate(bool bActive)
{
	return activate(LogLevel_First, LogLevel_Last, bActive);
}

void CLogListenerFile::log(const time64 time64Value)
{
	if (m_bTimeInSeconds)
	{
		double l_f64Time = ITimeArithmetics::timeToSeconds(time64Value.m_ui64TimeValue);
		std::stringstream ss;
		ss.precision(m_ui64TimePrecision);
		ss.setf(std::ios::fixed, std::ios::floatfield);
		ss << l_f64Time;
		ss << " sec";

		if (m_bLogWithHexa)
		{
			ss << " (0x" << hex << time64Value.m_ui64TimeValue << ")";
		}

		m_fsFileStream << ss.str();
	}
	else { logInteger(time64Value.m_ui64TimeValue); }
}

void CLogListenerFile::log(const uint64_t value) { logInteger(value); }
void CLogListenerFile::log(const uint32_t value) { logInteger(value); }
void CLogListenerFile::log(const uint16_t value) { logInteger(value); }
void CLogListenerFile::log(const uint8_t value) { logInteger(value); }
void CLogListenerFile::log(const int64_t value) { logInteger(value); }
void CLogListenerFile::log(const int value) { logInteger(value); }
void CLogListenerFile::log(const int16_t value) { logInteger(value); }
void CLogListenerFile::log(const int8_t value) { logInteger(value); }

void CLogListenerFile::log(const float value) { m_fsFileStream << value; }
void CLogListenerFile::log(const double value) { m_fsFileStream << value; }
void CLogListenerFile::log(const bool value) { m_fsFileStream << (value ? "true" : "false"); }
void CLogListenerFile::log(const CIdentifier& value) { m_fsFileStream << value.toString(); }

void CLogListenerFile::log(const CString& value)
{
	m_fsFileStream << value;
	m_fsFileStream << flush;
}

void CLogListenerFile::log(const char* value)
{
	m_fsFileStream << value;
	m_fsFileStream << flush;
}

void CLogListenerFile::log(const ELogLevel eLogLevel)
{
	switch (eLogLevel)
	{
		case LogLevel_Debug:
			m_fsFileStream << "[ DEBUG ] ";
			break;

		case LogLevel_Benchmark:
			m_fsFileStream << "[ BENCH ] ";
			break;

		case LogLevel_Trace:
			m_fsFileStream << "[ TRACE ] ";
			break;

		case LogLevel_Info:
			m_fsFileStream << "[  INF  ] ";
			break;

		case LogLevel_Warning:
			m_fsFileStream << "[WARNING] ";
			break;

		case LogLevel_ImportantWarning:
			m_fsFileStream << "[WARNING] ";
			break;

		case LogLevel_Error:
			m_fsFileStream << "[ ERROR ] ";
			break;

		case LogLevel_Fatal:
			m_fsFileStream << "[ FATAL ] ";
			break;

		default:
			m_fsFileStream << "[UNKNOWN] ";
			break;
	}
}

void CLogListenerFile::log(const ELogColor eLogColor) {}
