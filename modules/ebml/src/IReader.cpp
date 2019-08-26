#include "ebml/IReader.h"
#include <cstring>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace EBML;

// ________________________________________________________________________________________________________________
//

#if 0
#define _Debug_ _is_in_debug_mode_(m_ui64TotalBytes)
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

inline bool needsTwoBytesToGetCodedSizeLength(unsigned char* pBuffer) { return pBuffer[0] == 0; }

inline unsigned long getCodedSizeLength(unsigned char* pBuffer, unsigned long ulBufferLength)
{
	unsigned long l_ulCodedSizeLength;
	if (pBuffer[0] >> 7) { l_ulCodedSizeLength = 1; }
	else if (pBuffer[0] >> 6) { l_ulCodedSizeLength = 2; }
	else if (pBuffer[0] >> 5) { l_ulCodedSizeLength = 3; }
	else if (pBuffer[0] >> 4) { l_ulCodedSizeLength = 4; }
	else if (pBuffer[0] >> 3) { l_ulCodedSizeLength = 5; }
	else if (pBuffer[0] >> 2) { l_ulCodedSizeLength = 6; }
	else if (pBuffer[0] >> 1) { l_ulCodedSizeLength = 7; }
	else if (pBuffer[0]) { l_ulCodedSizeLength = 8; }
	else
	{
		if (_Debug_ && ulBufferLength < 2)
		{
			std::cout << "EBML::getCodedSizeLength called with smaller buffer size " << ulBufferLength << " - needs at least 2" << std::endl;
		}

		if (pBuffer[1] >> 7) { l_ulCodedSizeLength = 9; }
		else { l_ulCodedSizeLength = 10; }
	}
	return l_ulCodedSizeLength;
}

inline uint64_t getValue(unsigned char* pBuffer, unsigned long ulBufferLength)
{
	uint64_t result            = 0;
	const unsigned long length = getCodedSizeLength(pBuffer, ulBufferLength);
	unsigned long ithBit = length;
	for (unsigned long i = 0; i < length; i++)
	{
		result = (result << 8) + (pBuffer[i]);
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
			CReaderNode(const CIdentifier& rIdentifier, CReaderNode* pParentNode);

		private:
			CReaderNode();

		public:

			CReaderNode* m_pParentNode = nullptr;
			CIdentifier m_oIdentifier;
			uint64_t m_ui64ContentSize = 0;
			uint64_t m_ui64ReadContentSize = 0;
			unsigned char* m_pBuffer = nullptr;
			//			bool m_bBufferShouldBeDeleted;
		};
	} // namespace
} // namespace EBML

CReaderNode::CReaderNode(const CIdentifier& rIdentifier, CReaderNode* pParentNode)
	: m_pParentNode(pParentNode), m_oIdentifier(rIdentifier) { }

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CReader : public IReader
		{
		public:

			explicit CReader(IReaderCallback& rReaderCallback);
			~CReader() override;
			bool processData(const void* pBuffer, uint64_t ui64BufferSize) override;
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

			IReaderCallback& m_rReaderCallback;
			CReaderNode* m_pCurrentNode = nullptr;
			uint64_t m_ui64PendingSize = 0;
			uint64_t m_ui64PendingCount = 0;
			unsigned char* m_pPending = nullptr;
			Status m_eStatus = FillingIdentifier;
			Status m_eLastStatus;
			CIdentifier m_oCurrentIdentifier;
			uint64_t m_ui64CurrentContentSize = 0;

			uint64_t m_ui64TotalBytes = 0;
		};
	} // namespace
} // namespace EBML

// ________________________________________________________________________________________________________________
//

CReader::CReader(IReaderCallback& rReaderCallback) : m_rReaderCallback(rReaderCallback), m_oCurrentIdentifier(0) { }

CReader::~CReader()
{
	delete [] m_pPending;
	while (m_pCurrentNode)
	{
		CReaderNode* l_pParentNode = m_pCurrentNode->m_pParentNode;
		delete m_pCurrentNode;
		m_pCurrentNode = l_pParentNode;
	}
}

bool CReader::processData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	m_ui64TotalBytes += ui64BufferSize;

	if (_Debug_)
	{
		printf("Received %i byte(s) new buffer :", int(ui64BufferSize));
		for (int i = 0; i < int(ui64BufferSize) /* && i<4*/; i++)
		{
			printf("[%02X]", ((unsigned char*)pBuffer)[i]);
		}
		std::cout << "...\n";
	}

	if (!pBuffer || !ui64BufferSize) { return true; }

	unsigned char* l_pBuffer  = (unsigned char*)pBuffer;
	uint64_t l_ui64BufferSize = ui64BufferSize;
	bool l_bFinished          = false;
	while (!l_bFinished)
	{
		uint64_t l_ui64ProcessedPendingBytes = 0;
		uint64_t l_ui64ProcessedBytes        = 0;
		m_eLastStatus                        = m_eStatus;

		if (_Debug_)
		{
			if (m_ui64PendingCount)
			{
				printf("%i byte(s) pending : ", (int)m_ui64PendingCount);
				for (int i = 0; i < (int)m_ui64PendingCount; i++){
					printf("[%02X]", m_pPending[i]);
				}
				std::cout << "\n";
			}
		}

		// Processes data
		switch (m_eStatus)
		{
			case FillingIdentifier:
			case FillingContentSize:
			{
				if (needsTwoBytesToGetCodedSizeLength(m_ui64PendingCount ? m_pPending : l_pBuffer))
				{
					if (m_ui64PendingCount + l_ui64BufferSize < 2)
					{
						l_bFinished = true;
						break;
					}

					if (m_ui64PendingCount == 1)
					{
						// assumes (l_ui64BufferSize != 0) because (m_ui64PendingCount + l_ui64BufferSize >= 2) and (m_ui64PendingCount == 1)
						m_pPending[1] = l_pBuffer[0];
						l_pBuffer++;
						m_ui64PendingCount++;
						l_ui64BufferSize--;
					}
				}

				const unsigned long codedSizeLength = getCodedSizeLength(m_ui64PendingCount ? m_pPending : l_pBuffer,
																	   (unsigned long)(m_ui64PendingCount ? m_ui64PendingCount : l_ui64BufferSize));
				if (codedSizeLength > l_ui64BufferSize + m_ui64PendingCount)
				{
					l_bFinished = true;
				}
				else
				{
					unsigned char* encodedBuffer   = new unsigned char[codedSizeLength];
					const uint64_t pendingBytesToCopy = (codedSizeLength > m_ui64PendingCount ? m_ui64PendingCount : codedSizeLength);
					memcpy(encodedBuffer, m_pPending, size_t(pendingBytesToCopy));
					memcpy(encodedBuffer + pendingBytesToCopy, l_pBuffer, size_t(codedSizeLength - pendingBytesToCopy));
					const uint64_t value = getValue(encodedBuffer, codedSizeLength);
					delete [] encodedBuffer;
					l_ui64ProcessedPendingBytes = pendingBytesToCopy;
					l_ui64ProcessedBytes        = codedSizeLength;

					switch (m_eStatus)
					{
						case FillingIdentifier:
						{
							m_oCurrentIdentifier = value;
							m_eStatus            = FillingContentSize;
							if (_Debug_)
							{
								printf("Found identifier 0x%llX - Changing status to FillingContentSize...\n", (unsigned long long)m_oCurrentIdentifier);
							}
						}
						break;

						case FillingContentSize:
						{
							m_ui64CurrentContentSize = value;
							if (m_rReaderCallback.isMasterChild(m_oCurrentIdentifier))
							{
								m_eStatus = FillingIdentifier;
								if (_Debug_)
								{
									std::cout << "Found content size " << m_ui64CurrentContentSize << " of master node - Changing status to FillingIdentifier...\n";
								}
							}
							else
							{
								m_eStatus = FillingContent;
								if (_Debug_)
								{
									std::cout << "Found content size " << m_ui64CurrentContentSize << " of *non* master node - Changing status to FillingContent...\n";
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
				if (m_pCurrentNode->m_ui64ContentSize == 0)
				{
					m_eStatus = FillingIdentifier;
					if (_Debug_)
					{
						std::cout << "Finished with " << m_pCurrentNode->m_ui64ContentSize << " byte(s) content - Changing status to FillingIdentifier...\n";
					}
					m_rReaderCallback.processChildData(nullptr, 0);
				}
				else
				{
					if (m_pCurrentNode->m_ui64ReadContentSize == 0 && m_pCurrentNode->m_ui64ContentSize <= l_ui64BufferSize)
					{
						m_eStatus = FillingIdentifier;

						l_ui64ProcessedBytes = m_pCurrentNode->m_ui64ContentSize;
						if (_Debug_)
						{
							std::cout << "Optimized processing of " << m_pCurrentNode->m_ui64ContentSize << " byte(s) content - Changing status to FillingIdentifier...\n";
						}
						m_rReaderCallback.processChildData(l_pBuffer, m_pCurrentNode->m_ui64ContentSize);
					}
					else
					{
						if (m_pCurrentNode->m_ui64ContentSize - m_pCurrentNode->m_ui64ReadContentSize > l_ui64BufferSize)
						{
							memcpy(m_pCurrentNode->m_pBuffer + m_pCurrentNode->m_ui64ReadContentSize, l_pBuffer, size_t(l_ui64BufferSize));
							l_ui64ProcessedBytes = l_ui64BufferSize;
							l_bFinished          = true;
						}
						else
						{
							memcpy(m_pCurrentNode->m_pBuffer + m_pCurrentNode->m_ui64ReadContentSize, l_pBuffer, size_t(m_pCurrentNode->m_ui64ContentSize - m_pCurrentNode->m_ui64ReadContentSize));
							l_ui64ProcessedBytes = m_pCurrentNode->m_ui64ContentSize - m_pCurrentNode->m_ui64ReadContentSize;

							m_eStatus = FillingIdentifier;
							if (_Debug_)
							{
								std::cout << "Finished with " << m_pCurrentNode->m_ui64ContentSize << " byte(s) content - Changing status to FillingIdentifier...\n";
							}
							m_rReaderCallback.processChildData(m_pCurrentNode->m_pBuffer, m_pCurrentNode->m_ui64ContentSize);
						}
					}
				}
			}
			break;
		}

		// Updates buffer pointer and size
		const uint64_t processedBytesInBuffer = l_ui64ProcessedBytes - l_ui64ProcessedPendingBytes;
		l_pBuffer += processedBytesInBuffer;
		l_ui64BufferSize -= processedBytesInBuffer;
		m_ui64PendingCount -= l_ui64ProcessedPendingBytes;

		// Updates read size
		CReaderNode* l_pNode = m_pCurrentNode;
		while (l_pNode)
		{
			l_pNode->m_ui64ReadContentSize += l_ui64ProcessedBytes;
			l_pNode = l_pNode->m_pParentNode;
		}

		// Creates new node when needed
		if (m_eStatus != FillingContentSize && m_eLastStatus == FillingContentSize)
		{
			m_pCurrentNode                    = new CReaderNode(m_oCurrentIdentifier, m_pCurrentNode);
			m_pCurrentNode->m_ui64ContentSize = m_ui64CurrentContentSize;
			m_pCurrentNode->m_pBuffer         = new unsigned char[(unsigned int)(m_ui64CurrentContentSize)];
			m_rReaderCallback.openChild(m_pCurrentNode->m_oIdentifier);
		}
		else
		{
			// Closes finished nodes
			while (m_pCurrentNode && (m_pCurrentNode->m_ui64ContentSize == m_pCurrentNode->m_ui64ReadContentSize || m_pCurrentNode->m_ui64ContentSize == 0))
			{
				m_rReaderCallback.closeChild();
				CReaderNode* l_pParentNode = m_pCurrentNode->m_pParentNode;
				delete [] m_pCurrentNode->m_pBuffer;
				delete m_pCurrentNode;
				m_pCurrentNode = l_pParentNode;
			}
		}
	}

	// Updates pending data
	if (m_ui64PendingCount + l_ui64BufferSize > m_ui64PendingSize)
	{
		unsigned char* l_pPending = new unsigned char[(unsigned int)(m_ui64PendingCount + l_ui64BufferSize + 1)]; // Ugly hack, reserve 1 more byte on pending data so we are sure we can insert this additional pending byte when only one byte is pending and two bytes are needed for decoding identifier and/or buffer size
		memcpy(l_pPending, m_pPending, size_t(m_ui64PendingCount));
		delete [] m_pPending;
		m_pPending        = l_pPending;
		m_ui64PendingSize = m_ui64PendingCount + l_ui64BufferSize;
	}
	memcpy(m_pPending + m_ui64PendingCount, l_pBuffer, size_t(l_ui64BufferSize));
	m_ui64PendingCount += l_ui64BufferSize;

	if (_Debug_) { std::cout << "\n"; }
	return true;
}

CIdentifier CReader::getCurrentNodeIdentifier() const
{
	if (_Debug_) { std::cout << "getCurrentNodeIdentifier : " << m_pCurrentNode << std::endl; }
	return m_pCurrentNode ? m_pCurrentNode->m_oIdentifier : CIdentifier();
}

uint64_t CReader::getCurrentNodeSize() const
{
	if (_Debug_) { std::cout << "getCurrentNodeSize : " << m_pCurrentNode->m_ui64ContentSize << std::endl; }
	return m_pCurrentNode ? m_pCurrentNode->m_ui64ContentSize : 0;
}

void CReader::release() { delete this; }

// ________________________________________________________________________________________________________________
//

EBML_API IReader* EBML::createReader(IReaderCallback& rReaderCallback) { return new CReader(rReaderCallback); }
