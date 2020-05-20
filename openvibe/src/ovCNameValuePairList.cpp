#include "ovCString.h"
#include "ovCNameValuePairList.h"

#include <cstdio>
#include <string>
#include <map>


namespace OpenViBE {

struct CNameValuePairListImpl
{
	std::map<CString, CString> m_Map;
};

CNameValuePairList::CNameValuePairList() { m_impl = new CNameValuePairListImpl(); }

CNameValuePairList::CNameValuePairList(const CNameValuePairList& pairs)
{
	m_impl        = new CNameValuePairListImpl();
	m_impl->m_Map = pairs.m_impl->m_Map;
}

CNameValuePairList::~CNameValuePairList() { delete m_impl; }

CNameValuePairList& CNameValuePairList::operator=(const CNameValuePairList& pairs)
{
	m_impl->m_Map = pairs.m_impl->m_Map;
	return *this;
}

bool CNameValuePairList::setValue(const CString& name, const CString& value) const
{
	m_impl->m_Map[name] = value;
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const char* value) const
{
	if (value == nullptr) { return false; }
	m_impl->m_Map[name] = value;
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const double& value) const
{
	m_impl->m_Map[name] = std::to_string(value).c_str();
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const bool value) const
{
	m_impl->m_Map[name] = value ? "1" : "0";
	return true;
}

bool CNameValuePairList::getValue(const CString& name, CString& value) const
{
	if (m_impl->m_Map.find(name) == m_impl->m_Map.end()) { return false; }
	value = m_impl->m_Map[name];
	return true;
}

bool CNameValuePairList::getValue(const CString& name, double& value) const
{
	if (m_impl->m_Map.find(name) == m_impl->m_Map.end()) { return false; }
	double temp;

	try { temp = std::stod(m_impl->m_Map[name].toASCIIString()); }
	catch (const std::exception&) { return false; }

	value = temp;
	return true;
}

bool CNameValuePairList::getValue(const CString& name, bool& value) const
{
	if (m_impl->m_Map.find(name) == m_impl->m_Map.end()) { return false; }
	const CString str = m_impl->m_Map[name];
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

bool CNameValuePairList::getValue(const size_t index, CString& name, CString& value) const
{
	if (index >= this->getSize()) { return false; }
	auto it = m_impl->m_Map.begin();
	std::advance(it, index);
	name  = it->first;
	value = it->second;
	return true;
}

size_t CNameValuePairList::getSize() const { return m_impl->m_Map.size(); }

} // namespace OpenViBE
