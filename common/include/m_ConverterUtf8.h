#pragma once

#include <vector>
#include <stdexcept>

namespace Common {
namespace Converter {

#if defined TARGET_OS_Windows
// Returns the wstring (ie the utf16 formatted string) version of an utf8 string
static std::wstring Utf8ToUtf16(const std::string& utf8)
{
	std::vector<unsigned long> unicode;
	size_t i = 0;
	while (i < utf8.size())
	{
		unsigned long uni;
		size_t todo;
		unsigned char ch = utf8[i++];
		if (ch <= 0x7F)
		{
			uni  = ch;
			todo = 0;
		}
		else if (ch <= 0xBF) { throw std::logic_error("not a UTF-8 string"); }
		else if (ch <= 0xDF)
		{
			uni  = ch & 0x1F;
			todo = 1;
		}
		else if (ch <= 0xEF)
		{
			uni  = ch & 0x0F;
			todo = 2;
		}
		else if (ch <= 0xF7)
		{
			uni  = ch & 0x07;
			todo = 3;
		}
		else { throw std::logic_error("not a UTF-8 string"); }
		for (size_t j = 0; j < todo; ++j)
		{
			if (i == utf8.size()) { throw std::logic_error("not a UTF-8 string"); }
			ch = utf8[i++];
			if (ch < 0x80 || ch > 0xBF) { throw std::logic_error("not a UTF-8 string"); }
			uni <<= 6;
			uni += ch & 0x3F;
		}
		if (uni >= 0xD800 && uni <= 0xDFFF) { throw std::logic_error("not a UTF-8 string"); }
		if (uni > 0x10FFFF) { throw std::logic_error("not a UTF-8 string"); }
		unicode.push_back(uni);
	}
	std::wstring utf16;
	for (i = 0; i < unicode.size(); ++i)
	{
		unsigned long uni = unicode[i];
		if (uni <= 0xFFFF) { utf16 += wchar_t(uni); }
		else
		{
			uni -= 0x10000;
			utf16 += wchar_t((uni >> 10) + 0xD800);
			utf16 += wchar_t((uni & 0x3FF) + 0xDC00);
		}
	}
	return utf16;
}
#endif // TARGET_OS_Windows

}  // namespace Converter
}  // namespace Common
