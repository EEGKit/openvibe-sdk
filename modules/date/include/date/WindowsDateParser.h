#pragma once
#include "defines.h"
#include <ctime>

namespace DateParser
{
	extern Date_API char* windowsStrptime(const char* buf, const char* fmt, struct tm* tmParsed);
}
