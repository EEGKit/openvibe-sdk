#include "ebml/CReader.h"

using namespace EBML;

CReader::CReader(IReaderCallback& rReaderCallback) { m_pReaderImplementation = createReader(rReaderCallback); }

CReader::~CReader() { m_pReaderImplementation->release(); }

bool CReader::processData(const void* buffer, const uint64_t size) { return m_pReaderImplementation->processData(buffer, size); }

CIdentifier CReader::getCurrentNodeIdentifier() const { return m_pReaderImplementation->getCurrentNodeIdentifier(); }

uint64_t CReader::getCurrentNodeSize() const { return m_pReaderImplementation->getCurrentNodeSize(); }

void CReader::release() {}
