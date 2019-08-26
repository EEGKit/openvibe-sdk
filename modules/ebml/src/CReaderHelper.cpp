#include "ebml/CReaderHelper.h"

using namespace EBML;

CReaderHelper::CReaderHelper(){
	m_pReaderHelperImplementation = createReaderHelper();
}

CReaderHelper::~CReaderHelper() { m_pReaderHelperImplementation->release(); }

uint64_t CReaderHelper::getUIntegerFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pReaderHelperImplementation->getUIntegerFromChildData(pBuffer, ui64BufferSize);
}

int64_t CReaderHelper::getSIntegerFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pReaderHelperImplementation->getSIntegerFromChildData(pBuffer, ui64BufferSize);
}

double CReaderHelper::getFloatFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pReaderHelperImplementation->getFloatFromChildData(pBuffer, ui64BufferSize);
}

const char* CReaderHelper::getASCIIStringFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pReaderHelperImplementation->getASCIIStringFromChildData(pBuffer, ui64BufferSize);
}

void CReaderHelper::release() {}
