#include "ebml/CReader.h"

using namespace EBML;

CReader::CReader(IReaderCallback& rReaderCallback)
	: m_pReaderImplementation(nullptr)
{
	m_pReaderImplementation = createReader(rReaderCallback);
}

CReader::~CReader()
{
	m_pReaderImplementation->release();
}

bool CReader::processData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pReaderImplementation->processData(pBuffer, ui64BufferSize);
}

CIdentifier CReader::getCurrentNodeIdentifier() const
{
	return m_pReaderImplementation->getCurrentNodeIdentifier();
}

uint64_t CReader::getCurrentNodeSize() const
{
	return m_pReaderImplementation->getCurrentNodeSize();
}

void CReader::release() {}
