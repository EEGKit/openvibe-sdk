#include "ovCMemoryBuffer.h"

#include <cstring> // memcpy

using namespace OpenViBE;

namespace OpenViBE
{
	namespace
	{
		class CMemoryBufferImpl final : public IMemoryBuffer
		{
		public:

			CMemoryBufferImpl();
			explicit CMemoryBufferImpl(const IMemoryBuffer& rMemoryBuffer);
			CMemoryBufferImpl(const uint8_t* pMemoryBuffer, uint64_t size);
			~CMemoryBufferImpl() override;
			bool reserve(const uint64_t ui64Size) override;
			bool setSize(const uint64_t ui64Size, const bool bDiscard) override;
			uint64_t getSize() const override;
			uint8_t* getDirectPointer() override;
			const uint8_t* getDirectPointer() const override;
			bool append(const uint8_t* buffer, const uint64_t size) override;
			bool append(const IMemoryBuffer& rMemoryBuffer) override;

			_IsDerivedFromClass_Final_(IMemoryBuffer, OV_ClassId_MemoryBufferImpl)

		protected:

			uint8_t* m_pBuffer           = nullptr;
			uint64_t m_size              = 0;
			uint64_t m_ui64AllocatedSize = 0;
		};
	} // namespace
} // namespace OpenViBE

// ________________________________________________________________________________________________________________
//

CMemoryBufferImpl::CMemoryBufferImpl() {}

CMemoryBufferImpl::CMemoryBufferImpl(const IMemoryBuffer& rMemoryBuffer)
{
	m_pBuffer = new uint8_t[size_t(rMemoryBuffer.getSize() + 1)]; // $$$
	if (m_pBuffer)
	{
		m_size              = rMemoryBuffer.getSize();
		m_ui64AllocatedSize = rMemoryBuffer.getSize();
		if (rMemoryBuffer.getDirectPointer())
		{
			memcpy(m_pBuffer, rMemoryBuffer.getDirectPointer(), size_t(m_size)); // $$$
		}
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
}

CMemoryBufferImpl::CMemoryBufferImpl(const uint8_t* pMemoryBuffer, const uint64_t size)
{
	m_pBuffer = new uint8_t[size_t(size + 1)]; // $$$
	if (m_pBuffer)
	{
		m_size              = size;
		m_ui64AllocatedSize = size;
		if (pMemoryBuffer)
		{
			memcpy(m_pBuffer, pMemoryBuffer, size_t(m_size)); // $$$
		}
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
}

CMemoryBufferImpl::~CMemoryBufferImpl()
{
	if (m_pBuffer)
	{
		delete [] m_pBuffer;
		m_pBuffer = nullptr;
	}
}

uint8_t* CMemoryBufferImpl::getDirectPointer() { return m_pBuffer; }

const uint8_t* CMemoryBufferImpl::getDirectPointer() const { return m_pBuffer; }

uint64_t CMemoryBufferImpl::getSize() const { return m_size; }

bool CMemoryBufferImpl::reserve(const uint64_t ui64Size)
{
	if (ui64Size > m_ui64AllocatedSize)
	{
		uint8_t* l_pSavedBuffer = m_pBuffer;
		m_pBuffer               = new uint8_t[size_t(ui64Size + 1)]; // $$$
		if (!m_pBuffer) { return false; }
		memcpy(m_pBuffer, l_pSavedBuffer, size_t(m_size)); // $$$

		delete [] l_pSavedBuffer;
		m_ui64AllocatedSize            = ui64Size;
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
	return true;
}

bool CMemoryBufferImpl::setSize(const uint64_t ui64Size, const bool bDiscard)
{
	if (ui64Size > m_ui64AllocatedSize)
	{
		uint8_t* l_pSavedBuffer = m_pBuffer;
		m_pBuffer               = new uint8_t[size_t(ui64Size + 1)]; // $$$
		if (!m_pBuffer) { return false; }
		if (!bDiscard) { memcpy(m_pBuffer, l_pSavedBuffer, size_t(m_size)); }	// $$$
		if (l_pSavedBuffer) { delete [] l_pSavedBuffer; }
		m_ui64AllocatedSize            = ui64Size;
		m_pBuffer[m_ui64AllocatedSize] = 0;
	}
	m_size = ui64Size;
	return true;
}

bool CMemoryBufferImpl::append(const uint8_t* buffer, const uint64_t size)
{
	if (size != 0)
	{
		uint64_t l_ui64BufferSizeBackup = m_size;
		if (!this->setSize(m_size + size, false)) { return false; }
		memcpy(m_pBuffer + l_ui64BufferSizeBackup, buffer, size_t(size));
	}
	return true;
}

bool CMemoryBufferImpl::append(const IMemoryBuffer& rMemoryBuffer)
{
	if (rMemoryBuffer.getSize() != 0)
	{
		uint64_t l_ui64BufferSizeBackup = m_size;
		if (!this->setSize(m_size + rMemoryBuffer.getSize(), false)) { return false; }
		memcpy(m_pBuffer + l_ui64BufferSizeBackup, rMemoryBuffer.getDirectPointer(), size_t(rMemoryBuffer.getSize()));
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

CMemoryBuffer::CMemoryBuffer() { m_pMemoryBufferImpl = new CMemoryBufferImpl(); }

CMemoryBuffer::CMemoryBuffer(const IMemoryBuffer& rMemoryBuffer) { m_pMemoryBufferImpl = new CMemoryBufferImpl(rMemoryBuffer); }

CMemoryBuffer::CMemoryBuffer(const uint8_t* pMemoryBuffer, const uint64_t size) { m_pMemoryBufferImpl = new CMemoryBufferImpl(pMemoryBuffer, size); }

CMemoryBuffer::~CMemoryBuffer() { delete m_pMemoryBufferImpl; }

bool CMemoryBuffer::reserve(const uint64_t ui64Size) { return m_pMemoryBufferImpl->reserve(ui64Size); }

bool CMemoryBuffer::setSize(const uint64_t ui64Size, const bool bDiscard) { return m_pMemoryBufferImpl->setSize(ui64Size, bDiscard); }

uint64_t CMemoryBuffer::getSize() const { return m_pMemoryBufferImpl->getSize(); }

uint8_t* CMemoryBuffer::getDirectPointer() { return m_pMemoryBufferImpl->getDirectPointer(); }

const uint8_t* CMemoryBuffer::getDirectPointer() const { return m_pMemoryBufferImpl->getDirectPointer(); }

bool CMemoryBuffer::append(const uint8_t* buffer, const uint64_t size) { return m_pMemoryBufferImpl->append(buffer, size); }

bool CMemoryBuffer::append(const IMemoryBuffer& rMemoryBuffer) { return m_pMemoryBufferImpl->append(rMemoryBuffer); }
