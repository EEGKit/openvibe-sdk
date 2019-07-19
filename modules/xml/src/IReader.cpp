#include "IReader.h"

#include <expat.h>
#include <string>
#include <iostream>

using namespace XML;
using namespace std;

namespace XML
{
	class CReader : public IReader
	{
	public:
		explicit CReader(IReaderCallback& rReaderCallback);

		virtual bool processData(const void* pBuffer, uint64_t ui64BufferSize);
		virtual void release();

		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount);
		virtual void processChildData(const char* sData);
		virtual void closeChild();

	protected:

		IReaderCallback& m_rReaderCallback;
		XML_Parser m_pXMLParser;
		std::string m_sData;
	};

	static void XMLCALL expat_xml_start(void* pData, const char* pElement, const char** ppAttribute);
	static void XMLCALL expat_xml_end(void* pData, const char* pElement);
	static void XMLCALL expat_xml_data(void* pData, const char* pDataValue, int iDataLength);
};

CReader::CReader(IReaderCallback& rReaderCallback)
	: m_rReaderCallback(rReaderCallback)
	  , m_pXMLParser(NULL)
{
	m_pXMLParser = XML_ParserCreate(NULL);
	XML_SetElementHandler(m_pXMLParser, expat_xml_start, expat_xml_end);
	XML_SetCharacterDataHandler(m_pXMLParser, expat_xml_data);
	XML_SetUserData(m_pXMLParser, this);
}

bool CReader::processData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	// $$$ TODO take 64bits size into consideration
	XML_Status l_eStatus = XML_Parse(
		m_pXMLParser,
		static_cast<const char*>(pBuffer),
		static_cast<const int>(ui64BufferSize),
		false);
	if (l_eStatus != XML_STATUS_OK)
	{
		XML_Error l_oErrorCode = XML_GetErrorCode(m_pXMLParser);
		// Although printf() is not too elegant, this component has no context to use e.g. LogManager -> printf() is better than a silent fail.
		std::cout << "processData(): expat error " << l_oErrorCode << " on the line " << XML_GetCurrentLineNumber(m_pXMLParser) << " of the input .xml\n";
	}

	return (l_eStatus == XML_STATUS_OK);
}

void CReader::release()
{
	XML_ParserFree(m_pXMLParser);
	delete this;
}

void CReader::openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount)
{
	m_rReaderCallback.openChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount);
	m_sData = "";
}

void CReader::processChildData(const char* sData)
{
	m_sData += sData;
}

void CReader::closeChild()
{
	if (m_sData.size() != 0)
	{
		m_rReaderCallback.processChildData(m_sData.c_str());
	}
	m_sData = "";
	m_rReaderCallback.closeChild();
}

XML_API IReader* XML::createReader(IReaderCallback& rReaderCallback)
{
	return new CReader(rReaderCallback);
}

static void XMLCALL XML::expat_xml_start(void* pData, const char* pElement, const char** ppAttribute)
{
	uint64_t i, l_ui64AttributeCount = 0;
	while (ppAttribute[l_ui64AttributeCount++]);
	l_ui64AttributeCount >>= 1;

	// $$$ TODO take 64bits size into consideration
	const char** l_pAttributeName  = new const char*[static_cast<size_t>(l_ui64AttributeCount)];
	const char** l_pAttributeValue = new const char*[static_cast<size_t>(l_ui64AttributeCount)];

	for (i = 0; i < l_ui64AttributeCount; i++)
	{
		l_pAttributeName[i]  = ppAttribute[(i << 1)];
		l_pAttributeValue[i] = ppAttribute[(i << 1) + 1];
	}

	static_cast<CReader*>(pData)->openChild(pElement, l_pAttributeName, l_pAttributeValue, l_ui64AttributeCount);

	delete [] l_pAttributeName;
	delete [] l_pAttributeValue;
}

static void XMLCALL XML::expat_xml_end(void* pData, const char* pElement)
{
	static_cast<CReader*>(pData)->closeChild();
}

static void XMLCALL XML::expat_xml_data(void* pData, const char* pDataValue, int iDataLength)
{
	string sData(pDataValue, iDataLength);
	static_cast<CReader*>(pData)->processChildData(sData.c_str());
}
