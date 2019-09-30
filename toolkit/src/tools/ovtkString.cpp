#include "ovtkString.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include <cctype>

#include <cstring>

using namespace OpenViBE;

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace String
		{
			namespace
			{
				bool isSeparator(uint8_t ui8Value, uint8_t* pSeparator, uint32_t ui32SeparatorCount)
				{
					for (uint32_t i = 0; i < ui32SeparatorCount; i++) { if (ui8Value == pSeparator[i]) { return true; } }
					return false;
				}

				// because std::tolower has multiple signatures,
				// it can not be easily used in std::transform
				// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
				template <class TCharT>
				TCharT ToLower(TCharT c) { return std::tolower(c); }
			} // namespace
		} // namespace String
	} // namespace Tools
} // namespace OpenViBEToolkit

uint32_t OpenViBEToolkit::Tools::String::split(const CString& rString, const ISplitCallback& rSplitCallback, uint8_t ui8Separator)
{
	return split(rString, rSplitCallback, &ui8Separator, 1);
}

uint32_t OpenViBEToolkit::Tools::String::split(const CString& rString, const ISplitCallback& rSplitCallback, uint8_t* pSeparator, uint32_t ui32SeparatorCount)
{
	if (ui32SeparatorCount == 0 || pSeparator == nullptr) { return 0; }

	uint32_t n = 0;
	std::string str(rString.toASCIIString());
	size_t i = 0;
	while (i < str.length())
	{
		size_t j = i;
		while (j < str.length() && !isSeparator(str[j], pSeparator, ui32SeparatorCount)) { j++; }
		//if(i!=j)
		{
			rSplitCallback.setToken(std::string(str, i, j - i).c_str());
			n++;
		}
		i = j + 1;
	}
	if (str.length() != 0 && isSeparator(str[str.length() - 1], pSeparator, ui32SeparatorCount))
	{
		rSplitCallback.setToken("");
		n++;
	}

	return n;
}


bool OpenViBEToolkit::Tools::String::isAlmostEqual(const CString& rString1, const CString& rString2, const bool bCaseSensitive, const bool bRemoveStartSpaces,
												   const bool bRemoveEndSpaces)
{
	const char* str1Start = rString1.toASCIIString();
	const char* str1End   = str1Start + strlen(str1Start) - 1;

	const char* str2Start = rString2.toASCIIString();
	const char* str2End   = str2Start + strlen(str2Start) - 1;

	if (bRemoveStartSpaces)
	{
		while (*str1Start == ' ') { str1Start++; }
		while (*str2Start == ' ') { str2Start++; }
	}

	if (bRemoveEndSpaces)
	{
		while (str1Start < str1End && *str1End == ' ') { str1End--; }
		while (str2Start < str2End && *str2End == ' ') { str2End--; }
	}

	std::string str1(str1Start, str1End - str1Start + 1);
	std::string str2(str2Start, str2End - str2Start + 1);

	if (!bCaseSensitive)
	{
		std::transform(str1.begin(), str1.end(), str1.begin(), ToLower<std::string::value_type>);
		std::transform(str2.begin(), str2.end(), str2.begin(), ToLower<std::string::value_type>);
	}

	return str1 == str2;
}
