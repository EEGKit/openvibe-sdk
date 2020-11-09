#pragma once
#include "defines.h"
#if defined TARGET_OS_Windows
#include "WindowsDateParser.h"
#else
#include <ctime>
#endif

namespace DateParser {
class Date_API CDateParser
{
public:
	/**
	* \brief parse a string representing a date along a format given by a char*, and store it in  a tm struct
	* \param sStringToParse: the string to parse
	* \param sDateFormat: the string that gives the format of the date to parse, example "%Y-%m-%d %H:%M"
	* \param tmParsed: time parsed
	*/
	static char* strpTime(const char* sStringToParse, const char* sDateFormat, struct tm* tmParsed);

private:
	CDateParser() = default;
};
}  // namespace DateParser
