#include "ovCString.h"
#include "ovCNameValuePairList.h"

#include <cstdio>
#include <string>
#include <map>

using namespace OpenViBE;

namespace OpenViBE
{
	struct CNameValuePairListImpl
	{
		std::map<CString, CString> m_Map;
	};
}

CNameValuePairList::CNameValuePairList() { m_pNameValuePairListImpl = new CNameValuePairListImpl(); }

CNameValuePairList::CNameValuePairList(const CNameValuePairList& rNameValuePairList)
{
	m_pNameValuePairListImpl        = new CNameValuePairListImpl();
	m_pNameValuePairListImpl->m_Map = rNameValuePairList.m_pNameValuePairListImpl->m_Map;
}

CNameValuePairList::~CNameValuePairList() { delete m_pNameValuePairListImpl; }

CNameValuePairList& CNameValuePairList::operator=(const CNameValuePairList& rNameValuePairList)
{
	m_pNameValuePairListImpl->m_Map = rNameValuePairList.m_pNameValuePairListImpl->m_Map;
	return *this;
}

bool CNameValuePairList::setValue(const CString& name, const CString& value)
{
	m_pNameValuePairListImpl->m_Map[name] = value;
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const char* value)
{
	if (value == nullptr) { return false; }
	m_pNameValuePairListImpl->m_Map[name] = value;
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const double& value)
{
	m_pNameValuePairListImpl->m_Map[name] = std::to_string(value).c_str();
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const bool value)
{
	m_pNameValuePairListImpl->m_Map[name] = value ? "1" : "0";
	return true;
}

bool CNameValuePairList::getValue(const CString& name, CString& value) const
{
	if (m_pNameValuePairListImpl->m_Map.find(name) == m_pNameValuePairListImpl->m_Map.end()) { return false; }
	value = m_pNameValuePairListImpl->m_Map[name];
	return true;
}

bool CNameValuePairList::getValue(const CString& name, double& value) const
{
	if (m_pNameValuePairListImpl->m_Map.find(name) == m_pNameValuePairListImpl->m_Map.end()) { return false; }
	double temp;

	try { temp = std::stod(m_pNameValuePairListImpl->m_Map[name].toASCIIString()); }
	catch (const std::exception&) { return false; }

	value = temp;
	return true;
}

bool CNameValuePairList::getValue(const CString& name, bool& value) const
{
	if (m_pNameValuePairListImpl->m_Map.find(name) == m_pNameValuePairListImpl->m_Map.end()) { return false; }
	const CString str = m_pNameValuePairListImpl->m_Map[name];
	if (str == CString("0") || str == CString("FALSE") || str == CString("false"))
	{
		value = false;
		return true;
	}
	if (str == CString("1") || str == CString("TRUE") || str == CString("true"))
	{
		value = true;
		return true;
	}
	return false;
}

bool CNameValuePairList::getValue(const uint32_t rIndex, CString& name, CString& value) const
{
	if (rIndex >= this->getSize()) { return false; }
	auto it = m_pNameValuePairListImpl->m_Map.begin();
	std::advance(it, rIndex);
	name  = it->first;
	value = it->second;
	return true;
}

uint32_t CNameValuePairList::getSize() const { return m_pNameValuePairListImpl->m_Map.size(); }
