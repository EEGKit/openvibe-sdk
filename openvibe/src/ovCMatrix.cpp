#include "ovCMatrix.h"

#include <vector>
#include <string>
#include <cstring>

using namespace OpenViBE;
using namespace std;

namespace OpenViBE
{
	namespace
	{
		class CMatrixImpl final : public IMatrix
		{
		public:

			CMatrixImpl();
			CMatrixImpl(const CMatrixImpl& other);
			CMatrixImpl& operator=(const CMatrixImpl& other);
			~CMatrixImpl() override;
			uint32_t getDimensionCount() const override;
			uint32_t getDimensionSize(uint32_t index) const override;
			const char* getDimensionLabel(uint32_t index, uint32_t entryIndex) const override;
			const double* getBuffer() const override;
			uint32_t getBufferElementCount() const override;
			bool setDimensionCount(uint32_t count) override;
			bool setDimensionSize(uint32_t index, uint32_t size) override;
			bool setDimensionLabel(uint32_t index, uint32_t entryIndex, const char* label) override;
			double* getBuffer() override;

			_IsDerivedFromClass_Final_(IMatrix, OV_ClassId_MatrixImpl)

		private:

			bool refreshInternalBuffer() const;

		protected:

			mutable double* m_buffer = nullptr;
			mutable uint32_t m_size   = 0;

			std::vector<uint32_t> m_vDimensionSize;
			std::vector<std::vector<std::string>> m_vDimensionLabel;
		};
	} // namespace
} // namespace OpenViBE

// ________________________________________________________________________________________________________________
//

CMatrixImpl::CMatrixImpl() {}

CMatrixImpl::CMatrixImpl(const CMatrixImpl& other)
{
	m_vDimensionSize  = other.m_vDimensionSize;
	m_vDimensionLabel = other.m_vDimensionLabel;
	this->refreshInternalBuffer();
	std::memcpy(this->getBuffer(), other.getBuffer(), other.getBufferElementCount() * sizeof(double));
}

CMatrixImpl& CMatrixImpl::operator=(const CMatrixImpl& other)
{
	if (this != &other)
	{
		if (m_buffer)
		{
			delete[] m_buffer;
			m_buffer = nullptr;
		}
		this->m_vDimensionSize  = other.m_vDimensionSize;
		this->m_vDimensionLabel = other.m_vDimensionLabel;
		this->refreshInternalBuffer();
		std::memcpy(this->getBuffer(), other.getBuffer(), other.getBufferElementCount() * sizeof(double));
	}
	return *this;
}

CMatrixImpl::~CMatrixImpl()
{
	if (m_buffer)
	{
		delete [] m_buffer;
		m_buffer = nullptr;
	}
}

uint32_t CMatrixImpl::getDimensionCount() const { return uint32_t(m_vDimensionSize.size()); }

uint32_t CMatrixImpl::getDimensionSize(const uint32_t index) const
{
	if (index >= m_vDimensionSize.size()) { return 0; }
	return m_vDimensionSize[index];
}

const char* CMatrixImpl::getDimensionLabel(const uint32_t index, const uint32_t entryIndex) const
{
	if (index >= m_vDimensionSize.size()) { return ""; }
	if (entryIndex >= m_vDimensionSize[index]) { return ""; }
	return m_vDimensionLabel[index][entryIndex].c_str();
}

const double* CMatrixImpl::getBuffer() const
{
	if (!m_buffer) { this->refreshInternalBuffer(); }
	return m_buffer;
}

uint32_t CMatrixImpl::getBufferElementCount() const
{
	if (!m_buffer || !m_size) { this->refreshInternalBuffer(); }
	return m_size;
}

bool CMatrixImpl::setDimensionCount(const uint32_t count)
{
	if (count == 0) { return false; }

	if (m_buffer)
	{
		delete [] m_buffer;
		m_buffer = nullptr;
	}

	m_vDimensionSize.clear();
	m_vDimensionSize.resize(count);

	m_vDimensionLabel.clear();
	m_vDimensionLabel.resize(count);

	return true;
}

bool CMatrixImpl::setDimensionSize(const uint32_t index, const uint32_t size)
{
	if (index >= m_vDimensionSize.size()) { return false; }

	if (m_buffer)
	{
		delete [] m_buffer;
		m_buffer = nullptr;
	}

	m_vDimensionSize[index] = size;
	m_vDimensionLabel[index].clear();
	m_vDimensionLabel[index].resize(size);
	return true;
}

bool CMatrixImpl::setDimensionLabel(const uint32_t index, const uint32_t entryIndex, const char* label)
{
	if (index >= m_vDimensionSize.size()) { return false; }
	if (entryIndex >= m_vDimensionSize[index]) { return false; }
	m_vDimensionLabel[index][entryIndex] = label;
	return true;
}

double* CMatrixImpl::getBuffer()
{
	if (!m_buffer) { this->refreshInternalBuffer(); }
	return m_buffer;
}

bool CMatrixImpl::refreshInternalBuffer() const
{
	if (m_buffer || m_vDimensionSize.empty()) { return false; }

	m_size = 1;
	for (size_t i = 0; i < m_vDimensionSize.size(); i++) { m_size *= m_vDimensionSize[i]; }

	if (m_size == 0) { return false; }

	m_buffer = new double[m_size];
	if (!m_buffer)
	{
		m_size = 0;
		return false;
	}

	return true;
}

// ________________________________________________________________________________________________________________
//

CMatrix::CMatrix() { m_pMatrixImpl = new CMatrixImpl(); }
CMatrix::CMatrix(const CMatrix& other) { m_pMatrixImpl = new CMatrixImpl(*dynamic_cast<CMatrixImpl*>(other.m_pMatrixImpl)); }
CMatrix::~CMatrix() { delete m_pMatrixImpl; }

CMatrix& CMatrix::operator=(const CMatrix& other)
{
	delete m_pMatrixImpl;
	m_pMatrixImpl = new CMatrixImpl(*dynamic_cast<CMatrixImpl*>(other.m_pMatrixImpl));
	return *this;
}

uint32_t CMatrix::getDimensionCount() const { return m_pMatrixImpl->getDimensionCount(); }
uint32_t CMatrix::getDimensionSize(const uint32_t index) const { return m_pMatrixImpl->getDimensionSize(index); }
const char* CMatrix::getDimensionLabel(const uint32_t index, const uint32_t entryIndex) const { return m_pMatrixImpl->getDimensionLabel(index, entryIndex); }
const double* CMatrix::getBuffer() const { return m_pMatrixImpl->getBuffer(); }
uint32_t CMatrix::getBufferElementCount() const { return m_pMatrixImpl->getBufferElementCount(); }
bool CMatrix::setDimensionCount(const uint32_t count) { return m_pMatrixImpl->setDimensionCount(count); }
bool CMatrix::setDimensionSize(const uint32_t index, const uint32_t size) { return m_pMatrixImpl->setDimensionSize(index, size); }

bool CMatrix::setDimensionLabel(const uint32_t index, const uint32_t entryIndex, const char* label)
{
	return m_pMatrixImpl->setDimensionLabel(index, entryIndex, label);
}

double* CMatrix::getBuffer() { return m_pMatrixImpl->getBuffer(); }
