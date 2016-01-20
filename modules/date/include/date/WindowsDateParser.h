#pragma once
#include "defines.h"
#include <ctime>

namespace DateParser
{
	extern Date_API char* windowsStrptime(const char* sStringToParse, const char* sDateFormat, struct tm* tmParsed);
}

