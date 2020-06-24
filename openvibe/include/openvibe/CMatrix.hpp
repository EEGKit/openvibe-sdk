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
	CMatrix() = default;
	explicit CMatrix(const size_t dim1, const size_t dim2 = 0) { resize(dim1, dim2); }
	CMatrix(const CMatrix& other) { copy(other); }
	~CMatrix() override { clear(); }

	//--------------------------------------------------
	//----------------- Getter/Setter ------------------
	//--------------------------------------------------
	size_t getDimensionCount() const { return m_dimSizes->size(); }
	size_t getDimensionSize(const size_t index) const { return (index >= m_dimSizes->size()) ? 0 : m_dimSizes->at(index); }
	std::string getDimensionLabel(const size_t idx1, const size_t idx2) const;
	const double* getBuffer() const;
	double* getBuffer();
	size_t getSize() const;

	bool setDimensionCount(const size_t count);
	bool setDimensionSize(const size_t index, const size_t size);
	bool setDimensionLabel(const size_t index1, const size_t index2, const std::string& label) const;

	//--------------------------------------------------
	//------------------- Operators --------------------
	//--------------------------------------------------
	CMatrix& operator=(const CMatrix& other)
	{
		copy(other);
		return *this;
	}

	const double& operator [](const size_t index) const { return this->getBuffer()[index]; }
	double& operator [](const size_t index) { return this->getBuffer()[index]; }

	//--------------------------------------------------
	//---------------------- Misc ----------------------
	//--------------------------------------------------
	void resize(const size_t dim1 = 0, const size_t dim2 = 0);
	void clearBuffer() const;
	void clear();
	void copy(const CMatrix& other);
	void reset() const;

	/// <summary> Display the matrix. </summary>
	/// <returns> the Matrix. </returns>
	std::string str() const;

	//--------------------------------------------------
	//--- friends (must be in header files for link) ---
	//--------------------------------------------------

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
	void refreshInternalBuffer() const;

	mutable double* m_buffer = nullptr;
	mutable size_t m_size    = 0;

	std::vector<size_t>* m_dimSizes                    = nullptr;
	std::vector<std::vector<std::string>>* m_dimLabels = nullptr;
};

}  // namespace OpenViBE
