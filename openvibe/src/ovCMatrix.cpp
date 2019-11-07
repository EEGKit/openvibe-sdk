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
			~CMatrixImpl() override;
			size_t getDimensionCount() const override;
			size_t getDimensionSize(const size_t index) const override;
			const char* getDimensionLabel(const size_t index1, const size_t index2) const override;
			const double* getBuffer() const override;
			size_t getBufferElementCount() const override;
			bool setDimensionCount(const size_t count) override;
			bool setDimensionSize(const size_t index, const size_t size) override;
			bool setDimensionLabel(const size_t index1, const size_t index2, const char* label) override;
			double* getBuffer() override;

			_IsDerivedFromClass_Final_(IMatrix, OV_ClassId_MatrixImpl)

		private:

			bool refreshInternalBuffer() const;

		protected:

			mutable double* m_buffer = nullptr;
			mutable size_t m_size    = 0;

			std::vector<size_t> m_vDimensionSize;
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

CMatrixImpl::~CMatrixImpl()
{
	if (m_buffer)
	{
		delete [] m_buffer;
		m_buffer = nullptr;
	}
}

size_t CMatrixImpl::getDimensionCount() const { return m_vDimensionSize.size(); }

size_t CMatrixImpl::getDimensionSize(const size_t index) const
{
	if (index >= m_vDimensionSize.size()) { return 0; }
	return m_vDimensionSize[index];
}

const char* CMatrixImpl::getDimensionLabel(const size_t index1, const size_t index2) const
{
	if (index1 >= m_vDimensionSize.size()) { return ""; }
	if (index2 >= m_vDimensionSize[index1]) { return ""; }
	return m_vDimensionLabel[index1][index2].c_str();
}

const double* CMatrixImpl::getBuffer() const
{
	if (!m_buffer) { this->refreshInternalBuffer(); }
	return m_buffer;
}

size_t CMatrixImpl::getBufferElementCount() const
{
	if (!m_buffer || !m_size) { this->refreshInternalBuffer(); }
	return m_size;
}

bool CMatrixImpl::setDimensionCount(const size_t count)
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

bool CMatrixImpl::setDimensionSize(const size_t index, const size_t size)
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

bool CMatrixImpl::setDimensionLabel(const size_t index1, const size_t index2, const char* label)
{
	if (index1 >= m_vDimensionSize.size()) { return false; }
	if (index2 >= m_vDimensionSize[index1]) { return false; }
	m_vDimensionLabel[index1][index2] = label;
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
	for (size_t i = 0; i < m_vDimensionSize.size(); ++i) { m_size *= m_vDimensionSize[i]; }

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

CMatrix::CMatrix() { m_impl = new CMatrixImpl(); }
CMatrix::CMatrix(const CMatrix& other) { m_impl = new CMatrixImpl(*dynamic_cast<CMatrixImpl*>(other.m_impl)); }
CMatrix::~CMatrix() { delete m_impl; }

CMatrix& CMatrix::operator=(const CMatrix& other)
{
	delete m_impl;
	m_impl = new CMatrixImpl(*dynamic_cast<CMatrixImpl*>(other.m_impl));
	return *this;
}

size_t CMatrix::getDimensionCount() const { return m_impl->getDimensionCount(); }
size_t CMatrix::getDimensionSize(const size_t index) const { return m_impl->getDimensionSize(index); }
const char* CMatrix::getDimensionLabel(const size_t index1, const size_t index2) const { return m_impl->getDimensionLabel(index1, index2); }
const double* CMatrix::getBuffer() const { return m_impl->getBuffer(); }
size_t CMatrix::getBufferElementCount() const { return m_impl->getBufferElementCount(); }
bool CMatrix::setDimensionCount(const size_t count) { return m_impl->setDimensionCount(count); }
bool CMatrix::setDimensionSize(const size_t index, const size_t size) { return m_impl->setDimensionSize(index, size); }
bool CMatrix::setDimensionLabel(const size_t index1, const size_t index2, const char* label) { return m_impl->setDimensionLabel(index1, index2, label); }
double* CMatrix::getBuffer() { return m_impl->getBuffer(); }
