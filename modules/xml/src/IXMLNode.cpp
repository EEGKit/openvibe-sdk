#include "IXMLNode.h"

#include <string>
#include <cstring>
#include <stdexcept>
#include <stack>
#include <vector>
#include <map>

//This is to remove the warning on windows about strdup
#if defined TARGET_OS_Windows
#define strdup _strdup
#endif

namespace XML
{
	class IXMLNodeImpl : public IXMLNode
	{
	public:
		explicit IXMLNodeImpl(const char* sName);

		virtual const char* getName() const;

		virtual void release(void);

		//Attribute
		virtual bool addAttribute(const char* sAttributeName, const char* sAttributeValue);
		virtual bool hasAttribute(const char* sAttributeName) const;
		virtual const char* getAttribute(const char* sAttributeName) const;

		//PCDATA
		virtual void setPCData(const char* childData);
		virtual void appendPCData(const char* childData);
		virtual const char* getPCData(void) const;

		//Child
		virtual void addChild(IXMLNode* pChildNode);
		virtual IXMLNode* getChild(const size_t iChildIndex) const;
		virtual IXMLNode* getChildByName(const char* sName) const;
		virtual size_t getChildCount(void) const;

		//XMl generation
		virtual char* getXML(const uint32_t depth = 0) const;

	protected:
		virtual ~IXMLNodeImpl(void);

	private:
		std::string sanitize(const std::string& sString) const;
		void applyIndentation(std::string& sString, uint32_t depth) const;


		std::vector<IXMLNode *> m_oNodeVector;
		std::map<std::string, std::string> m_mAttibuteMap;
		std::string m_sNodeName;
		std::string m_sPCData;
		bool m_bHasPCData;
	};
}

using namespace std;
using namespace XML;

IXMLNodeImpl::~IXMLNodeImpl(void)
{
	for (size_t i = 0; i < getChildCount(); ++i)
	{
		getChild(i)->release();
	}
}

void IXMLNodeImpl::release(void)
{
	delete this;
}

IXMLNodeImpl::IXMLNodeImpl(const char* sName): m_sNodeName(sName)
											   , m_sPCData("")
											   , m_bHasPCData(false) {}

const char* IXMLNodeImpl::getName() const
{
	return m_sNodeName.c_str();
}

bool IXMLNodeImpl::addAttribute(const char* sAttributeName, const char* sAttributeValue)
{
	m_mAttibuteMap[sAttributeName] = sAttributeValue;
	return true;
}

bool IXMLNodeImpl::hasAttribute(const char* sAttributeName) const
{
	return m_mAttibuteMap.count(sAttributeName) != 0;
}

const char* IXMLNodeImpl::getAttribute(const char* sAttributeName) const
{
	const char* res = NULL;
	const std::string l_sAttributeName(sAttributeName);

	std::map<std::string, std::string>::const_iterator it = m_mAttibuteMap.find(l_sAttributeName);
	if (it != m_mAttibuteMap.end())
	{
		res = (*it).second.c_str();
	}

	return res;
}

void IXMLNodeImpl::setPCData(const char* childData)
{
	m_sPCData    = childData;
	m_bHasPCData = true;
}

void IXMLNodeImpl::appendPCData(const char* childData)
{
	m_sPCData += childData;
	m_bHasPCData = true;
}

const char* IXMLNodeImpl::getPCData(void) const
{
	return m_sPCData.c_str();
}

void IXMLNodeImpl::addChild(IXMLNode* pChildNode)
{
	m_oNodeVector.push_back(pChildNode);
}

IXMLNode* IXMLNodeImpl::getChild(const size_t iChildIndex) const
{
	return m_oNodeVector[iChildIndex];
}

IXMLNode* IXMLNodeImpl::getChildByName(const char* sName) const
{
	for (vector<IXMLNode*>::const_iterator it = m_oNodeVector.begin(); it != m_oNodeVector.end(); ++it)
	{
		IXMLNode* l_sTempNode = static_cast<IXMLNode*>(*it);
		if (strcmp(l_sTempNode->getName(), sName) == 0)
			return l_sTempNode;
	}
	return NULL;
}

size_t IXMLNodeImpl::getChildCount(void) const
{
	return m_oNodeVector.size();
}

std::string IXMLNodeImpl::sanitize(const string& sString) const
{
	string::size_type i;
	string l_sRes(sString);
	if (l_sRes.length() != 0)
	{
		// mandatory, this one should be the first because the other ones add & symbols
		for (i = l_sRes.find("&", 0); i != string::npos; i = l_sRes.find("&", i + 1))
			l_sRes.replace(i, 1, "&amp;");
		// other escape sequences
		for (i = l_sRes.find("\"", 0); i != string::npos; i = l_sRes.find("\"", i + 1))
			l_sRes.replace(i, 1, "&quot;");
		for (i = l_sRes.find("<", 0); i != string::npos; i = l_sRes.find("<", i + 1))
			l_sRes.replace(i, 1, "&lt;");
		for (i = l_sRes.find(">", 0); i != string::npos; i = l_sRes.find(">", i + 1))
			l_sRes.replace(i, 1, "&gt;");
	}
	return l_sRes;
}

void IXMLNodeImpl::applyIndentation(string& sString, uint32_t depth) const
{
	string l_sIndent(depth, '\t');
	sString.append(l_sIndent);
}

char* IXMLNodeImpl::getXML(const uint32_t depth) const
{
	string l_sRes;
	applyIndentation(l_sRes, depth);
	l_sRes = l_sRes + "<" + m_sNodeName;

	//Add attributes if we have some
	if (!m_mAttibuteMap.empty())
	{
		for (map<string, string>::const_iterator it = m_mAttibuteMap.begin(); it != m_mAttibuteMap.end(); ++it)
		{
			l_sRes = l_sRes + string(" ") + it->first + string("=\"") + sanitize(it->second) + string("\"");
		}
	}
	//If we have nothing else to print let's close the node and return
	if (!m_bHasPCData && m_oNodeVector.empty())
	{
		l_sRes       = l_sRes + string("/>");
		char* l_pRes = ::strdup(l_sRes.c_str());
		return l_pRes;
	}

	l_sRes = l_sRes + string(">");

	if (m_bHasPCData)
	{
		l_sRes = l_sRes + sanitize(m_sPCData);
	}

	for (vector<IXMLNode*>::const_iterator it = m_oNodeVector.begin(); it != m_oNodeVector.end(); ++it)
	{
		IXMLNode* l_sTempNode = static_cast<IXMLNode *>(*it);
		l_sRes                = l_sRes + string("\n") + l_sTempNode->getXML(depth + 1);
	}

	if (!m_oNodeVector.empty())
	{
		l_sRes = l_sRes + "\n";
		applyIndentation(l_sRes, depth);
	}
	l_sRes = l_sRes + "</" + m_sNodeName + ">";

	char* l_pRes = ::strdup(l_sRes.c_str());
	return l_pRes;
}

OV_API IXMLNode* XML::createNode(const char* sName)
{
	return new IXMLNodeImpl(sName);
}
