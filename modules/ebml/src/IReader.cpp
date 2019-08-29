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

inline bool needsTwoBytesToGetCodedSizeLength(unsigned char* buffer) { return buffer[0] == 0; }

inline unsigned long getCodedSizeLength(unsigned char* buffer, unsigned long ulBufferLength)
{
	unsigned long l_ulCodedSizeLength;
	if (buffer[0] >> 7) { l_ulCodedSizeLength = 1; }
	else if (buffer[0] >> 6) { l_ulCodedSizeLength = 2; }
	else if (buffer[0] >> 5) { l_ulCodedSizeLength = 3; }
	else if (buffer[0] >> 4) { l_ulCodedSizeLength = 4; }
	else if (buffer[0] >> 3) { l_ulCodedSizeLength = 5; }
	else if (buffer[0] >> 2) { l_ulCodedSizeLength = 6; }
	else if (buffer[0] >> 1) { l_ulCodedSizeLength = 7; }
	else if (buffer[0]) { l_ulCodedSizeLength = 8; }
	else
	{
		if (_Debug_ && ulBufferLength < 2)
		{
			std::cout << "EBML::getCodedSizeLength called with smaller buffer size " << ulBufferLength << " - needs at least 2" << std::endl;
		}

		if (buffer[1] >> 7) { l_ulCodedSizeLength = 9; }
		else { l_ulCodedSizeLength = 10; }
	}
	return l_ulCodedSizeLength;
}

inline uint64_t getValue(unsigned char* buffer, unsigned long ulBufferLength)
{
	uint64_t result            = 0;
	const unsigned long length = getCodedSizeLength(buffer, ulBufferLength);
	unsigned long ithBit = length;
	for (unsigned long i = 0; i < length; i++)
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
		class CReader final : public IReader
		{
		public:

			explicit CReader(IReaderCallback& rReaderCallback);
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

bool CReader::processData(const void* buffer, const uint64_t size)
{
	m_ui64TotalBytes += size;

	if (_Debug_)
	{
		printf("Received %i byte(s) new buffer :", int(size));
		for (int i = 0; i < int(size) /* && i<4*/; i++)
		{
			printf("[%02X]", ((unsigned char*)buffer)[i]);
		}
		std::cout << "...\n";
	}

	if (!buffer || !size) { return true; }

	unsigned char* tmpBuffer  = (unsigned char*)buffer;
	uint64_t currentSize = size;
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
				printf("%i byte(s) pending : ", int(m_ui64PendingCount));
				for (int i = 0; i < int(m_ui64PendingCount); i++){
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
				if (needsTwoBytesToGetCodedSizeLength(m_ui64PendingCount ? m_pPending : tmpBuffer))
				{
					if (m_ui64PendingCount + currentSize < 2)
					{
						l_bFinished = true;
						break;
					}

					if (m_ui64PendingCount == 1)
					{
						// assumes (currentSize != 0) because (m_ui64PendingCount + currentSize >= 2) and (m_ui64PendingCount == 1)
						m_pPending[1] = tmpBuffer[0];
						tmpBuffer++;
						m_ui64PendingCount++;
						currentSize--;
					}
				}

				const unsigned long codedSizeLength = getCodedSizeLength(m_ui64PendingCount ? m_pPending : tmpBuffer, 
																		 static_cast<unsigned long>(m_ui64PendingCount ? m_ui64PendingCount : currentSize));
				if (codedSizeLength > currentSize + m_ui64PendingCount)
				{
					l_bFinished = true;
				}
				else
				{
					unsigned char* encodedBuffer   = new unsigned char[codedSizeLength];
					const uint64_t pendingBytesToCopy = (codedSizeLength > m_ui64PendingCount ? m_ui64PendingCount : codedSizeLength);
					memcpy(encodedBuffer, m_pPending, size_t(pendingBytesToCopy));
					memcpy(encodedBuffer + pendingBytesToCopy, tmpBuffer, size_t(codedSizeLength - pendingBytesToCopy));
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
								printf("Found identifier 0x%llX - Changing status to FillingContentSize...\n", static_cast<unsigned long long>(m_oCurrentIdentifier));
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
					if (m_pCurrentNode->m_ui64ReadContentSize == 0 && m_pCurrentNode->m_ui64ContentSize <= currentSize)
					{
						m_eStatus = FillingIdentifier;

						l_ui64ProcessedBytes = m_pCurrentNode->m_ui64ContentSize;
						if (_Debug_)
						{
							std::cout << "Optimized processing of " << m_pCurrentNode->m_ui64ContentSize << " byte(s) content - Changing status to FillingIdentifier...\n";
						}
						m_rReaderCallback.processChildData(tmpBuffer, m_pCurrentNode->m_ui64ContentSize);
					}
					else
					{
						if (m_pCurrentNode->m_ui64ContentSize - m_pCurrentNode->m_ui64ReadContentSize > currentSize)
						{
							memcpy(m_pCurrentNode->m_pBuffer + m_pCurrentNode->m_ui64ReadContentSize, tmpBuffer, size_t(currentSize));
							l_ui64ProcessedBytes = currentSize;
							l_bFinished          = true;
						}
						else
						{
							memcpy(m_pCurrentNode->m_pBuffer + m_pCurrentNode->m_ui64ReadContentSize, tmpBuffer, size_t(m_pCurrentNode->m_ui64ContentSize - m_pCurrentNode->m_ui64ReadContentSize));
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
		tmpBuffer += processedBytesInBuffer;
		currentSize -= processedBytesInBuffer;
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
			m_pCurrentNode->m_pBuffer         = new unsigned char[static_cast<unsigned int>(m_ui64CurrentContentSize)];
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
	if (m_ui64PendingCount + currentSize > m_ui64PendingSize)
	{
		unsigned char* l_pPending = new unsigned char[static_cast<unsigned int>(m_ui64PendingCount + currentSize + 1)]; // Ugly hack, reserve 1 more byte on pending data so we are sure we can insert this additional pending byte when only one byte is pending and two bytes are needed for decoding identifier and/or buffer size
		memcpy(l_pPending, m_pPending, size_t(m_ui64PendingCount));
		delete [] m_pPending;
		m_pPending        = l_pPending;
		m_ui64PendingSize = m_ui64PendingCount + currentSize;
	}
	memcpy(m_pPending + m_ui64PendingCount, tmpBuffer, size_t(currentSize));
	m_ui64PendingCount += currentSize;

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
