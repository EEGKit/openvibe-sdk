#include "ovkCLogListenerConsole.h"

#include <iostream>
#include <sstream>

#if defined TARGET_OS_Windows
#include <Windows.h>
#endif

#include <openvibe/ovTimeArithmetics.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace std;

CLogListenerConsole::CLogListenerConsole(const IKernelContext& ctx, const CString& sApplicationName)
	: TKernelObject<ILogListener>(ctx), m_color(LogColor_Default), m_applicationName(sApplicationName), m_timeInSeconds(true), m_timePrecision(3), m_useColor(true)
{
#if defined TARGET_OS_Windows
	SetConsoleOutputCP(CP_UTF8);
#endif
}

void CLogListenerConsole::configure(const IConfigurationManager& configManager)
{
	m_timeInSeconds = configManager.expandAsBoolean("${Kernel_ConsoleLogTimeInSecond}", true);
	m_logWithHexa    = configManager.expandAsBoolean("${Kernel_ConsoleLogWithHexa}", false);
	m_timePrecision  = configManager.expandAsUInteger("${Kernel_ConsoleLogTimePrecision}", 3);
	m_useColor      = configManager.expandAsBoolean("${Kernel_ConsoleLogUseColor}", true);
}


bool CLogListenerConsole::isActive(ELogLevel level)
{
	const auto itLogLevel = m_activeLevels.find(level);
	if (itLogLevel == m_activeLevels.end()) { return true; }
	return itLogLevel->second;
}

bool CLogListenerConsole::activate(ELogLevel level, bool active)
{
	m_activeLevels[level] = active;
	return true;
}

bool CLogListenerConsole::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool active)
{
	for (int i = eStartLogLevel; i <= eEndLogLevel; ++i) { m_activeLevels[ELogLevel(i)] = active; }
	return true;
}

bool CLogListenerConsole::activate(bool active) { return activate(LogLevel_First, LogLevel_Last, active); }

void CLogListenerConsole::log(const time64 value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	const ios_base::fmtflags fmt = cout.flags();

	if (m_timeInSeconds)
	{
		const uint64_t precision = m_timePrecision;
		const double time        = TimeArithmetics::timeToSeconds(value.timeValue);
		std::stringstream ss;
		ss.precision(static_cast<long long>(precision));
		ss.setf(std::ios::fixed, std::ios::floatfield);
		ss << time;
		ss << " sec";
		if (m_logWithHexa) { ss << " (0x" << hex << value.timeValue << ")"; }

		cout << ss.str();
	}
	else
	{
		cout << dec << value.timeValue;
		if (m_logWithHexa) { cout << " (0x" << hex << value.timeValue << ")"; }
	}

	cout.flags(fmt);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint64_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	const ios_base::fmtflags fmt = cout.flags();
	cout << dec << value;
	if (m_logWithHexa) { cout << " (0x" << hex << value << ")"; }

	cout.flags(fmt);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint32_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	const ios_base::fmtflags fmt = cout.flags();
	cout << dec << value;
	if (m_logWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(fmt);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int64_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	const ios_base::fmtflags fmt = cout.flags();
	cout << dec << value;
	if (m_logWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(fmt);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	const ios_base::fmtflags fmt = cout.flags();
	cout << dec << value;
	if (m_logWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(fmt);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const double value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	cout << value;
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const bool value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	cout << (value ? "true" : "false");
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const CIdentifier& value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	cout << value.toString();
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const CString& value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	cout << value;
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const std::string& value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	cout << value;
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const char* value) { cout << value << flush; }

void CLogListenerConsole::log(const ELogLevel level)
{
	switch (level)
	{
		case LogLevel_Debug:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundBlue);
			cout << " DEBUG ";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Benchmark:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundMagenta);
			cout << " BENCH ";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Trace:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundYellow);
			cout << " TRACE ";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Info:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundGreen);
			cout << "  INF  ";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Warning:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundCyan);
			cout << "WARNING";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_ImportantWarning:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundRed);
			cout << "WARNING";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Error:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundRed);
			cout << " ERROR ";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		case LogLevel_Fatal:
			cout << "[";
			this->log(LogColor_PushStateBit);
			this->log(LogColor_ForegroundRed);
			cout << " FATAL ";
			this->log(LogColor_PopStateBit);
			cout << "]";
			break;

		default:
			cout << "[UNKNOWN]";
			break;
	}
	cout << " ";
}

void CLogListenerConsole::log(const ELogColor color)
{
	if (m_useColor)
	{
		// Tests 'push state' bit
		if (color & LogColor_PushStateBit) { m_colors.push(m_color); }

		// Tests 'pop state' bit
		if (color & LogColor_PopStateBit)
		{
			if (!m_colors.empty())
			{
				m_color = m_colors.top();
				m_colors.pop();
			}
			else { m_color = LogColor_Default; }
		}

		// Tests 'reset' bit
		if (color & LogColor_ResetBit) { m_color = LogColor_Default; }

		// Tests 'foreground' bit
		if (color & LogColor_ForegroundBit)
		{
			// Tests 'color' bit
			if (color & LogColor_ForegroundColorBit)
			{
				const ELogColor colorMask = ELogColor(LogColor_ForegroundColorRedBit | LogColor_ForegroundColorGreenBit | LogColor_ForegroundColorBlueBit);
				m_color               = ELogColor(m_color & (~colorMask));
				m_color               = ELogColor(m_color | (color & colorMask) | LogColor_ForegroundBit | LogColor_ForegroundColorBit);
			}

			// Test 'light' bit
			if (color & LogColor_ForegroundLightBit)
			{
				const ELogColor lightMask = ELogColor(LogColor_ForegroundLightBit | LogColor_ForegroundLightStateBit);
				m_color               = ELogColor(m_color & (~lightMask));
				m_color               = ELogColor(m_color | (color & lightMask) | LogColor_ForegroundBit);
			}

			// Test 'blink' bit
			if (color & LogColor_ForegroundBlinkBit)
			{
				const ELogColor blinkMask = ELogColor(LogColor_ForegroundBlinkBit | LogColor_ForegroundBlinkStateBit);
				m_color               = ELogColor(m_color & (~blinkMask));
				m_color               = ELogColor(m_color | (color & blinkMask) | LogColor_ForegroundBit);
			}

			// Test 'bold' bit
			if (color & LogColor_ForegroundBoldBit)
			{
				const ELogColor boldMask = ELogColor(LogColor_ForegroundBoldBit | LogColor_ForegroundBoldStateBit);
				m_color              = ELogColor(m_color & (~boldMask));
				m_color              = ELogColor(m_color | (color & boldMask) | LogColor_ForegroundBit);
			}

			// Test 'underline' bit
			if (color & LogColor_ForegroundUnderlineBit)
			{
				const ELogColor underlineMask = ELogColor(LogColor_ForegroundBlinkBit | LogColor_ForegroundBlinkStateBit);
				m_color                   = ELogColor(m_color & (~underlineMask));
				m_color                   = ELogColor(m_color | (color & underlineMask) | LogColor_ForegroundBit);
			}
		}

		// Tests 'background' bit
		if (color & LogColor_BackgroundBit)
		{
			// Tests 'color' bit
			if (color & LogColor_BackgroundColorBit)
			{
				const ELogColor colorMask = ELogColor(LogColor_BackgroundColorRedBit | LogColor_BackgroundColorGreenBit | LogColor_BackgroundColorBlueBit);
				m_color               = ELogColor(m_color & (~colorMask));
				m_color               = ELogColor(m_color | (color & colorMask) | LogColor_BackgroundBit | LogColor_BackgroundColorBit);
			}

			// Test 'light' bit
			if (color & LogColor_BackgroundLightBit)
			{
				const ELogColor lightMask = ELogColor(LogColor_BackgroundLightBit | LogColor_BackgroundLightStateBit);
				m_color               = ELogColor(m_color & (~lightMask));
				m_color               = ELogColor(m_color | (color & lightMask) | LogColor_BackgroundBit);
			}

			// Test 'blink' bit
			if (color & LogColor_BackgroundBlinkBit)
			{
				const ELogColor blinkMask = ELogColor(LogColor_BackgroundBlinkBit | LogColor_BackgroundBlinkStateBit);
				m_color               = ELogColor(m_color & (~blinkMask));
				m_color               = ELogColor(m_color | (color & blinkMask) | LogColor_BackgroundBit);
			}
		}

		// Finally applies current color
		applyColor();
	}
}

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

void CLogListenerConsole::applyColor()
{
	int gotACommand=0;

	#define _command_separator_ (gotACommand++?";":"")

	cout << "\033[00m";

	if(m_color!=LogColor_Default)
	{
		cout << "\033[";

		if(m_color&LogColor_ForegroundBit)
		{
			if(m_color&LogColor_ForegroundLightBit && m_color&LogColor_ForegroundLightStateBit)
			{
				// No function to do that
			}

			if(m_color&LogColor_ForegroundBoldBit && m_color&LogColor_ForegroundBoldStateBit)
			{
				cout << _command_separator_ << "01";
			}

			if(m_color&LogColor_ForegroundUnderlineBit && m_color&LogColor_ForegroundUnderlineStateBit)
			{
				cout << _command_separator_ << "04";
			}

			if(m_color&LogColor_ForegroundBlinkBit && m_color&LogColor_ForegroundBlinkStateBit)
			{
				cout << _command_separator_ << "05";
			}

			if(m_color&LogColor_ForegroundColorBit)
			{
				ELogColor color=ELogColor(m_color&(LogColor_ForegroundBit|LogColor_ForegroundColorBit|LogColor_ForegroundColorRedBit|LogColor_ForegroundColorGreenBit|LogColor_ForegroundColorBlueBit));
				switch(color)
				{
					case LogColor_ForegroundBlack:    cout << _command_separator_ << "30"; break;
					case LogColor_ForegroundRed:      cout << _command_separator_ << "31"; break;
					case LogColor_ForegroundGreen:    cout << _command_separator_ << "32"; break;
					case LogColor_ForegroundYellow:   cout << _command_separator_ << "33"; break;
					case LogColor_ForegroundBlue:     cout << _command_separator_ << "34"; break;
					case LogColor_ForegroundMagenta:  cout << _command_separator_ << "35"; break;
					case LogColor_ForegroundCyan:     cout << _command_separator_ << "36"; break;
					case LogColor_ForegroundWhite:    cout << _command_separator_ << "37"; break;
					default: break;
				}
			}
		}

		if(m_color&LogColor_BackgroundBit)
		{
			if(m_color&LogColor_BackgroundColorBit)
			{
				ELogColor color=ELogColor(m_color&(LogColor_BackgroundBit|LogColor_BackgroundColorBit|LogColor_BackgroundColorRedBit|LogColor_BackgroundColorGreenBit|LogColor_BackgroundColorBlueBit));
				switch(color)
				{
					case LogColor_BackgroundBlack:    cout << _command_separator_ << "40"; break;
					case LogColor_BackgroundRed:      cout << _command_separator_ << "41"; break;
					case LogColor_BackgroundGreen:    cout << _command_separator_ << "42"; break;
					case LogColor_BackgroundYellow:   cout << _command_separator_ << "43"; break;
					case LogColor_BackgroundBlue:     cout << _command_separator_ << "44"; break;
					case LogColor_BackgroundMagenta:  cout << _command_separator_ << "45"; break;
					case LogColor_BackgroundCyan:     cout << _command_separator_ << "46"; break;
					case LogColor_BackgroundWhite:    cout << _command_separator_ << "47"; break;
					default: break;
				}
			}

			if(m_color&LogColor_BackgroundLightBit && m_color&LogColor_BackgroundLightStateBit)
			{
				// No function to do that
			}

			if(m_color&LogColor_BackgroundBlinkBit && m_color&LogColor_BackgroundBlinkStateBit)
			{
				// No function to do that
			}
		}

		cout << "m";
	}

	#undef _command_separator_

}

#elif defined TARGET_OS_Windows

void CLogListenerConsole::applyColor()
{
	WORD attribute = 0;

	if (m_color & LogColor_ForegroundBit)
	{
		if (m_color & LogColor_ForegroundColorBit)
		{
			attribute |= ((m_color & LogColor_ForegroundColorRedBit) ? FOREGROUND_RED : 0);
			attribute |= ((m_color & LogColor_ForegroundColorGreenBit) ? FOREGROUND_GREEN : 0);
			attribute |= ((m_color & LogColor_ForegroundColorBlueBit) ? FOREGROUND_BLUE : 0);
		}
		else
		{
			attribute |= FOREGROUND_RED;
			attribute |= FOREGROUND_GREEN;
			attribute |= FOREGROUND_BLUE;
		}

		if (m_color & LogColor_ForegroundLightBit && m_color & LogColor_ForegroundLightStateBit) { attribute |= FOREGROUND_INTENSITY; }

		if (m_color & LogColor_ForegroundBoldBit && m_color & LogColor_ForegroundBoldStateBit)
		{
			// No function to do that
		}

		if (m_color & LogColor_ForegroundUnderlineBit && m_color & LogColor_ForegroundUnderlineStateBit) { attribute |= COMMON_LVB_UNDERSCORE; }

		if (m_color & LogColor_ForegroundBlinkBit && m_color & LogColor_ForegroundBlinkStateBit)
		{
			// No function to do that
		}
	}
	else
	{
		attribute |= FOREGROUND_RED;
		attribute |= FOREGROUND_GREEN;
		attribute |= FOREGROUND_BLUE;
	}

	if (m_color & LogColor_BackgroundBit)
	{
		if (m_color & LogColor_BackgroundColorBit)
		{
			attribute |= ((m_color & LogColor_BackgroundColorRedBit) ? BACKGROUND_RED : 0);
			attribute |= ((m_color & LogColor_BackgroundColorGreenBit) ? BACKGROUND_GREEN : 0);
			attribute |= ((m_color & LogColor_BackgroundColorBlueBit) ? BACKGROUND_BLUE : 0);
		}

		if (m_color & LogColor_BackgroundLightBit && m_color & LogColor_BackgroundLightStateBit) { attribute |= BACKGROUND_INTENSITY; }

		if (m_color & LogColor_BackgroundBlinkBit && m_color & LogColor_BackgroundBlinkStateBit)
		{
			// No function to do that
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attribute);
}

#else

void CLogListenerConsole::applyColor()
{
}

#endif
