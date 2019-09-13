#include "ebml/IWriter.h"

#include <vector>
#include <cstdlib>
#include <cstring>

using namespace EBML;
using namespace std;

// ________________________________________________________________________________________________________________
//

inline size_t getCodedSizeLength(const uint64_t uiValue)
{
	size_t codeSizeLength;
	if (uiValue < 0x000000000000007fLL) { codeSizeLength = 1; }
	else if (uiValue < 0x0000000000003fffLL) { codeSizeLength = 2; }
	else if (uiValue < 0x00000000001fffffLL) { codeSizeLength = 3; }
	else if (uiValue < 0x000000000fffffffLL) { codeSizeLength = 4; }
	else if (uiValue < 0x00000007ffffffffLL) { codeSizeLength = 5; }
	else if (uiValue < 0x000003ffffffffffLL) { codeSizeLength = 6; }
	else if (uiValue < 0x0001ffffffffffffLL) { codeSizeLength = 7; }
	else if (uiValue < 0x00ffffffffffffffLL) { codeSizeLength = 8; }
	else if (uiValue < 0x7fffffffffffffffLL) { codeSizeLength = 9; }
	else { codeSizeLength = 10; }

	return codeSizeLength;
}

inline bool getCodedBuffer(const uint64_t uiValue, unsigned char* buffer, uint64_t* pBufferLength)
{
	const size_t codeSizeLength = getCodedSizeLength(uiValue);

	if (codeSizeLength > *pBufferLength) { return false; }

	size_t l_ulIthBit = codeSizeLength;
	for (size_t i = 0; i < codeSizeLength; i++)
	{
		const size_t l_ulByteShift = codeSizeLength - i - 1;
		size_t l_ulByte            = (l_ulByteShift >= 8 ? 0 : static_cast<unsigned char>((uiValue >> (l_ulByteShift * 8)) & 0xff));
		l_ulByte |= (l_ulIthBit > 0 && l_ulIthBit <= 8 ? (1 << (8 - l_ulIthBit)) : 0);
		l_ulIthBit -= 8;

		buffer[i] = static_cast<unsigned char>(l_ulByte);
	}

	*pBufferLength = codeSizeLength;
	return true;
}

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CWriterNode final
		{
		public:

			CWriterNode(const CIdentifier& identifier, CWriterNode* pParentNode);
			~CWriterNode();
			void process(IWriterCallback& rWriterCallback);

		protected:

			uint64_t getTotalContentSize(bool bCountIdentifierAndSize);

		private:

			CWriterNode();

		public:

			CIdentifier m_oIdentifier;
			CWriterNode* m_pParentNode;
			uint64_t m_ui64BufferLength = 0;
			unsigned char* m_pBuffer    = nullptr;
			bool m_bBuffered            = false;
			vector<CWriterNode*> m_vChildren;
		};
	} // namespace
} // namespace EBML

// ________________________________________________________________________________________________________________
//

CWriterNode::CWriterNode(const CIdentifier& identifier, CWriterNode* pParentNode)
	: m_oIdentifier(identifier), m_pParentNode(pParentNode) {}

CWriterNode::~CWriterNode()
{
	for (auto i = m_vChildren.begin(); i != m_vChildren.end(); ++i) { delete (*i); }

	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = nullptr;
	}
}

void CWriterNode::process(IWriterCallback& rWriterCallback)
{
	unsigned char id[10];
	unsigned char pContentSize[10];
	uint64_t contentSizeLength = sizeof(pContentSize);
	uint64_t identifierLength  = sizeof(id);
	const uint64_t contentSize = getTotalContentSize(false);

	if (!getCodedBuffer(contentSize, pContentSize, &contentSizeLength))
	{
		// SHOULD NEVER HAPPEN
	}

	if (!getCodedBuffer(m_oIdentifier, id, &identifierLength))
	{
		// SHOULD NEVER HAPPEN
	}

	rWriterCallback.write(id, identifierLength);
	rWriterCallback.write(pContentSize, contentSizeLength);

	if (m_vChildren.empty()) { rWriterCallback.write(m_pBuffer, m_ui64BufferLength); }
	else { for (auto i = m_vChildren.begin(); i != m_vChildren.end(); ++i) { (*i)->process(rWriterCallback); } }
}

uint64_t CWriterNode::getTotalContentSize(bool bCountIdentifierAndSize)
{
	uint64_t contentSize = 0;
	if (m_vChildren.empty()) { contentSize = m_ui64BufferLength; }
	else { for (auto i = m_vChildren.begin(); i != m_vChildren.end(); ++i) { contentSize += (*i)->getTotalContentSize(true); } }

	uint64_t l_ui64Result = contentSize;
	if (bCountIdentifierAndSize)
	{
		l_ui64Result += getCodedSizeLength(m_oIdentifier);
		l_ui64Result += getCodedSizeLength(contentSize);
	}

	return l_ui64Result;
}

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CWriter final : public IWriter
		{
		public:

			explicit CWriter(IWriterCallback& rWriterCallback) : m_rWriterCallback(rWriterCallback) {}
			bool openChild(const CIdentifier& identifier) override;
			bool setChildData(const void* buffer, const uint64_t size) override;
			bool closeChild() override;
			void release() override;

		protected:

			CWriterNode* m_pCurrentNode = nullptr;
			IWriterCallback& m_rWriterCallback;

		private:
			CWriter() = delete;
		};
	} // namespace
} // namespace EBML

// ________________________________________________________________________________________________________________
//

bool CWriter::openChild(const CIdentifier& identifier)
{
	if (m_pCurrentNode) { if (m_pCurrentNode->m_bBuffered) { return false; } }

	CWriterNode* pResult = new CWriterNode(identifier, m_pCurrentNode);
	if (m_pCurrentNode) { m_pCurrentNode->m_vChildren.push_back(pResult); }
	m_pCurrentNode = pResult;
	return true;
}

bool CWriter::setChildData(const void* buffer, const uint64_t size)
{
	if (!m_pCurrentNode) { return false; }

	if (!m_pCurrentNode->m_vChildren.empty()) { return false; }

	unsigned char* bufferCopy = nullptr;
	if (size)
	{
		if (!buffer) { return false; }
		bufferCopy = new unsigned char[static_cast<unsigned int>(size)];
		if (!bufferCopy) { return false; }
		memcpy(bufferCopy, buffer, size_t(size));
	}

	delete [] m_pCurrentNode->m_pBuffer;

	m_pCurrentNode->m_ui64BufferLength = size;
	m_pCurrentNode->m_pBuffer          = bufferCopy;
	m_pCurrentNode->m_bBuffered        = true;
	return true;
}

bool CWriter::closeChild()
{
	if (!m_pCurrentNode) { return false; }

	if ((!m_pCurrentNode->m_bBuffered) && (!m_pCurrentNode->m_vChildren.size()))
	{
		m_pCurrentNode->m_ui64BufferLength = 0;
		m_pCurrentNode->m_pBuffer          = nullptr;
		m_pCurrentNode->m_bBuffered        = true;
	}

	CWriterNode* l_pParentNode = m_pCurrentNode->m_pParentNode;
	if (!l_pParentNode)
	{
		m_pCurrentNode->process(m_rWriterCallback);
		delete m_pCurrentNode;
	}

	m_pCurrentNode = l_pParentNode;
	return true;
}

void CWriter::release()
{
	while (m_pCurrentNode) { closeChild(); }
	delete this;
}

// ________________________________________________________________________________________________________________
//

EBML_API IWriter* EBML::createWriter(IWriterCallback& rWriterCallback) { return new CWriter(rWriterCallback); }
