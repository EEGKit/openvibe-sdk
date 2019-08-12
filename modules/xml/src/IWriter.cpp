#include "IWriter.h"

#include <stack>
#include <string>

using namespace XML;
using namespace std;

namespace XML
{
	class CWriter : public IWriter
	{
	public:
		explicit CWriter(IWriterCallback& rWriterCallback);
		bool openChild(const char* sName) override;
		bool setChildData(const char* sData) override;
		bool setAttribute(const char* sAttributeName, const char* sAttributeValue) override;
		bool closeChild() override;
		void release() override;

	private:
		void sanitize(string& sString, bool escapeQuotes = true);

	protected:
		IWriterCallback& m_rWriterCallback;
		stack<string> m_vNodes;
		bool m_bHasChild = false;
		bool m_bHasData = false;
		bool m_bHasClosedOpeningNode = true;
	};
}

CWriter::CWriter(IWriterCallback& rWriterCallback) : m_rWriterCallback(rWriterCallback) {}

bool CWriter::openChild(const char* sName)
{
	if (sName == nullptr) { return false; }

	if (m_bHasData) { return false; }

	if (!m_bHasClosedOpeningNode)
	{
		m_rWriterCallback.write(">");
		m_bHasClosedOpeningNode = true;
	}

	string l_sIndent(m_vNodes.size(), '\t');
	string l_sResult = (!m_vNodes.empty() ? string("\n") : string("")) + l_sIndent + string("<") + string(sName);
	m_rWriterCallback.write(l_sResult.c_str());
	m_vNodes.push(sName);
	m_bHasChild             = false;
	m_bHasData              = false;
	m_bHasClosedOpeningNode = false;
	return true;
}

bool CWriter::setChildData(const char* sData)
{
	if (sData == nullptr) { return false; }

	if (m_bHasChild) { return false; }

	if (!m_bHasClosedOpeningNode)
	{
		m_rWriterCallback.write(">");
		m_bHasClosedOpeningNode = true;
	}

	string l_sData(sData);
	this->sanitize(l_sData, false);

	m_rWriterCallback.write(l_sData.c_str());
	m_bHasChild = false;
	m_bHasData  = true;
	return true;
}

bool CWriter::setAttribute(const char* sAttributeName, const char* sAttributeValue)
{
	if (sAttributeName == nullptr) { return false; }

	if (sAttributeValue == nullptr) { return false; }

	if (m_bHasChild) { return false; }

	if (m_bHasData) { return false; }

	if (m_bHasClosedOpeningNode) { return false; }

	string l_sAttributeValue(sAttributeValue);
	this->sanitize(l_sAttributeValue);

	string l_sResult = string(" ") + string(sAttributeName) + string("=\"") + string(l_sAttributeValue) + string("\"");
	m_rWriterCallback.write(l_sResult.c_str());
	return true;
}

bool CWriter::closeChild()
{
	if (m_vNodes.empty()) { return false; }

	if (!m_bHasClosedOpeningNode)
	{
		m_rWriterCallback.write(">");
		m_bHasClosedOpeningNode = true;
	}

	string l_sIndent(m_vNodes.size() - 1, '\t');
	string l_sResult = ((m_bHasData || !m_bHasChild) ? string("") : string("\n") + l_sIndent) + string("</") + m_vNodes.top() + string(">");
	m_rWriterCallback.write(l_sResult.c_str());
	m_vNodes.pop();
	m_bHasChild = true;
	m_bHasData  = false;
	return true;
}

void CWriter::release()
{
	while (!m_vNodes.empty()) { closeChild(); }
	delete this;
}

void CWriter::sanitize(string& sString, bool escapeQuotes)
{
	string::size_type i;
	if (sString.length() != 0)
	{
		// mandatory, this one should be the first because the other ones add & symbols
		for (i = sString.find("&", 0); i != string::npos; i = sString.find("&", i + 1))
			sString.replace(i, 1, "&amp;");
		for (i = sString.find("<", 0); i != string::npos; i = sString.find("<", i + 1))
			sString.replace(i, 1, "&lt;");
		for (i = sString.find(">", 0); i != string::npos; i = sString.find(">", i + 1))
			sString.replace(i, 1, "&gt;");

		// Quotes need only be escaped in attributes
		if (escapeQuotes)
		{
			for (i = sString.find("'", 0); i != string::npos; i = sString.find("'", i + 1))
				sString.replace(i, 1, "&apos;");
			for (i = sString.find("\"", 0); i != string::npos; i = sString.find("\"", i + 1))
				sString.replace(i, 1, "&quot;");
		}
	}
}

XML_API IWriter* XML::createWriter(IWriterCallback& rWriterCallback)
{
	return new CWriter(rWriterCallback);
}
