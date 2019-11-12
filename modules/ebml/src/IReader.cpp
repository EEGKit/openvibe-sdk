#include "ebml/IReader.h"
#include <cstring>

#include <cstdio>
#include <cstring>
#include <iostream>

using namespace EBML;
using namespace std;

// ________________________________________________________________________________________________________________
//

#if 0
#define _Debug_ _is_in_debug_mode_(m_totalBytes)
static bool _is_in_debug_mode_(uint64_t ui64Value)
{
	static int i=0;
	// bool result=i++>5500000;
	bool result=ui64Value>29605500;
	if (result) std::cout << "Arround " << ui64Value << std::endl;
	return result;

}
#else
#define _Debug_ false
#endif

// ________________________________________________________________________________________________________________
//

inline bool needsTwoBytesToGetCodedSizeLength(const unsigned char* buffer) { return buffer[0] == 0; }

inline unsigned long getCodedSizeLength(const unsigned char* buffer, unsigned long bufferLength)
{
	unsigned long codedSizeLength;
	if (buffer[0] >> 7) { codedSizeLength = 1; }
	else if (buffer[0] >> 6) { codedSizeLength = 2; }
	else if (buffer[0] >> 5) { codedSizeLength = 3; }
	else if (buffer[0] >> 4) { codedSizeLength = 4; }
	else if (buffer[0] >> 3) { codedSizeLength = 5; }
	else if (buffer[0] >> 2) { codedSizeLength = 6; }
	else if (buffer[0] >> 1) { codedSizeLength = 7; }
	else if (buffer[0]) { codedSizeLength = 8; }
	else
	{
		if (_Debug_ && bufferLength < 2)
		{
			std::cout << "EBML::getCodedSizeLength called with smaller buffer size " << bufferLength << " - needs at least 2" << std::endl;
		}

		if (buffer[1] >> 7) { codedSizeLength = 9; }
		else { codedSizeLength = 10; }
	}
	return codedSizeLength;
}

inline uint64_t getValue(unsigned char* buffer, unsigned long ulBufferLength)
{
	uint64_t result            = 0;
	const unsigned long length = getCodedSizeLength(buffer, ulBufferLength);
	unsigned long ithBit       = length;
	for (unsigned long i = 0; i < length; ++i)
	{
		result = (result << 8) + (buffer[i]);
		result &= ~(ithBit > 0 && ithBit <= 8 ? (1 << (8 - ithBit)) : 0);
		ithBit -= 8;
	}
	return result;
}

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CReaderNode
		{
		public:
			CReaderNode(const CIdentifier& identifier, CReaderNode* pParentNode) : m_ParentNode(pParentNode), m_Id(identifier) { }

		private:
			CReaderNode() = delete;

		public:

			CReaderNode* m_ParentNode = nullptr;
			CIdentifier m_Id;
			uint64_t m_ContentSize     = 0;
			uint64_t m_ReadContentSize = 0;
			unsigned char* m_Buffer       = nullptr;
		};
	} // namespace
} // namespace EBML

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CReader final : public IReader
		{
		public:

			explicit CReader(IReaderCallback& rReaderCallback) : m_readerCB(rReaderCallback) { }
			~CReader() override;
			bool processData(const void* buffer, uint64_t size) override;
			CIdentifier getCurrentNodeIdentifier() const override;
			uint64_t getCurrentNodeSize() const override;
			void release() override;

		protected:

			enum Status
			{
				FillingIdentifier,
				FillingContentSize,
				FillingContent,
			};

			IReaderCallback& m_readerCB;
			CReaderNode* m_currentNode       = nullptr;
			uint64_t m_pendingSize        = 0;
			uint64_t m_nPending       = 0;
			unsigned char* m_pending         = nullptr;
			Status m_status                  = FillingIdentifier;
			Status m_lastStatus              = FillingIdentifier;
			CIdentifier m_currentID  = 0;
			uint64_t m_currentContentSize = 0;
			uint64_t m_totalBytes = 0;
		};
	} // namespace
} // namespace EBML

// ________________________________________________________________________________________________________________
//

CReader::~CReader()
{
	delete [] m_pending;
	while (m_currentNode)
	{
		CReaderNode* parentNode = m_currentNode->m_ParentNode;
		delete m_currentNode;
		m_currentNode = parentNode;
	}
}

bool CReader::processData(const void* buffer, const uint64_t size)
{
	m_totalBytes += size;

	if (_Debug_)
	{
		printf("Received %i byte(s) new buffer :", int(size));
		for (int i = 0; i < int(size) /* && i<4*/; ++i) { printf("[%02X]", ((unsigned char*)buffer)[i]); }
		std::cout << "...\n";
	}

	if (!buffer || !size) { return true; }

	unsigned char* tmpBuffer = (unsigned char*)buffer;
	uint64_t currentSize     = size;
	bool finished            = false;
	while (!finished)
	{
		uint64_t processedPendingBytes = 0;
		uint64_t processedBytes        = 0;
		m_lastStatus                        = m_status;

		if (_Debug_)
		{
			if (m_nPending)
			{
				printf("%i byte(s) pending : ", int(m_nPending));
				for (int i = 0; i < int(m_nPending); ++i) { printf("[%02X]", m_pending[i]); }
				std::cout << "\n";
			}
		}

		// Processes data
		switch (m_status)
		{
			case FillingIdentifier:
			case FillingContentSize:
			{
				if (needsTwoBytesToGetCodedSizeLength(m_nPending ? m_pending : tmpBuffer))
				{
					if (m_nPending + currentSize < 2)
					{
						finished = true;
						break;
					}

					if (m_nPending == 1)
					{
						// assumes (currentSize != 0) because (m_nPending + currentSize >= 2) and (m_nPending == 1)
						m_pending[1] = tmpBuffer[0];
						tmpBuffer++;
						m_nPending++;
						currentSize--;
					}
				}

				const unsigned long codedSizeLength = getCodedSizeLength(m_nPending ? m_pending : tmpBuffer, static_cast<unsigned long>(m_nPending ? m_nPending : currentSize));
				if (codedSizeLength > currentSize + m_nPending) { finished = true; }
				else
				{
					unsigned char* encodedBuffer      = new unsigned char[codedSizeLength];
					const uint64_t pendingBytesToCopy = (codedSizeLength > m_nPending ? m_nPending : codedSizeLength);
					memcpy(encodedBuffer, m_pending, size_t(pendingBytesToCopy));
					memcpy(encodedBuffer + pendingBytesToCopy, tmpBuffer, size_t(codedSizeLength - pendingBytesToCopy));
					const uint64_t value = getValue(encodedBuffer, codedSizeLength);
					delete [] encodedBuffer;
					processedPendingBytes = pendingBytesToCopy;
					processedBytes        = codedSizeLength;

					switch (m_status)
					{
						case FillingIdentifier:
						{
							m_currentID = value;
							m_status            = FillingContentSize;
							if (_Debug_)
							{
								printf("Found identifier 0x%llX - Changing status to FillingContentSize...\n",
									   static_cast<unsigned long long>(m_currentID));
							}
						}
						break;

						case FillingContentSize:
						{
							m_currentContentSize = value;
							if (m_readerCB.isMasterChild(m_currentID))
							{
								m_status = FillingIdentifier;
								if (_Debug_)
								{
									std::cout << "Found content size " << m_currentContentSize << " of master node - Changing status to FillingIdentifier...\n";
								}
							}
							else
							{
								m_status = FillingContent;
								if (_Debug_)
								{
									std::cout << "Found content size " << m_currentContentSize << " of *non* master node - Changing status to FillingContent...\n";
								}
							}
						}
						break;

						case FillingContent:
							// Should never happen - avoids the warning
							break;
					}
				}
			}
				break;

			case FillingContent:
			{
				if (m_currentNode->m_ContentSize == 0)
				{
					m_status = FillingIdentifier;
					if (_Debug_)
					{
						std::cout << "Finished with " << m_currentNode->m_ContentSize << " byte(s) content - Changing status to FillingIdentifier...\n";
					}
					m_readerCB.processChildData(nullptr, 0);
				}
				else
				{
					if (m_currentNode->m_ReadContentSize == 0 && m_currentNode->m_ContentSize <= currentSize)
					{
						m_status = FillingIdentifier;

						processedBytes = m_currentNode->m_ContentSize;
						if (_Debug_)
						{
							std::cout << "Optimized processing of " << m_currentNode->m_ContentSize <<
									" byte(s) content - Changing status to FillingIdentifier...\n";
						}
						m_readerCB.processChildData(tmpBuffer, m_currentNode->m_ContentSize);
					}
					else
					{
						if (m_currentNode->m_ContentSize - m_currentNode->m_ReadContentSize > currentSize)
						{
							memcpy(m_currentNode->m_Buffer + m_currentNode->m_ReadContentSize, tmpBuffer, size_t(currentSize));
							processedBytes = currentSize;
							finished          = true;
						}
						else
						{
							memcpy(m_currentNode->m_Buffer + m_currentNode->m_ReadContentSize, tmpBuffer,
								   size_t(m_currentNode->m_ContentSize - m_currentNode->m_ReadContentSize));
							processedBytes = m_currentNode->m_ContentSize - m_currentNode->m_ReadContentSize;

							m_status = FillingIdentifier;
							if (_Debug_)
							{
								std::cout << "Finished with " << m_currentNode->m_ContentSize <<
										" byte(s) content - Changing status to FillingIdentifier...\n";
							}
							m_readerCB.processChildData(m_currentNode->m_Buffer, m_currentNode->m_ContentSize);
						}
					}
				}
			}
			break;
		}

		// Updates buffer pointer and size
		const uint64_t processedBytesInBuffer = processedBytes - processedPendingBytes;
		tmpBuffer += processedBytesInBuffer;
		currentSize -= processedBytesInBuffer;
		m_nPending -= processedPendingBytes;

		// Updates read size
		CReaderNode* l_pNode = m_currentNode;
		while (l_pNode)
		{
			l_pNode->m_ReadContentSize += processedBytes;
			l_pNode = l_pNode->m_ParentNode;
		}

		// Creates new node when needed
		if (m_status != FillingContentSize && m_lastStatus == FillingContentSize)
		{
			m_currentNode                    = new CReaderNode(m_currentID, m_currentNode);
			m_currentNode->m_ContentSize = m_currentContentSize;
			m_currentNode->m_Buffer         = new unsigned char[uint32_t(m_currentContentSize)];
			m_readerCB.openChild(m_currentNode->m_Id);
		}
		else
		{
			// Closes finished nodes
			while (m_currentNode && (m_currentNode->m_ContentSize == m_currentNode->m_ReadContentSize || m_currentNode->m_ContentSize == 0))
			{
				m_readerCB.closeChild();
				CReaderNode* l_pParentNode = m_currentNode->m_ParentNode;
				delete [] m_currentNode->m_Buffer;
				delete m_currentNode;
				m_currentNode = l_pParentNode;
			}
		}
	}

	// Updates pending data
	if (m_nPending + currentSize > m_pendingSize)
	{
		unsigned char* l_pPending = new unsigned char[uint32_t(m_nPending + currentSize + 1)
		]; // Ugly hack, reserve 1 more byte on pending data so we are sure we can insert this additional pending byte when only one byte is pending and two bytes are needed for decoding identifier and/or buffer size
		memcpy(l_pPending, m_pending, size_t(m_nPending));
		delete [] m_pending;
		m_pending        = l_pPending;
		m_pendingSize = m_nPending + currentSize;
	}
	memcpy(m_pending + m_nPending, tmpBuffer, size_t(currentSize));
	m_nPending += currentSize;

	if (_Debug_) { std::cout << "\n"; }
	return true;
}

CIdentifier CReader::getCurrentNodeIdentifier() const
{
	if (_Debug_) { std::cout << "getCurrentNodeIdentifier : " << m_currentNode << std::endl; }
	return m_currentNode ? m_currentNode->m_Id : CIdentifier();
}

uint64_t CReader::getCurrentNodeSize() const
{
	if (_Debug_) { std::cout << "getCurrentNodeSize : " << m_currentNode->m_ContentSize << std::endl; }
	return m_currentNode ? m_currentNode->m_ContentSize : 0;
}

void CReader::release() { delete this; }

// ________________________________________________________________________________________________________________
//

EBML_API IReader* EBML::createReader(IReaderCallback& rReaderCallback) { return new CReader(rReaderCallback); }
