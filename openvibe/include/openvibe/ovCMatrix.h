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
	 * This class offers a basic standalone impementation of the OpenViBE::IMatrix
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the OpenViBE::IMatrix
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
		uint32_t getDimensionCount() const override;
		uint32_t getDimensionSize(uint32_t index) const override;
		const char* getDimensionLabel(uint32_t index, uint32_t entryIndex) const override;
		const double* getBuffer() const override;
		uint32_t getBufferElementCount() const override;
		bool setDimensionCount(uint32_t count) override;
		bool setDimensionSize(uint32_t index, uint32_t size) override;
		bool setDimensionLabel(uint32_t index, uint32_t entryIndex, const char* label) override;
		double* getBuffer() override;

		_IsDerivedFromClass_Final_(OpenViBE::IMatrix, OV_ClassId_MatrixBridge)

	private:

		IMatrix* m_matrixImpl = nullptr; //!< Internal implementation
	};
} // namespace OpenViBE
