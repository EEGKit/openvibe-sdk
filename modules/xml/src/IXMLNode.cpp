#include "IXMLNode.h"

#include <string>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <map>

//This is to remove the warning on windows about strdup
#if defined TARGET_OS_Windows
#define strdup _strdup
#endif

using namespace std;

namespace XML {
class IXMLNodeImpl final : public IXMLNode
{
public:
	explicit IXMLNodeImpl(const char* name) : m_name(name) {}
	const char* getName() const override { return m_name.c_str(); }
	void release() override { delete this; }

	//Attribute
	bool addAttribute(const char* name, const char* value) override;
	bool hasAttribute(const char* name) const override { return m_attibutes.count(name) != 0; }
	const char* getAttribute(const char* name) const override;

	//PCDATA
	void setPCData(const char* data) override;
	void appendPCData(const char* data) override;
	const char* getPCData() const override { return m_pcData.c_str(); }

	//Child
	void addChild(IXMLNode* node) override { m_nodes.push_back(node); }
	IXMLNode* getChild(const size_t index) const override { return m_nodes[index]; }
	IXMLNode* getChildByName(const char* name) const override;
	size_t getChildCount() const override;

	//XMl generation
	char* getXML(const size_t depth = 0) const override;

protected:
	~IXMLNodeImpl() override { for (size_t i = 0; i < getChildCount(); ++i) { getChild(i)->release(); } }

private:
	static std::string sanitize(const std::string& str);
	static void applyIndentation(std::string& str, const size_t depth);


	std::vector<IXMLNode*> m_nodes;
	std::map<std::string, std::string> m_attibutes;
	std::string m_name   = "";
	std::string m_pcData = "";
	bool m_hasPCData     = false;
};
}	// namespace XML

using namespace std;
using namespace XML;


bool IXMLNodeImpl::addAttribute(const char* name, const char* value)
{
	m_attibutes[name] = value;
	return true;
}

const char* IXMLNodeImpl::getAttribute(const char* name) const
{
	const char* res = nullptr;
	const std::string str(name);

	const auto it = m_attibutes.find(str);
	if (it != m_attibutes.end()) { res = (*it).second.c_str(); }

	return res;
}

void IXMLNodeImpl::setPCData(const char* data)
{
	m_pcData    = data;
	m_hasPCData = true;
}

void IXMLNodeImpl::appendPCData(const char* data)
{
	m_pcData += data;
	m_hasPCData = true;
}

IXMLNode* IXMLNodeImpl::getChildByName(const char* name) const
{
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		IXMLNode* node = static_cast<IXMLNode*>(*it);
		if (strcmp(node->getName(), name) == 0) { return node; }
	}
	return nullptr;
}

size_t IXMLNodeImpl::getChildCount() const { return m_nodes.size(); }

std::string IXMLNodeImpl::sanitize(const string& str)
{
	string::size_type i;
	string res(str);
	if (res.length() != 0)
	{
		// mandatory, this one should be the first because the other ones add & symbols
		for (i = res.find('&', 0); i != string::npos; i = res.find('&', i + 1)) { res.replace(i, 1, "&amp;"); }
		// other escape sequences
		for (i = res.find('\"', 0); i != string::npos; i = res.find('\"', i + 1)) { res.replace(i, 1, "&quot;"); }
		for (i = res.find('<', 0); i != string::npos; i = res.find('<', i + 1)) { res.replace(i, 1, "&lt;"); }
		for (i = res.find('>', 0); i != string::npos; i = res.find('>', i + 1)) { res.replace(i, 1, "&gt;"); }
	}
	return res;
}

void IXMLNodeImpl::applyIndentation(string& str, const size_t depth)
{
	const string indent(depth, '\t');
	str.append(indent);
}

char* IXMLNodeImpl::getXML(const size_t depth) const
{
	string str;
	applyIndentation(str, depth);
	str += "<" + m_name;

	//Add attributes if we have some
	if (!m_attibutes.empty())
	{
		for (auto it = m_attibutes.begin(); it != m_attibutes.end(); ++it)
		{
			str += string(" ") + it->first + string("=\"") + sanitize(it->second) + string("\"");
		}
	}
	//If we have nothing else to print let's close the node and return
	if (!m_hasPCData && m_nodes.empty())
	{
		str = str + string("/>");
		return ::strdup(str.c_str());
	}

	str = str + string(">");

	if (m_hasPCData) { str = str + sanitize(m_pcData); }

	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		IXMLNode* node = static_cast<IXMLNode*>(*it);
		str += string("\n") + node->getXML(depth + 1);
	}

	if (!m_nodes.empty())
	{
		str = str + "\n";
		applyIndentation(str, depth);
	}
	str = str + "</" + m_name + ">";

	return ::strdup(str.c_str());
}

OV_API IXMLNode* XML::createNode(const char* name) { return new IXMLNodeImpl(name); }
