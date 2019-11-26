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

			CMemoryBufferImpl() {}
			explicit CMemoryBufferImpl(const IMemoryBuffer& buffer);
			CMemoryBufferImpl(const uint8_t* buffer, size_t size);
			~CMemoryBufferImpl() override;
			bool reserve(const size_t size) override;
			bool setSize(const size_t size, const bool discard) override;
			size_t getSize() const override { return m_size; }
			uint8_t* getDirectPointer() override { return m_buffer; }
			const uint8_t* getDirectPointer() const override { return m_buffer; }
			bool append(const uint8_t* buffer, const size_t size) override;
			bool append(const IMemoryBuffer& buffer) override;

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

CMemoryBufferImpl::CMemoryBufferImpl(const IMemoryBuffer& buffer)
{
	m_buffer = new uint8_t[size_t(buffer.getSize() + 1)]; // $$$
	if (m_buffer)
	{
		m_size          = buffer.getSize();
		m_allocatedSize = buffer.getSize();
		if (buffer.getDirectPointer())
		{
			memcpy(m_buffer, buffer.getDirectPointer(), size_t(m_size)); // $$$
		}
		m_buffer[m_allocatedSize] = 0;
	}
}

CMemoryBufferImpl::CMemoryBufferImpl(const uint8_t* buffer, const size_t size)
{
	m_buffer = new uint8_t[size_t(size + 1)]; // $$$
	if (m_buffer)
	{
		m_size          = size;
		m_allocatedSize = size;
		if (buffer)
		{
			memcpy(m_buffer, buffer, size_t(m_size)); // $$$
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

bool CMemoryBufferImpl::reserve(const size_t size)
{
	if (size > m_allocatedSize)
	{
		uint8_t* buffer = m_buffer;
		m_buffer        = new uint8_t[size_t(size + 1)]; // $$$
		if (!m_buffer) { return false; }
		memcpy(m_buffer, buffer, size_t(m_size)); // $$$

		delete [] buffer;
		m_allocatedSize           = size;
		m_buffer[m_allocatedSize] = 0;
	}
	return true;
}

bool CMemoryBufferImpl::setSize(const size_t size, const bool discard)
{
	if (size > m_allocatedSize)
	{
		uint8_t* buffer = m_buffer;
		m_buffer        = new uint8_t[size_t(size + 1)]; // $$$
		if (!m_buffer) { return false; }
		if (!discard) { memcpy(m_buffer, buffer, size_t(m_size)); }	// $$$
		delete [] buffer;
		m_allocatedSize           = size;
		m_buffer[m_allocatedSize] = 0;
	}
	m_size = size;
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

bool CMemoryBufferImpl::append(const IMemoryBuffer& buffer)
{
	if (buffer.getSize() != 0)
	{
		const size_t size = m_size;
		if (!this->setSize(m_size + buffer.getSize(), false)) { return false; }
		memcpy(m_buffer + size, buffer.getDirectPointer(), size_t(buffer.getSize()));
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

CMemoryBuffer::CMemoryBuffer() { m_impl = new CMemoryBufferImpl(); }
CMemoryBuffer::CMemoryBuffer(const IMemoryBuffer& buffer) { m_impl = new CMemoryBufferImpl(buffer); }
CMemoryBuffer::CMemoryBuffer(const uint8_t* buffer, const size_t size) { m_impl = new CMemoryBufferImpl(buffer, size); }
CMemoryBuffer::~CMemoryBuffer() { delete m_impl; }

bool CMemoryBuffer::reserve(const size_t size) { return m_impl->reserve(size); }

bool CMemoryBuffer::setSize(const size_t size, const bool discard) { return m_impl->setSize(size, discard); }
size_t CMemoryBuffer::getSize() const { return m_impl->getSize(); }

uint8_t* CMemoryBuffer::getDirectPointer() { return m_impl->getDirectPointer(); }
const uint8_t* CMemoryBuffer::getDirectPointer() const { return m_impl->getDirectPointer(); }

bool CMemoryBuffer::append(const uint8_t* buffer, const size_t size) { return m_impl->append(buffer, size); }
bool CMemoryBuffer::append(const IMemoryBuffer& buffer) { return m_impl->append(buffer); }
