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
			explicit CMemoryBufferImpl(const IMemoryBuffer& memoryBuffer);
			CMemoryBufferImpl(const uint8_t* pMemoryBuffer, size_t size);
			~CMemoryBufferImpl() override;
			bool reserve(const size_t ui64Size) override;
			bool setSize(const size_t ui64Size, const bool bDiscard) override;
			size_t getSize() const override;
			uint8_t* getDirectPointer() override;
			const uint8_t* getDirectPointer() const override;
			bool append(const uint8_t* buffer, const size_t size) override;
			bool append(const IMemoryBuffer& memoryBuffer) override;

			_IsDerivedFromClass_Final_(IMemoryBuffer, OV_ClassId_MemoryBufferImpl)

		protected:

			uint8_t* m_buffer      = nullptr;
			size_t m_size          = 0;
			size_t m_allocatedSize = 0;
		};
	} // namespace
} // namespace OpenViBE

// ________________________________________________________________________________________________________________
//

CMemoryBufferImpl::CMemoryBufferImpl() {}

CMemoryBufferImpl::CMemoryBufferImpl(const IMemoryBuffer& memoryBuffer)
{
	m_buffer = new uint8_t[size_t(memoryBuffer.getSize() + 1)]; // $$$
	if (m_buffer)
	{
		m_size          = memoryBuffer.getSize();
		m_allocatedSize = memoryBuffer.getSize();
		if (memoryBuffer.getDirectPointer())
		{
			memcpy(m_buffer, memoryBuffer.getDirectPointer(), size_t(m_size)); // $$$
		}
		m_buffer[m_allocatedSize] = 0;
	}
}

CMemoryBufferImpl::CMemoryBufferImpl(const uint8_t* pMemoryBuffer, const size_t size)
{
	m_buffer = new uint8_t[size_t(size + 1)]; // $$$
	if (m_buffer)
	{
		m_size          = size;
		m_allocatedSize = size;
		if (pMemoryBuffer)
		{
			memcpy(m_buffer, pMemoryBuffer, size_t(m_size)); // $$$
		}
		m_buffer[m_allocatedSize] = 0;
	}
}

CMemoryBufferImpl::~CMemoryBufferImpl()
{
	if (m_buffer)
	{
		delete [] m_buffer;
		m_buffer = nullptr;
	}
}

uint8_t* CMemoryBufferImpl::getDirectPointer() { return m_buffer; }

const uint8_t* CMemoryBufferImpl::getDirectPointer() const { return m_buffer; }

size_t CMemoryBufferImpl::getSize() const { return m_size; }

bool CMemoryBufferImpl::reserve(const size_t ui64Size)
{
	if (ui64Size > m_allocatedSize)
	{
		uint8_t* buffer = m_buffer;
		m_buffer        = new uint8_t[size_t(ui64Size + 1)]; // $$$
		if (!m_buffer) { return false; }
		memcpy(m_buffer, buffer, size_t(m_size)); // $$$

		delete [] buffer;
		m_allocatedSize           = ui64Size;
		m_buffer[m_allocatedSize] = 0;
	}
	return true;
}

bool CMemoryBufferImpl::setSize(const size_t ui64Size, const bool bDiscard)
{
	if (ui64Size > m_allocatedSize)
	{
		uint8_t* buffer = m_buffer;
		m_buffer        = new uint8_t[size_t(ui64Size + 1)]; // $$$
		if (!m_buffer) { return false; }
		if (!bDiscard) { memcpy(m_buffer, buffer, size_t(m_size)); }	// $$$
		delete [] buffer;
		m_allocatedSize           = ui64Size;
		m_buffer[m_allocatedSize] = 0;
	}
	m_size = ui64Size;
	return true;
}

bool CMemoryBufferImpl::append(const uint8_t* buffer, const size_t size)
{
	if (size != 0)
	{
		const size_t bufferSizeBackup = m_size;
		if (!this->setSize(m_size + size, false)) { return false; }
		memcpy(m_buffer + bufferSizeBackup, buffer, size_t(size));
	}
	return true;
}

bool CMemoryBufferImpl::append(const IMemoryBuffer& memoryBuffer)
{
	if (memoryBuffer.getSize() != 0)
	{
		const size_t bufferSizeBackup = m_size;
		if (!this->setSize(m_size + memoryBuffer.getSize(), false)) { return false; }
		memcpy(m_buffer + bufferSizeBackup, memoryBuffer.getDirectPointer(), size_t(memoryBuffer.getSize()));
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

CMemoryBuffer::CMemoryBuffer() { m_impl = new CMemoryBufferImpl(); }

CMemoryBuffer::CMemoryBuffer(const IMemoryBuffer& memoryBuffer) { m_impl = new CMemoryBufferImpl(memoryBuffer); }

CMemoryBuffer::CMemoryBuffer(const uint8_t* memoryBuffer, const size_t size) { m_impl = new CMemoryBufferImpl(memoryBuffer, size); }

CMemoryBuffer::~CMemoryBuffer() { delete m_impl; }

bool CMemoryBuffer::reserve(const size_t size) { return m_impl->reserve(size); }

bool CMemoryBuffer::setSize(const size_t size, const bool discard) { return m_impl->setSize(size, discard); }

size_t CMemoryBuffer::getSize() const { return m_impl->getSize(); }

uint8_t* CMemoryBuffer::getDirectPointer() { return m_impl->getDirectPointer(); }

const uint8_t* CMemoryBuffer::getDirectPointer() const { return m_impl->getDirectPointer(); }

bool CMemoryBuffer::append(const uint8_t* buffer, const size_t size) { return m_impl->append(buffer, size); }

bool CMemoryBuffer::append(const IMemoryBuffer& memoryBuffer) { return m_impl->append(memoryBuffer); }
