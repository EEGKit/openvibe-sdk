#include "ovkCLogListenerFile.h"

#include <sstream>
#include <iostream>

#include <openvibe/ovTimeArithmetics.h>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace std;

CLogListenerFile::CLogListenerFile(const IKernelContext& ctx, const CString& applicationName, const CString& logFilename)
	: TKernelObject<ILogListener>(ctx), m_applicationName(applicationName), m_logFilename(logFilename)
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
	m_timeInSeconds = configurationManager.expandAsBoolean("${Kernel_FileLogTimeInSecond}", false);
	m_logWithHexa   = configurationManager.expandAsBoolean("${Kernel_FileLogWithHexa}", true);
	m_timePrecision = configurationManager.expandAsUInteger("${Kernel_FileLogTimePrecision}", 3);
}

bool CLogListenerFile::isActive(const ELogLevel level)
{
	const auto it = m_activeLevels.find(level);
	if (it == m_activeLevels.end()) { return true; }
	return it->second;
}

bool CLogListenerFile::activate(const ELogLevel level, const bool active)
{
	m_activeLevels[level] = active;
	return true;
}

bool CLogListenerFile::activate(const ELogLevel startLevel, const ELogLevel endLevel, const bool active)
{
	for (int i = startLevel; i <= endLevel; ++i) { m_activeLevels[ELogLevel(i)] = active; }
	return true;
}

bool CLogListenerFile::activate(const bool active) { return activate(LogLevel_First, LogLevel_Last, active); }

void CLogListenerFile::log(const time64 value)
{
	if (m_timeInSeconds)
	{
		const double time = TimeArithmetics::timeToSeconds(value.timeValue);
		std::stringstream ss;
		ss.precision(m_timePrecision);
		ss.setf(std::ios::fixed, std::ios::floatfield);
		ss << time;
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
