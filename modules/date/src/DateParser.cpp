#include "DateParser.h"

using namespace DateParser;

char* CDateParser::strpTime(const char* sStringToParse, const char* sDateFormat, struct tm* tmParsed)
{
#if defined TARGET_OS_Windows
	return windowsStrptime(sStringToParse, sDateFormat, tmParsed);
#else
	return strptime(sStringToParse, sDateFormat, tmParsed);
#endif
}
