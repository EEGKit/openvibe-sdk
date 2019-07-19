#include "ebml/CWriter.h"

using namespace EBML;

CWriter::CWriter(IWriterCallback& rWriterCallback)
	: m_pWriterImplementation(NULL)
{
	m_pWriterImplementation = createWriter(rWriterCallback);
}

CWriter::~CWriter()
{
	m_pWriterImplementation->release();
}

bool CWriter::openChild(const CIdentifier& rIdentifier)
{
	return m_pWriterImplementation->openChild(rIdentifier);
}

bool CWriter::setChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pWriterImplementation->setChildData(pBuffer, ui64BufferSize);
}

bool CWriter::closeChild()
{
	return m_pWriterImplementation->closeChild();
}

void CWriter::release() {}
