#include "ovCMemoryBuffer.h"

#include <cstring> // memcpy

using namespace OpenViBE;

namespace OpenViBE
{
	namespace
	{
		class CMemoryBufferImpl : public IMemoryBuffer
		{
		public:

			CMemoryBufferImpl(void);
			explicit CMemoryBufferImpl(const IMemoryBuffer& rMemoryBuffer);
			CMemoryBufferImpl(const uint8_t* pMemoryBuffer, uint64_t ui64BufferSize);
			virtual ~CMemoryBufferImpl(void);

			virtual bool reserve(uint64_t ui64Size);
			virtual bool setSize(uint64_t ui64Size, bool bDiscard);
			virtual uint64_t getSize(void) const;
			virtual uint8_t* getDirectPointer(void);
			virtual const uint8_t* getDirectPointer(void) const;
			virtual bool append(const uint8_t* pBuffer, uint64_t ui64BufferSize);
			virtual bool append(const IMemoryBuffer& rMemoryBuffer);

			_IsDerivedFromClass_Final_(IMemoryBuffer, OV_ClassId_MemoryBufferImpl);

		protected:

			uint8_t* m_pBuffer;
			uint64_t m_ui64BufferSize;
			uint64_t m_ui64AllocatedSize;
		};
	};
};

// ________________________________________________________________________________________________________________
//

CMemoryBufferImpl::CMemoryBufferImpl(void)
	: m_pBuffer(NULL)
	  , m_ui64BufferSize(0)
	  , m_ui64AllocatedSize(0) {}

CMemoryBufferImpl::CMemoryBufferImpl(const IMemoryBuffer& rMemoryBuffer)
	: m_pBuffer(NULL)
	  , m_ui64BufferSize(0)
	  , m_ui64AllocatedSize(0)
{
	m_pBuffer = new uint8_t[static_cast<size_t>(rMemoryBuffer.getSize() + 1)]; // $$$
	if (m_pBuffer)
	{
		m_ui64BufferSize    = rMemoryBuffer.getSize();
		m_ui64AllocatedSize = rMemoryBuffer.getSize();
		if (rMemoryBuffer.getDirectPointer())
		{
			memcpy(m_pBuffer, rMemoryBuffer.getDirectPointer(), static_cast<size_t>(m_ui64BufferSize)); // $$$
		}
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
}

CMemoryBufferImpl::CMemoryBufferImpl(const uint8_t* pMemoryBuffer, const uint64_t ui64BufferSize)
	: m_pBuffer(NULL)
	  , m_ui64BufferSize(0)
	  , m_ui64AllocatedSize(0)
{
	m_pBuffer = new uint8_t[static_cast<size_t>(ui64BufferSize + 1)]; // $$$
	if (m_pBuffer)
	{
		m_ui64BufferSize    = ui64BufferSize;
		m_ui64AllocatedSize = ui64BufferSize;
		if (pMemoryBuffer)
		{
			memcpy(m_pBuffer, pMemoryBuffer, static_cast<size_t>(m_ui64BufferSize)); // $$$
		}
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
}

CMemoryBufferImpl::~CMemoryBufferImpl(void)
{
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = NULL;
	}
}

uint8_t* CMemoryBufferImpl::getDirectPointer(void)
{
	return m_pBuffer;
}

const uint8_t* CMemoryBufferImpl::getDirectPointer(void) const
{
	return m_pBuffer;
}

uint64_t CMemoryBufferImpl::getSize(void) const
{
	return m_ui64BufferSize;
}

bool CMemoryBufferImpl::reserve(const uint64_t ui64Size)
{
	if (ui64Size > m_ui64AllocatedSize)
	{
		uint8_t* l_pSavedBuffer = m_pBuffer;
		m_pBuffer             = new uint8_t[static_cast<size_t>(ui64Size + 1)]; // $$$
		if (!m_pBuffer) { return false; }
		memcpy(m_pBuffer, l_pSavedBuffer, static_cast<size_t>(m_ui64BufferSize)); // $$$

		delete [] l_pSavedBuffer;
		m_ui64AllocatedSize            = ui64Size;
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
	return true;
}

bool CMemoryBufferImpl::setSize(
	const uint64_t ui64Size,
	const bool bDiscard)
{
	if (ui64Size > m_ui64AllocatedSize)
	{
		uint8_t* l_pSavedBuffer = m_pBuffer;
		m_pBuffer             = new uint8_t[static_cast<size_t>(ui64Size + 1)]; // $$$
		if (!m_pBuffer) { return false; }
		if (!bDiscard)
		{
			memcpy(m_pBuffer, l_pSavedBuffer, static_cast<size_t>(m_ui64BufferSize)); // $$$
		}
		if (l_pSavedBuffer)
		{
			delete [] l_pSavedBuffer;
		}
		m_ui64AllocatedSize            = ui64Size;
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
	m_ui64BufferSize = ui64Size;
	return true;
}

bool CMemoryBufferImpl::append(const uint8_t* pBuffer, const uint64_t ui64BufferSize)
{
	if (ui64BufferSize != 0)
	{
		uint64_t l_ui64BufferSizeBackup = m_ui64BufferSize;
		if (!this->setSize(m_ui64BufferSize + ui64BufferSize, false)) { return false; }
		memcpy(m_pBuffer + l_ui64BufferSizeBackup, pBuffer, static_cast<size_t>(ui64BufferSize));
	}
	return true;
}

bool CMemoryBufferImpl::append(const IMemoryBuffer& rMemoryBuffer)
{
	if (rMemoryBuffer.getSize() != 0)
	{
		uint64_t l_ui64BufferSizeBackup = m_ui64BufferSize;
		if (!this->setSize(m_ui64BufferSize + rMemoryBuffer.getSize(), false)) { return false; }
		memcpy(m_pBuffer + l_ui64BufferSizeBackup, rMemoryBuffer.getDirectPointer(), static_cast<size_t>(rMemoryBuffer.getSize()));
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

CMemoryBuffer::CMemoryBuffer(void)
	: m_pMemoryBufferImpl(NULL)
{
	m_pMemoryBufferImpl = new CMemoryBufferImpl();
}

CMemoryBuffer::CMemoryBuffer(const IMemoryBuffer& rMemoryBuffer)
	: m_pMemoryBufferImpl(NULL)
{
	m_pMemoryBufferImpl = new CMemoryBufferImpl(rMemoryBuffer);
}

CMemoryBuffer::CMemoryBuffer(const uint8_t* pMemoryBuffer, const uint64_t ui64BufferSize)
	: m_pMemoryBufferImpl(NULL)
{
	m_pMemoryBufferImpl = new CMemoryBufferImpl(pMemoryBuffer, ui64BufferSize);
}

CMemoryBuffer::~CMemoryBuffer(void)
{
	delete m_pMemoryBufferImpl;
}

bool CMemoryBuffer::reserve(const uint64_t ui64Size)
{
	return m_pMemoryBufferImpl->reserve(ui64Size);
}

bool CMemoryBuffer::setSize(const uint64_t ui64Size, const bool bDiscard)
{
	return m_pMemoryBufferImpl->setSize(ui64Size, bDiscard);
}

uint64_t CMemoryBuffer::getSize(void) const
{
	return m_pMemoryBufferImpl->getSize();
}

uint8_t* CMemoryBuffer::getDirectPointer(void)
{
	return m_pMemoryBufferImpl->getDirectPointer();
}

const uint8_t* CMemoryBuffer::getDirectPointer(void) const
{
	return m_pMemoryBufferImpl->getDirectPointer();
}

bool CMemoryBuffer::append(const uint8_t* pBuffer, const uint64_t ui64BufferSize)
{
	return m_pMemoryBufferImpl->append(pBuffer, ui64BufferSize);
}

bool CMemoryBuffer::append(const IMemoryBuffer& rMemoryBuffer)
{
	return m_pMemoryBufferImpl->append(rMemoryBuffer);
}
