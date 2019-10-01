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

bool CNameValuePairList::setValue(const CString& name, const CString& rValue)
{
	m_pNameValuePairListImpl->m_Map[name] = rValue;
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const char* pValue)
{
	if (pValue == nullptr) { return false; }
	m_pNameValuePairListImpl->m_Map[name] = pValue;
	return true;
}

bool CNameValuePairList::setValue(const CString& name, const double& rValue)
{
	char buffer[1024];
	sprintf(buffer, "%lf", rValue);
	m_pNameValuePairListImpl->m_Map[name] = buffer;
	return true;
}

bool CNameValuePairList::setValue(const CString& name, bool bValue)
{
	m_pNameValuePairListImpl->m_Map[name] = bValue ? "1" : "0";
	return true;
}

bool CNameValuePairList::getValue(const CString& name, CString& rValue) const
{
	if (m_pNameValuePairListImpl->m_Map.find(name) == m_pNameValuePairListImpl->m_Map.end()) { return false; }
	rValue = m_pNameValuePairListImpl->m_Map[name];
	return true;
}

bool CNameValuePairList::getValue(const CString& name, double& rValue) const
{
	if (m_pNameValuePairListImpl->m_Map.find(name) == m_pNameValuePairListImpl->m_Map.end()) { return false; }
	double temp;

	try { temp = std::stod(m_pNameValuePairListImpl->m_Map[name].toASCIIString()); }
	catch (const std::exception&) { return false; }

	rValue = temp;
	return true;
}

bool CNameValuePairList::getValue(const CString& name, bool& rValue) const
{
	if (m_pNameValuePairListImpl->m_Map.find(name) == m_pNameValuePairListImpl->m_Map.end()) { return false; }
	const CString value = m_pNameValuePairListImpl->m_Map[name];
	if (value == CString("0") || value == CString("FALSE") || value == CString("false"))
	{
		rValue = false;
		return true;
	}
	if (value == CString("1") || value == CString("TRUE") || value == CString("true"))
	{
		rValue = true;
		return true;
	}
	return false;
}

bool CNameValuePairList::getValue(const uint32_t rIndex, CString& name, CString& rValue) const
{
	if (rIndex >= this->getSize()) { return false; }
	auto it = m_pNameValuePairListImpl->m_Map.begin();
	std::advance(it, rIndex);
	name  = it->first;
	rValue = it->second;
	return true;
}

uint32_t CNameValuePairList::getSize() const { return uint32_t(m_pNameValuePairListImpl->m_Map.size()); }
