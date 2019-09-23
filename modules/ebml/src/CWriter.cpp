#include "ebml/CWriter.h"

using namespace EBML;

CWriter::CWriter(IWriterCallback& callback) { m_pWriterImplementation = createWriter(callback); }

CWriter::~CWriter() { m_pWriterImplementation->release(); }

bool CWriter::openChild(const CIdentifier& identifier) { return m_pWriterImplementation->openChild(identifier); }

bool CWriter::setChildData(const void* buffer, const uint64_t size) { return m_pWriterImplementation->setChildData(buffer, size); }

bool CWriter::closeChild() { return m_pWriterImplementation->closeChild(); }

void CWriter::release() {}
