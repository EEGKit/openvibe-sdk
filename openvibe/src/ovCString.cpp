#include "ovCString.h"

#include <string>

using namespace OpenViBE;

namespace OpenViBE
{
	struct CStringImpl
	{
		std::string m_Value;
	};
}

CString::CString() { m_pStringImpl = new CStringImpl(); }

CString::CString(const CString& rString)
{
	m_pStringImpl          = new CStringImpl();
	m_pStringImpl->m_Value = rString.m_pStringImpl->m_Value;
}

CString::CString(const char* pString)
{
	m_pStringImpl = new CStringImpl();
	if (pString) { m_pStringImpl->m_Value = pString; }
}

CString::~CString() { delete m_pStringImpl; }

CString::operator const char*() const { return m_pStringImpl->m_Value.c_str(); }

CString& CString::operator=(const CString& rString)
{
	m_pStringImpl->m_Value = rString.m_pStringImpl->m_Value;
	return *this;
}

CString& CString::operator+=(const CString& rString)
{
	m_pStringImpl->m_Value += rString.m_pStringImpl->m_Value;
	return *this;
}

char& CString::operator[](size_t idx) { return m_pStringImpl->m_Value[idx]; }

namespace OpenViBE
{
	const CString operator+(const CString& rString1, const CString& rString2)
	{
		std::string l_oResult;
		l_oResult = rString1.m_pStringImpl->m_Value + rString2.m_pStringImpl->m_Value;
		return l_oResult.c_str();
	}

	bool operator==(const CString& rString1, const CString& rString2) { return (rString1.m_pStringImpl->m_Value) == (rString2.m_pStringImpl->m_Value); }

	bool operator!=(const CString& rString1, const CString& rString2) { return (rString1.m_pStringImpl->m_Value) != (rString2.m_pStringImpl->m_Value); }

	bool operator<(const CString& rString1, const CString& rString2) { return (rString1.m_pStringImpl->m_Value) < (rString2.m_pStringImpl->m_Value); }
} // namespace OpenViBE

bool CString::set(const CString& rString)
{
	m_pStringImpl->m_Value = rString.m_pStringImpl->m_Value;
	return true;
}

bool CString::set(const char* pString)
{
	if (pString) { m_pStringImpl->m_Value = pString; }
	else { m_pStringImpl->m_Value = ""; }
	return true;
}

const char* CString::toASCIIString() const { return m_pStringImpl->m_Value.c_str(); }

size_t CString::length() const { return m_pStringImpl->m_Value.length(); }
