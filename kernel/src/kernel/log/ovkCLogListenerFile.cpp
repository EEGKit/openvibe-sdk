#include "ovkCLogListenerFile.h"

#include <sstream>
#include <iostream>

#include <openvibe/ovTimeArithmetics.h>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

CLogListenerFile::CLogListenerFile(const IKernelContext& ctx, const CString& applicationName, const CString& logFilename)
	: TKernelObject<ILogListener>(ctx), m_sApplicationName(applicationName), m_sLogFilename(logFilename)
{

	// Create the path to the log file
	FS::Files::createParentPath(logFilename.toASCIIString());

	FS::Files::openFStream(m_fsFileStream, logFilename.toASCIIString(), ios_base::out);

	if (!m_fsFileStream.is_open())
	{
		std::cout << "[  ERR  ] Error while creating FileLogListener into '" << logFilename << "'" << std::endl;
		return;
	}
	m_fsFileStream << flush;
}

void CLogListenerFile::configure(const IConfigurationManager& configurationManager)
{
	m_bTimeInSeconds    = configurationManager.expandAsBoolean("${Kernel_FileLogTimeInSecond}", false);
	m_logWithHexa      = configurationManager.expandAsBoolean("${Kernel_FileLogWithHexa}", true);
	m_timePrecision = configurationManager.expandAsUInteger("${Kernel_FileLogTimePrecision}", 3);
}

bool CLogListenerFile::isActive(ELogLevel logLevel)
{
	const auto it = m_vActiveLevel.find(logLevel);
	if (it == m_vActiveLevel.end()) { return true; }
	return it->second;
}

bool CLogListenerFile::activate(ELogLevel level, bool active)
{
	m_vActiveLevel[level] = active;
	return true;
}

bool CLogListenerFile::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool active)
{
	for (int i = eStartLogLevel; i <= eEndLogLevel; ++i) { m_vActiveLevel[ELogLevel(i)] = active; }
	return true;
}

bool CLogListenerFile::activate(bool active) { return activate(LogLevel_First, LogLevel_Last, active); }

void CLogListenerFile::log(const time64 value)
{
	if (m_bTimeInSeconds)
	{
		double l_f64Time = TimeArithmetics::timeToSeconds(value.timeValue);
		std::stringstream ss;
		ss.precision(m_timePrecision);
		ss.setf(std::ios::fixed, std::ios::floatfield);
		ss << l_f64Time;
		ss << " sec";

		if (m_logWithHexa) { ss << " (0x" << hex << value.timeValue << ")"; }

		m_fsFileStream << ss.str();
	}
	else { logInteger(value.timeValue); }
}

void CLogListenerFile::log(const ELogLevel level)
{
	switch (level)
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
