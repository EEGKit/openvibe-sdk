#include "CMatrix.hpp"

#include <sstream>
#include <string>

namespace OpenViBE {
//--------------------------------------------------
//----------------- Getter/Setter ------------------
//--------------------------------------------------

//--------------------------------------------------------------------------------
std::string CMatrix::getDimensionLabel(const size_t dim, const size_t idx) const
{
	return (dim >= m_dimSizes->size() || idx >= m_dimSizes->at(dim)) ? "" : m_dimLabels->at(dim)[idx];
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
double* CMatrix::getBuffer()
{
	if (!m_buffer) { initBuffer(); }
	return m_buffer;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
size_t CMatrix::getSize() const
{
	if (!m_buffer || m_size == 0) { initBuffer(); }
	return m_size;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::setDimensionCount(const size_t count) const
{
	if (count == 0) { return; }

	clearBuffer();
	m_dimSizes->resize(count);
	m_dimLabels->resize(count);
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::setDimensionSize(const size_t dim, const size_t size) const
{
	if (dim >= m_dimSizes->size()) { return; }

	clearBuffer();
	m_dimSizes->at(dim) = size;
	m_dimLabels->at(dim).clear();
	m_dimLabels->at(dim).resize(size);
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::setDimensionLabel(const size_t dim, const size_t idx, const std::string& label) const
{
	if (dim >= m_dimLabels->size() || idx >= m_dimLabels->at(dim).size()) { return; }
	m_dimLabels->at(dim)[idx] = label;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------
//---------------------- Misc ----------------------
//--------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::resize(const size_t dim1, const size_t dim2)
{
	clear();
	initVector();
	if (dim1 == 0) { return; }
	if (dim2 == 0)
	{
		setDimensionCount(1);
		setDimensionSize(0, dim1);
	}
	else
	{
		setDimensionCount(2);
		setDimensionSize(0, dim1);
		setDimensionSize(1, dim2);
	}
	reset();	// Initialise buffer with 0
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::clearBuffer() const
{
	if (m_buffer)
	{
		delete[] m_buffer;
		m_buffer = nullptr;
	}
	m_size = 0;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::clearVector()
{
	if (m_dimSizes)
	{
		delete m_dimSizes;
		m_dimSizes = nullptr;
	}
	if (m_dimLabels)
	{
		delete m_dimLabels;
		m_dimLabels = nullptr;
	}
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::clear()
{
	clearBuffer();
	clearVector();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::copy(const CMatrix& m)
{
	clear();
	m_dimSizes  = new std::vector<size_t>(*m.m_dimSizes);
	m_dimLabels = new std::vector<std::vector<std::string>>(*m.m_dimLabels);

	initBuffer();
	if (m_buffer) { std::memcpy(m_buffer, m.getBuffer(), getSize() * sizeof(double)); }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::initVector()
{
	clearVector();
	m_dimSizes  = new std::vector<size_t>;
	m_dimLabels = new std::vector<std::vector<std::string>>;
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::reset() const
{
	if (!m_buffer) { initBuffer(); }
	if (m_buffer) { std::memset(m_buffer, 0, m_size); }
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
std::string CMatrix::str() const
{
	std::stringstream ss;
	if (m_dimSizes->size() == 2)
	{
		size_t i = 0;
		for (size_t row = 0; row < m_dimSizes->at(0); ++row)
		{
			for (size_t col = 0; col < m_dimSizes->at(1); ++col) { ss << m_buffer[i++] << "\t"; }
			ss << "\n";
		}
	}
	else { for (size_t i = 0; i < m_size; ++i) { ss << m_buffer[i] << "\t"; } }
	return ss.str();
}
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void CMatrix::initBuffer() const
{
	if (m_buffer || m_dimSizes->empty()) { return; }

	m_size = 1;
	for (const auto& s : *m_dimSizes) { m_size *= s; }

	if (m_size != 0) { m_buffer = new double[m_size]; }
	if (!m_buffer) { m_size = 0; }
}
//--------------------------------------------------------------------------------

}  // namespace OpenViBE
