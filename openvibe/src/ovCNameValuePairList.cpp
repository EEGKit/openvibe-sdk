#include "ovCString.h"
#include "ovCNameValuePairList.h"

#include <cstdio>
#include <string>
#include <map>

using namespace OpenViBE;

namespace OpenViBE
{
	struct CNameValuePairListImpl { std::map<CString, CString> m_oMap; };
}

CNameValuePairList::CNameValuePairList()
{
	m_pNameValuePairListImpl = new CNameValuePairListImpl();
}

CNameValuePairList::CNameValuePairList(const CNameValuePairList& rNameValuePairList)
{
	m_pNameValuePairListImpl         = new CNameValuePairListImpl();
	m_pNameValuePairListImpl->m_oMap = rNameValuePairList.m_pNameValuePairListImpl->m_oMap;
}

CNameValuePairList::~CNameValuePairList()
{
	delete m_pNameValuePairListImpl;
}

CNameValuePairList& CNameValuePairList::operator=(const CNameValuePairList& rNameValuePairList)
{
	m_pNameValuePairListImpl->m_oMap = rNameValuePairList.m_pNameValuePairListImpl->m_oMap;
	return *this;
}

bool CNameValuePairList::setValue(const CString& rName, const CString& rValue)
{
	m_pNameValuePairListImpl->m_oMap[rName] = rValue;
	return true;
}

bool CNameValuePairList::setValue(const CString& rName, const char* pValue)
{
	if (pValue == nullptr) { return false; }
	m_pNameValuePairListImpl->m_oMap[rName] = pValue;
	return true;
}

bool CNameValuePairList::setValue(const CString& rName, const double& rValue)
{
	char s_pBuffer[1024];
	sprintf(s_pBuffer, "%lf", rValue);
	m_pNameValuePairListImpl->m_oMap[rName] = s_pBuffer;
	return true;
}

bool CNameValuePairList::setValue(const CString& rName, bool bValue)
{
	m_pNameValuePairListImpl->m_oMap[rName] = bValue ? "1" : "0";
	return true;
}

bool CNameValuePairList::getValue(const CString& rName, CString& rValue) const
{
	if (m_pNameValuePairListImpl->m_oMap.find(rName) == m_pNameValuePairListImpl->m_oMap.end()) { return false; }
	rValue = m_pNameValuePairListImpl->m_oMap[rName];
	return true;
}

bool CNameValuePairList::getValue(const CString& rName, double& rValue) const
{
	if (m_pNameValuePairListImpl->m_oMap.find(rName) == m_pNameValuePairListImpl->m_oMap.end()) { return false; }
	double temp;

	try
	{
		temp = std::stod(m_pNameValuePairListImpl->m_oMap[rName].toASCIIString());
	}
	catch (const std::exception&) { return false; }

	rValue = temp;
	return true;
}

bool CNameValuePairList::getValue(const CString& rName, bool& rValue) const
{
	if (m_pNameValuePairListImpl->m_oMap.find(rName) == m_pNameValuePairListImpl->m_oMap.end()) { return false; }
	CString l_sValue = m_pNameValuePairListImpl->m_oMap[rName];
	if (l_sValue == CString("0") || l_sValue == CString("FALSE") || l_sValue == CString("false"))
	{
		rValue = false;
		return true;
	}
	if (l_sValue == CString("1") || l_sValue == CString("TRUE") || l_sValue == CString("true"))
	{
		rValue = true;
		return true;
	}
	return false;
}

bool CNameValuePairList::getValue(const uint32_t rIndex, CString& rName, CString& rValue) const
{
	if (rIndex >= this->getSize()) { return false; }
	std::map<CString, CString>::const_iterator it = m_pNameValuePairListImpl->m_oMap.begin();
	std::advance(it, rIndex);
	rName  = it->first;
	rValue = it->second;
	return true;
}

uint32_t CNameValuePairList::getSize() const { return static_cast<uint32_t>(m_pNameValuePairListImpl->m_oMap.size()); }
