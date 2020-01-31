#pragma once

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
	 * This class offers a basic standalone impementation of the IMatrix
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the IMatrix
	 * interface and redirect their calls to this implementation.
	 */
	class OV_API CMatrix final : public IMatrix
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this matrix.
		 */
		CMatrix();


		CMatrix(const CMatrix& other);

		CMatrix& operator=(const CMatrix& other);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		~CMatrix() override;

		//@}
		size_t getDimensionCount() const override;
		size_t getDimensionSize(const size_t index) const override;
		const char* getDimensionLabel(const size_t index1, const size_t index2) const override;
		const double* getBuffer() const override;
		size_t getBufferElementCount() const override;
		bool setDimensionCount(const size_t count) override;
		bool setDimensionSize(const size_t index, const size_t size) override;
		bool setDimensionLabel(const size_t index1, const size_t index2, const char* label) override;
		double* getBuffer() override;

		_IsDerivedFromClass_Final_(IMatrix, OV_ClassId_MatrixBridge)

	private:

		IMatrix* m_impl = nullptr; //!< Internal implementation
	};
} // namespace OpenViBE
