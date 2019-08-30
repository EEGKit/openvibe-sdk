#include "ovkCLogListenerConsole.h"

#include <iostream>
#include <sstream>

#if defined TARGET_OS_Windows
#include <Windows.h>
#endif

#include <openvibe/ovITimeArithmetics.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

CLogListenerConsole::CLogListenerConsole(const IKernelContext& rKernelContext, const CString& sApplicationName)
	: TKernelObject<ILogListener>(rKernelContext)
	  , m_eLogColor(LogColor_Default)
	  , m_sApplicationName(sApplicationName), m_bTimeInSeconds(true)
	  , m_ui64TimePrecision(3)
	  , m_bUseColor(true)
{
#if defined TARGET_OS_Windows
	SetConsoleOutputCP(CP_UTF8);
#endif
}

void CLogListenerConsole::configure(const IConfigurationManager& rConfigurationManager)
{
	m_bTimeInSeconds    = rConfigurationManager.expandAsBoolean("${Kernel_ConsoleLogTimeInSecond}", true);
	m_bLogWithHexa      = rConfigurationManager.expandAsBoolean("${Kernel_ConsoleLogWithHexa}", false);
	m_ui64TimePrecision = rConfigurationManager.expandAsUInteger("${Kernel_ConsoleLogTimePrecision}", 3);
	m_bUseColor         = rConfigurationManager.expandAsBoolean("${Kernel_ConsoleLogUseColor}", true);
}


bool CLogListenerConsole::isActive(ELogLevel eLogLevel)
{
	map<ELogLevel, bool>::iterator itLogLevel = m_vActiveLevel.find(eLogLevel);
	if (itLogLevel == m_vActiveLevel.end()) { return true; }
	return itLogLevel->second;
}

bool CLogListenerConsole::activate(ELogLevel eLogLevel, bool bActive)
{
	m_vActiveLevel[eLogLevel] = bActive;
	return true;
}

bool CLogListenerConsole::activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive)
{
	for (int i = eStartLogLevel; i <= eEndLogLevel; i++) { m_vActiveLevel[ELogLevel(i)] = bActive; }
	return true;
}

bool CLogListenerConsole::activate(bool bActive)
{
	return activate(LogLevel_First, LogLevel_Last, bActive);
}

void CLogListenerConsole::log(const time64 value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();

	if (m_bTimeInSeconds)
	{
		uint64_t l_ui64Precision = m_ui64TimePrecision;
		double l_f64Time         = ITimeArithmetics::timeToSeconds(value.m_ui64TimeValue);
		std::stringstream ss;
		ss.precision(static_cast<long long>(l_ui64Precision));
		ss.setf(std::ios::fixed, std::ios::floatfield);
		ss << l_f64Time;
		ss << " sec";
		if (m_bLogWithHexa)
		{
			ss << " (0x" << hex << value.m_ui64TimeValue << ")";
		}

		cout << ss.str().c_str();
	}
	else
	{
		cout << dec << value.m_ui64TimeValue;
		if (m_bLogWithHexa)
		{
			cout << " (0x" << hex << value.m_ui64TimeValue << ")";
		}
	}

	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint64_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa)
	{
		cout << " (0x" << hex << value << ")";
	}

	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint32_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa)
	{
		cout << " (0x" << hex << value << ")";
	}
	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint16_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa)
	{
		cout << " (0x" << hex << value << ")";
	}
	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const uint8_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int64_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int16_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const int8_t value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	ios_base::fmtflags l_oFormat = cout.flags();
	cout << dec << value;
	if (m_bLogWithHexa) { cout << " (0x" << hex << value << ")"; }
	cout.flags(l_oFormat);
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const double value)
{
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	cout << value;
	this->log(LogColor_PopStateBit);
}

void CLogListenerConsole::log(const float value)
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

void CLogListenerConsole::log(const char* value)
{
	cout << value;
	cout << flush;
}

void CLogListenerConsole::log(const ELogLevel eLogLevel)
{
	switch (eLogLevel)
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

#if 0
	cout << "<";
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundBlue);
	cout << "Application";
	this->log(LogColor_PopStateBit);
	cout << "::";
	this->log(LogColor_PushStateBit);
	this->log(LogColor_ForegroundMagenta);
	cout << m_sApplicationName.toASCIIString();
	this->log(LogColor_PopStateBit);
	cout << "> ";
#endif
}

void CLogListenerConsole::log(const ELogColor eLogColor)
{
	if (m_bUseColor)
	{
		// Tests 'push state' bit
		if (eLogColor & LogColor_PushStateBit) { m_vLogColor.push(m_eLogColor); }

		// Tests 'pop state' bit
		if (eLogColor & LogColor_PopStateBit)
		{
			if (!m_vLogColor.empty())
			{
				m_eLogColor = m_vLogColor.top();
				m_vLogColor.pop();
			}
			else { m_eLogColor = LogColor_Default; }
		}

		// Tests 'reset' bit
		if (eLogColor & LogColor_ResetBit) { m_eLogColor = LogColor_Default; }

		// Tests 'foreground' bit
		if (eLogColor & LogColor_ForegroundBit)
		{
			// Tests 'color' bit
			if (eLogColor & LogColor_ForegroundColorBit)
			{
				ELogColor l_eColorMask = ELogColor(LogColor_ForegroundColorRedBit | LogColor_ForegroundColorGreenBit | LogColor_ForegroundColorBlueBit);
				m_eLogColor            = ELogColor(m_eLogColor & (~l_eColorMask));
				m_eLogColor            = ELogColor(m_eLogColor | (eLogColor & l_eColorMask) | LogColor_ForegroundBit | LogColor_ForegroundColorBit);
			}

			// Test 'light' bit
			if (eLogColor & LogColor_ForegroundLightBit)
			{
				ELogColor l_eLightMask = ELogColor(LogColor_ForegroundLightBit | LogColor_ForegroundLightStateBit);
				m_eLogColor            = ELogColor(m_eLogColor & (~l_eLightMask));
				m_eLogColor            = ELogColor(m_eLogColor | (eLogColor & l_eLightMask) | LogColor_ForegroundBit);
			}

			// Test 'blink' bit
			if (eLogColor & LogColor_ForegroundBlinkBit)
			{
				ELogColor l_eBlinkMask = ELogColor(LogColor_ForegroundBlinkBit | LogColor_ForegroundBlinkStateBit);
				m_eLogColor            = ELogColor(m_eLogColor & (~l_eBlinkMask));
				m_eLogColor            = ELogColor(m_eLogColor | (eLogColor & l_eBlinkMask) | LogColor_ForegroundBit);
			}

			// Test 'bold' bit
			if (eLogColor & LogColor_ForegroundBoldBit)
			{
				ELogColor l_eBoldMask = ELogColor(LogColor_ForegroundBoldBit | LogColor_ForegroundBoldStateBit);
				m_eLogColor           = ELogColor(m_eLogColor & (~l_eBoldMask));
				m_eLogColor           = ELogColor(m_eLogColor | (eLogColor & l_eBoldMask) | LogColor_ForegroundBit);
			}

			// Test 'underline' bit
			if (eLogColor & LogColor_ForegroundUnderlineBit)
			{
				ELogColor l_eUnderlineMask = ELogColor(LogColor_ForegroundBlinkBit | LogColor_ForegroundBlinkStateBit);
				m_eLogColor                = ELogColor(m_eLogColor & (~l_eUnderlineMask));
				m_eLogColor                = ELogColor(m_eLogColor | (eLogColor & l_eUnderlineMask) | LogColor_ForegroundBit);
			}
		}

		// Tests 'background' bit
		if (eLogColor & LogColor_BackgroundBit)
		{
			// Tests 'color' bit
			if (eLogColor & LogColor_BackgroundColorBit)
			{
				ELogColor l_eColorMask = ELogColor(LogColor_BackgroundColorRedBit | LogColor_BackgroundColorGreenBit | LogColor_BackgroundColorBlueBit);
				m_eLogColor            = ELogColor(m_eLogColor & (~l_eColorMask));
				m_eLogColor            = ELogColor(m_eLogColor | (eLogColor & l_eColorMask) | LogColor_BackgroundBit | LogColor_BackgroundColorBit);
			}

			// Test 'light' bit
			if (eLogColor & LogColor_BackgroundLightBit)
			{
				ELogColor l_eLightMask = ELogColor(LogColor_BackgroundLightBit | LogColor_BackgroundLightStateBit);
				m_eLogColor            = ELogColor(m_eLogColor & (~l_eLightMask));
				m_eLogColor            = ELogColor(m_eLogColor | (eLogColor & l_eLightMask) | LogColor_BackgroundBit);
			}

			// Test 'blink' bit
			if (eLogColor & LogColor_BackgroundBlinkBit)
			{
				ELogColor l_eBlinkMask = ELogColor(LogColor_BackgroundBlinkBit | LogColor_BackgroundBlinkStateBit);
				m_eLogColor            = ELogColor(m_eLogColor & (~l_eBlinkMask));
				m_eLogColor            = ELogColor(m_eLogColor | (eLogColor & l_eBlinkMask) | LogColor_BackgroundBit);
			}
		}

		// Finally applies current color
		applyColor();
	}
}

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

void CLogListenerConsole::applyColor()
{
	int l_iGotACommand=0;

	#define _command_separator_ (l_iGotACommand++?";":"")

	cout << "\033[00m";

	if(m_eLogColor!=LogColor_Default)
	{
		cout << "\033[";

		if(m_eLogColor&LogColor_ForegroundBit)
		{
			if(m_eLogColor&LogColor_ForegroundLightBit && m_eLogColor&LogColor_ForegroundLightStateBit)
			{
				// No function to do that
			}

			if(m_eLogColor&LogColor_ForegroundBoldBit && m_eLogColor&LogColor_ForegroundBoldStateBit)
			{
				cout << _command_separator_ << "01";
			}

			if(m_eLogColor&LogColor_ForegroundUnderlineBit && m_eLogColor&LogColor_ForegroundUnderlineStateBit)
			{
				cout << _command_separator_ << "04";
			}

			if(m_eLogColor&LogColor_ForegroundBlinkBit && m_eLogColor&LogColor_ForegroundBlinkStateBit)
			{
				cout << _command_separator_ << "05";
			}

			if(m_eLogColor&LogColor_ForegroundColorBit)
			{
				ELogColor l_eLogColor=ELogColor(m_eLogColor&(LogColor_ForegroundBit|LogColor_ForegroundColorBit|LogColor_ForegroundColorRedBit|LogColor_ForegroundColorGreenBit|LogColor_ForegroundColorBlueBit));
				switch(l_eLogColor)
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

		if(m_eLogColor&LogColor_BackgroundBit)
		{
			if(m_eLogColor&LogColor_BackgroundColorBit)
			{
				ELogColor l_eLogColor=ELogColor(m_eLogColor&(LogColor_BackgroundBit|LogColor_BackgroundColorBit|LogColor_BackgroundColorRedBit|LogColor_BackgroundColorGreenBit|LogColor_BackgroundColorBlueBit));
				switch(l_eLogColor)
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

			if(m_eLogColor&LogColor_BackgroundLightBit && m_eLogColor&LogColor_BackgroundLightStateBit)
			{
				// No function to do that
			}

			if(m_eLogColor&LogColor_BackgroundBlinkBit && m_eLogColor&LogColor_BackgroundBlinkStateBit)
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
	WORD l_dwTextAttribute = 0;

	if (m_eLogColor & LogColor_ForegroundBit)
	{
		if (m_eLogColor & LogColor_ForegroundColorBit)
		{
			l_dwTextAttribute |= ((m_eLogColor & LogColor_ForegroundColorRedBit) ? FOREGROUND_RED : 0);
			l_dwTextAttribute |= ((m_eLogColor & LogColor_ForegroundColorGreenBit) ? FOREGROUND_GREEN : 0);
			l_dwTextAttribute |= ((m_eLogColor & LogColor_ForegroundColorBlueBit) ? FOREGROUND_BLUE : 0);
		}
		else
		{
			l_dwTextAttribute |= FOREGROUND_RED;
			l_dwTextAttribute |= FOREGROUND_GREEN;
			l_dwTextAttribute |= FOREGROUND_BLUE;
		}

		if (m_eLogColor & LogColor_ForegroundLightBit && m_eLogColor & LogColor_ForegroundLightStateBit)
		{
			l_dwTextAttribute |= FOREGROUND_INTENSITY;
		}

		if (m_eLogColor & LogColor_ForegroundBoldBit && m_eLogColor & LogColor_ForegroundBoldStateBit)
		{
			// No function to do that
		}

		if (m_eLogColor & LogColor_ForegroundUnderlineBit && m_eLogColor & LogColor_ForegroundUnderlineStateBit)
		{
			l_dwTextAttribute |= COMMON_LVB_UNDERSCORE;
		}

		if (m_eLogColor & LogColor_ForegroundBlinkBit && m_eLogColor & LogColor_ForegroundBlinkStateBit)
		{
			// No function to do that
		}
	}
	else
	{
		l_dwTextAttribute |= FOREGROUND_RED;
		l_dwTextAttribute |= FOREGROUND_GREEN;
		l_dwTextAttribute |= FOREGROUND_BLUE;
	}

	if (m_eLogColor & LogColor_BackgroundBit)
	{
		if (m_eLogColor & LogColor_BackgroundColorBit)
		{
			l_dwTextAttribute |= ((m_eLogColor & LogColor_BackgroundColorRedBit) ? BACKGROUND_RED : 0);
			l_dwTextAttribute |= ((m_eLogColor & LogColor_BackgroundColorGreenBit) ? BACKGROUND_GREEN : 0);
			l_dwTextAttribute |= ((m_eLogColor & LogColor_BackgroundColorBlueBit) ? BACKGROUND_BLUE : 0);
		}

		if (m_eLogColor & LogColor_BackgroundLightBit && m_eLogColor & LogColor_BackgroundLightStateBit)
		{
			l_dwTextAttribute |= BACKGROUND_INTENSITY;
		}

		if (m_eLogColor & LogColor_BackgroundBlinkBit && m_eLogColor & LogColor_BackgroundBlinkStateBit)
		{
			// No function to do that
		}
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), l_dwTextAttribute);
}

#else

void CLogListenerConsole::applyColor()
{
}

#endif
