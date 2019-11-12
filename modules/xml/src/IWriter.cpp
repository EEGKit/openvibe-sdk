#include "IWriter.h"

#include <stack>
#include <string>

using namespace XML;
using namespace std;

namespace XML
{
	class CWriter final : public IWriter
	{
	public:
		explicit CWriter(IWriterCallback& rWriterCallback);
		bool openChild(const char* name) override;
		bool setChildData(const char* sData) override;
		bool setAttribute(const char* sAttributeName, const char* sAttributeValue) override;
		bool closeChild() override;
		void release() override;

	private:
		void sanitize(string& sString, bool escapeQuotes = true);

	protected:
		IWriterCallback& m_rWriterCallback;
		stack<string> m_nodes;
		bool m_hasChild             = false;
		bool m_hasData              = false;
		bool m_hasClosedOpeningNode = true;
	};
}

CWriter::CWriter(IWriterCallback& rWriterCallback) : m_rWriterCallback(rWriterCallback) {}

bool CWriter::openChild(const char* name)
{
	if (name == nullptr) { return false; }

	if (m_hasData) { return false; }

	if (!m_hasClosedOpeningNode)
	{
		m_rWriterCallback.write(">");
		m_hasClosedOpeningNode = true;
	}

	string l_sIndent(m_nodes.size(), '\t');
	string res = (!m_nodes.empty() ? string("\n") : string("")) + l_sIndent + string("<") + string(name);
	m_rWriterCallback.write(res.c_str());
	m_nodes.push(name);
	m_hasChild             = false;
	m_hasData              = false;
	m_hasClosedOpeningNode = false;
	return true;
}

bool CWriter::setChildData(const char* sData)
{
	if (sData == nullptr) { return false; }

	if (m_hasChild) { return false; }

	if (!m_hasClosedOpeningNode)
	{
		m_rWriterCallback.write(">");
		m_hasClosedOpeningNode = true;
	}

	string l_sData(sData);
	this->sanitize(l_sData, false);

	m_rWriterCallback.write(l_sData.c_str());
	m_hasChild = false;
	m_hasData  = true;
	return true;
}

bool CWriter::setAttribute(const char* sAttributeName, const char* sAttributeValue)
{
	if (sAttributeName == nullptr) { return false; }

	if (sAttributeValue == nullptr) { return false; }

	if (m_hasChild) { return false; }

	if (m_hasData) { return false; }

	if (m_hasClosedOpeningNode) { return false; }

	string l_sAttributeValue(sAttributeValue);
	this->sanitize(l_sAttributeValue);

	string res = string(" ") + string(sAttributeName) + string("=\"") + string(l_sAttributeValue) + string("\"");
	m_rWriterCallback.write(res.c_str());
	return true;
}

bool CWriter::closeChild()
{
	if (m_nodes.empty()) { return false; }

	if (!m_hasClosedOpeningNode)
	{
		m_rWriterCallback.write(">");
		m_hasClosedOpeningNode = true;
	}

	string l_sIndent(m_nodes.size() - 1, '\t');
	string res = ((m_hasData || !m_hasChild) ? string("") : string("\n") + l_sIndent) + string("</") + m_nodes.top() + string(">");
	m_rWriterCallback.write(res.c_str());
	m_nodes.pop();
	m_hasChild = true;
	m_hasData  = false;
	return true;
}

void CWriter::release()
{
	while (!m_nodes.empty()) { closeChild(); }
	delete this;
}

void CWriter::sanitize(string& sString, bool escapeQuotes)
{
	string::size_type i;
	if (sString.length() != 0)
	{
		// mandatory, this one should be the first because the other ones add & symbols
		for (i = sString.find("&", 0); i != string::npos; i = sString.find("&", i + 1)) { sString.replace(i, 1, "&amp;"); }
		for (i = sString.find("<", 0); i != string::npos; i = sString.find("<", i + 1)) { sString.replace(i, 1, "&lt;"); }
		for (i = sString.find(">", 0); i != string::npos; i = sString.find(">", i + 1)) { sString.replace(i, 1, "&gt;"); }

		// Quotes need only be escaped in attributes
		if (escapeQuotes)
		{
			for (i = sString.find("'", 0); i != string::npos; i = sString.find("'", i + 1)) { sString.replace(i, 1, "&apos;"); }
			for (i = sString.find("\"", 0); i != string::npos; i = sString.find("\"", i + 1)) { sString.replace(i, 1, "&quot;"); }
		}
	}
}

XML_API IWriter* XML::createWriter(IWriterCallback& rWriterCallback) { return new CWriter(rWriterCallback); }
