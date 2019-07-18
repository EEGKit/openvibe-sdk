#include "ebml/CReader.h"

using namespace EBML;

CReader::CReader(IReaderCallback& rReaderCallback)
	: m_pReaderImplementation(NULL)
{
	m_pReaderImplementation = createReader(rReaderCallback);
}

CReader::~CReader(void)
{
	m_pReaderImplementation->release();
}

bool CReader::processData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pReaderImplementation->processData(pBuffer, ui64BufferSize);
}

CIdentifier CReader::getCurrentNodeIdentifier(void) const
{
	return m_pReaderImplementation->getCurrentNodeIdentifier();
}

uint64_t CReader::getCurrentNodeSize(void) const
{
	return m_pReaderImplementation->getCurrentNodeSize();
}

void CReader::release(void) {}
