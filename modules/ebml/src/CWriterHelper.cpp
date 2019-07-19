#include "ebml/CWriterHelper.h"

using namespace EBML;

CWriterHelper::CWriterHelper()
	: m_pWriterHelperImplementation(NULL)
{
	m_pWriterHelperImplementation = createWriterHelper();
}

CWriterHelper::~CWriterHelper()
{
	m_pWriterHelperImplementation->release();
}

bool CWriterHelper::connect(IWriter* pWriter)
{
	return m_pWriterHelperImplementation->connect(pWriter);
}

bool CWriterHelper::disconnect()
{
	return m_pWriterHelperImplementation->disconnect();
}

bool CWriterHelper::openChild(const CIdentifier& rIdentifier)
{
	return m_pWriterHelperImplementation->openChild(rIdentifier);
}

bool CWriterHelper::closeChild()
{
	return m_pWriterHelperImplementation->closeChild();
}

bool CWriterHelper::setSIntegerAsChildData(const int64_t iValue)
{
	return m_pWriterHelperImplementation->setSIntegerAsChildData(iValue);
}

bool CWriterHelper::setUIntegerAsChildData(const uint64_t uiValue)
{
	return m_pWriterHelperImplementation->setUIntegerAsChildData(uiValue);
}

bool CWriterHelper::setFloat32AsChildData(const float fValue)
{
	return m_pWriterHelperImplementation->setFloat32AsChildData(fValue);
}

bool CWriterHelper::setFloat64AsChildData(const double fValue)
{
	return m_pWriterHelperImplementation->setFloat64AsChildData(fValue);
}

bool CWriterHelper::setBinaryAsChildData(const void* pBuffer, const uint64_t ui64BufferLength)
{
	return m_pWriterHelperImplementation->setBinaryAsChildData(pBuffer, ui64BufferLength);
}

bool CWriterHelper::setASCIIStringAsChildData(const char* sValue)
{
	return m_pWriterHelperImplementation->setASCIIStringAsChildData(sValue);
}

void CWriterHelper::release() {}
