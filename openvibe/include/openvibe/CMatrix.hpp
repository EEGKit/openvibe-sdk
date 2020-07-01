///-------------------------------------------------------------------------------------------------
/// 
/// \file CMatrix.hpp
/// \brief Basic standalone OpenViBE matrix implementation.
/// \author  Yann Renard (INRIA/IRISA) & Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 21/11/2007.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "ovIObject.h"
#include <vector>

namespace OpenViBE {

/// <summary> OpenViBE Matrix Class. </summary>
class OV_API CMatrix final : public IObject
{
public:

	//--------------------------------------------------
	//------------ Constructor / Destructor ------------
	//--------------------------------------------------
	/// <summary> Default constructor. </summary>
	CMatrix() { initVector(); };

	/// <summary> Constructor for 1D or 2D matrix with initial dimensions. </summary>
	/// <param name="dim1"> The first dimension size. </param>
	/// <param name="dim2"> The second dimension size. If it's <c>0</c>, the matrix is a Row vector. </param>
	explicit CMatrix(const size_t dim1, const size_t dim2 = 0) { resize(dim1, dim2); }

	/// <summary> Copy Constructor. </summary>
	/// <param name="m"> the matrix to copy. </param>
	CMatrix(const CMatrix& m) { copy(m); }

	/// <summary> Default destructor. </summary>
	~CMatrix() override { clear(); }

	//--------------------------------------------------
	//----------------- Getter/Setter ------------------
	//--------------------------------------------------
	/// <summary> Gets the number of dimension. </summary>
	/// <returns> THe number of dimension. </returns>
	size_t getDimensionCount() const { return m_dimSizes->size(); }

	/// <summary> Gets the size of the dimension <c>index</c>. </summary>
	/// <param name="index">The dimension. </param>
	/// <returns> the size of the selected dimension. </returns>
	size_t getDimensionSize(const size_t index) const { return (index >= m_dimSizes->size()) ? 0 : m_dimSizes->at(index); }

	/// <summary> Gets the label of the selected index in selected dimensions. </summary>
	/// <param name="dim"> The dimension concerned. </param>
	/// <param name="idx"> The index on this dimension. </param>
	/// <returns> The label. </returns>
	std::string getDimensionLabel(const size_t dim, const size_t idx) const;

	/// <summary> Const Buffer Accessor. </summary>
	/// <returns> The buffer. </returns>
	const double* getBuffer() const { return m_buffer; }

	/// <summary> Buffer Accessor. </summary>
	/// <returns> The buffer. </returns>
	double* getBuffer();

	/// <summary> Get the number of element in buffer. </summary>
	/// <returns> The number of element. </returns>
	size_t getSize() const;

	/// <summary> Get the dimension's sizes vector. </summary>
	/// <returns> The member <see cref="m_dimSizes"/>. </returns>
	std::vector<size_t>* getSizes() const { return m_dimSizes; }

	/// <summary> Get the dimension's labels vector. </summary>
	/// <returns> The member <see cref="m_dimLabels"/>. </returns>
	std::vector<std::vector<std::string>>* getLabels() const { return m_dimLabels; }

	/// <summary> Set the number of dimensions. </summary>
	/// <param name="count">The number of dimensions. </param>
	/// <returns></returns>
	void setDimensionCount(const size_t count) const;

	/// <summary> Set the size of the selected dimension. </summary>
	/// <param name="dim"> The selected dimension. </param>
	/// <param name="size"> The new size. </param>
	void setDimensionSize(const size_t dim, const size_t size) const;

	/// <summary> Set the label of the index in the selected dimension. </summary>
	/// <param name="dim"> The dimension concerned. </param>
	/// <param name="idx"> The index concerned. </param>
	/// <param name="label"> The Label to set. </param>
	void setDimensionLabel(const size_t dim, const size_t idx, const std::string& label) const;

	//--------------------------------------------------
	//------------------- Operators --------------------
	//--------------------------------------------------
	/// <summary> Copy Assignment Operator. </summary>
	/// <param name="m"> The matrix to copy. </param>
	/// <returns> Himself. </returns>
	CMatrix& operator=(const CMatrix& m)
	{
		copy(m);
		return *this;
	}

	/// <summary> Overload of const operator []. </summary>
	/// <param name="index">The index.</param>
	/// <returns> Const Reference of the object. </returns>
	const double& operator [](const size_t index) const { return this->getBuffer()[index]; }

	/// <summary> Overload of operator []. </summary>
	/// <param name="index">The index.</param>
	/// <returns> Reference of the object. </returns>
	double& operator [](const size_t index) { return this->getBuffer()[index]; }

	//--------------------------------------------------
	//---------------------- Misc ----------------------
	//--------------------------------------------------

	/// <summary> Resize 2D matrix (or 1D if dim2 = 0). </summary>
	/// <param name="dim1"> The first dimension. </param>
	/// <param name="dim2"> The second dimension. </param>
	void resize(const size_t dim1 = 0, const size_t dim2 = 0);

	/// <summary> Delete all pointer and reset size to 0. </summary>
	/// <remarks> Be carefull with this, must use a new constructor or resize function to allocate all pointers. </remarks>
	void clear();

	/// <summary> Copy matrix to this instance. </summary>
	/// <param name="m"> The matrix to copy. </param>
	void copy(const CMatrix& m);

	/// <summary> Set all element of the buffer to 0. </summary>
	void reset() const;
	
	/// <summary> Set all labels to "". </summary>
	void resetLabels() const { for (auto& dim : *m_dimLabels) { for (auto& l : dim) { l = ""; } } }
	
	/// <summary> Display the matrix. </summary>
	/// <returns> the Matrix. </returns>
	std::string str() const;

	/// <summary> Override the ostream operator. </summary>
	/// <param name="os"> The ostream. </param>
	/// <param name="obj"> The object. </param>
	/// <returns> Return the modified ostream. </returns>
	friend std::ostream& operator<<(std::ostream& os, const CMatrix& obj)
	{
		os << obj.str();
		return os;
	}

	_IsDerivedFromClass_Final_(IObject, OV_ClassId_Matrix)

private:
	
	/// <summary> Init Buffer and number of element. </summary>
	void initBuffer() const;

	/// <summary> Initialize vector pointer. </summary>
	void initVector();

	/// <summary> Delete Buffer pointer and reset size to 0. </summary>
	void clearBuffer() const;

	/// <summary> Delete vector pointer. </summary>
	void clearVector();
	
	mutable double* m_buffer = nullptr;	///< The matrix buffer.
	mutable size_t m_size    = 0;		///< The number of element.

	std::vector<size_t>* m_dimSizes                    = nullptr;	///< Size of all dimensions
	std::vector<std::vector<std::string>>* m_dimLabels = nullptr;	///< Labels of all dimensions
};

}  // namespace OpenViBE
