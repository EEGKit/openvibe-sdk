#pragma once

#include "ovIObject.h"

namespace OpenViBE {
/**
 * \class IMatrix
 * \author Yann Renard (INRIA/IRISA)
 * \date 2007-11-21
 * \brief Basic OpenViBE matrix interface
 *
 * This interface offers functionalities to basically manipulate an n-dimensional
 * matrix. The manipulation that can be done are very limited and nearly limits
 * to memory manipulation. This is basically an unified way for storing matrices
 * content in OpenViBE
 *
 * OpenViBE provides an standalone implementation of this interface in CMatrix
 */
class OV_API IMatrix : public IObject
{
public:

	/** \name Accessors */
	//@{

	/**
	 * \brief Gets the dimension count for this matrix
	 * \return the dimension count for this matrix
	 * \note Uninitialized matrices should return \c 0
	 */
	virtual size_t getDimensionCount() const = 0;
	/**
	 * \brief Gets the dimension size for a given dimension index
	 * \param index [in] : the dimension index which size has to be returned
	 * \return the dimension size for the specified dimension index
	 * \note Each dimension can have a different size
	 */
	virtual size_t getDimensionSize(const size_t index) const = 0;
	/**
	 * \brief Gets the dimension label for a given dimension idx1 / dimension entry idx1 pair
	 * \param idx1 [in] : the dimension idx1
	 * \param idx2 [in] : the dimension entry idx1 for the specified dimension idx1
	 * \return the dimension label for the given dimension idx1 / dimension entry idx1 pair
	 * \note The string is permanent pointer until the dimension size or the label itself changes
	 */
	virtual const char* getDimensionLabel(const size_t idx1, const size_t idx2) const = 0;

	/**
	 * \brief Gets the raw buffer for this matrix
	 * \return the raw buffer for this matrix
	 * \note The raw buffer is permanent pointer until the dimension count or any dimension size changes
	 *
	 * When the raw buffer is acquired, the elements are accessed sequentially, using each dimension
	 * after the other. Suppose a matrix of 2 dimensions. First dimension size is 2 and second dimension
	 * size is 3 :
	 *
	 * \code
	 *
	 * IMatrix* matrix= // ...
	 *
	 * matrix->setDimensionCount(2);
	 * matrix->setDimensionSize(0, 2);
	 * matrix->setDimensionSize(1, 3);
	 *
	 * double* matrixRawBuffer=matrix->getBuffer();
	 *
	 * matrixRawBuffer[0]; // this is matrixRawBuffer[0*3 + 0];
	 * matrixRawBuffer[1]; // this is matrixRawBuffer[0*3 + 1];
	 * matrixRawBuffer[2]; // this is matrixRawBuffer[0*3 + 2];
	 * matrixRawBuffer[3]; // this is matrixRawBuffer[1*3 + 0];
	 * matrixRawBuffer[4]; // this is matrixRawBuffer[1*3 + 1];
	 * matrixRawBuffer[5]; // this is matrixRawBuffer[1*3 + 2];
	 *
	 * matrixRawBuffer[i*3+j]; // this is index i of the first dimension, and j of the second dimension
	 *
	 * \endcode
	 * \sa getBufferElementCount
	 */
	virtual const double* getBuffer() const = 0;
	/**
	 * \brief Gets the total number of elements in the matrix
	 * \return the total number of elements in the matrix
	 * \note The value returned by this function is the size of the raw buffer returned by \c getBuffer
	 */
	virtual size_t getBufferElementCount() const = 0;

	//@}
	/** \name Modifiers */
	//@{

	/**
	 * \brief Sets dimension count for this matrix
	 * \param count [in] : the number of dimension for this matrix
	 * \return \e true in case of success.
	 * \return \e false in case of error.
	 */
	virtual bool setDimensionCount(const size_t count) = 0;
	/**
	 * \brief Sets the dimension size for this matrix on a given dimension index
	 * \param index [in] : the dimension index which size has to be changed
	 * \param size [in] : the new dimension size for this dimension
	 * \return \e true in case of success.
	 * \return \e false in case of error.
	 */
	virtual bool setDimensionSize(const size_t index, const size_t size) = 0;
	/**
	 * \brief Sets the dimension label for this matrix on a given dimension idx1 / dimension entry idx1 pair
	 * \param idx1 [in] : the dimension idx1
	 * \param idx2 [in] : the entry idx1 in the specificed dimension which label has to be changed
	 * \param label [in] : the new dimension label to apply to this dimension idx1 / dimension entry idx1 pair
	 * \return \e true in case of success.
	 * \return \e false in case of error.
	 */
	virtual bool setDimensionLabel(const size_t idx1, const size_t idx2, const char* label) = 0;

	/**
	 * \brief Gets a writable raw buffer for this matrix
	 * \return the raw buffer for this matrix
	 * \note The raw buffer is permanent pointer until the dimension count or any dimension size changes
	 *
	 * \sa IMatrix::getBufferElementCount
	 * \sa IMatrix::getBuffer const
	 */
	virtual double* getBuffer() = 0;

	//@}

	_IsDerivedFromClass_(IObject, OV_ClassId_Matrix)

	const double& operator [](const size_t index) const { return this->getBuffer()[index]; }
	double& operator [](const size_t index) { return this->getBuffer()[index]; }
};
}  // namespace OpenViBE
