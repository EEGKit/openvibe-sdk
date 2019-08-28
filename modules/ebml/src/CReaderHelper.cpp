#include "ebml/CReaderHelper.h"

using namespace EBML;

CReaderHelper::CReaderHelper(){
	m_pReaderHelperImplementation = createReaderHelper();
}

CReaderHelper::~CReaderHelper() { m_pReaderHelperImplementation->release(); }

uint64_t CReaderHelper::getUIntegerFromChildData(const void* pBuffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getUIntegerFromChildData(pBuffer, size);
}

int64_t CReaderHelper::getSIntegerFromChildData(const void* pBuffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getSIntegerFromChildData(pBuffer, size);
}

double CReaderHelper::getFloatFromChildData(const void* pBuffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getFloatFromChildData(pBuffer, size);
}

const char* CReaderHelper::getASCIIStringFromChildData(const void* pBuffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getASCIIStringFromChildData(pBuffer, size);
}

void CReaderHelper::release() {}
