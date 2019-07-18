#ifndef __OpenViBE_CMatrix_H__
#define __OpenViBE_CMatrix_H__

#include "ovIMatrix.h"

namespace OpenViBE
{
	/**
	 * \class CMatrix
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic standalone OpenViBE matrix implementation
	 * \ingroup Group_Base
	 *
	 * This class offers a basic standalone impementation of the OpenViBE::IMatrix
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the OpenViBE::IMatrix
	 * interface and redirect their calls to this implementation.
	 */
	class OV_API CMatrix : public IMatrix
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this matrix.
		 */
		CMatrix(void);


		CMatrix(const CMatrix& other);

		CMatrix& operator=(const CMatrix& other);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		virtual ~CMatrix(void);

		//@}

		virtual const uint32_t getDimensionCount(void) const;
		virtual const uint32_t getDimensionSize(
			uint32_t ui32DimensionIndex) const;
		virtual const char* getDimensionLabel(
			uint32_t ui32DimensionIndex,
			uint32_t ui32DimensionEntryIndex) const;
		virtual const double* getBuffer(void) const;
		virtual const uint32_t getBufferElementCount(void) const;

		virtual bool setDimensionCount(
			uint32_t ui32DimensionCount);
		virtual bool setDimensionSize(
			uint32_t ui32DimensionIndex,
			uint32_t ui32DimensionSize);
		virtual bool setDimensionLabel(
			uint32_t ui32DimensionIndex,
			uint32_t ui32DimensionEntryIndex,
			const char* sDimensionLabel);
		virtual double* getBuffer(void);

		_IsDerivedFromClass_Final_(OpenViBE::IMatrix, OV_ClassId_MatrixBridge);

	private:

		IMatrix* m_pMatrixImpl = nullptr; //!< Internal implementation
	};
};

#endif // __OpenViBE_CMatrix_H__
