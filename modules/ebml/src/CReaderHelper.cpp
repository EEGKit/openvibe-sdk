#include "ebml/CReaderHelper.h"

using namespace EBML;

CReaderHelper::CReaderHelper() { m_pReaderHelperImplementation = createReaderHelper(); }

CReaderHelper::~CReaderHelper() { m_pReaderHelperImplementation->release(); }

uint64_t CReaderHelper::getUIntegerFromChildData(const void* buffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getUIntegerFromChildData(buffer, size);
}

int64_t CReaderHelper::getSIntegerFromChildData(const void* buffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getSIntegerFromChildData(buffer, size);
}

double CReaderHelper::getFloatFromChildData(const void* buffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getFloatFromChildData(buffer, size);
}

const char* CReaderHelper::getASCIIStringFromChildData(const void* buffer, const uint64_t size)
{
	return m_pReaderHelperImplementation->getASCIIStringFromChildData(buffer, size);
}

void CReaderHelper::release() {}
