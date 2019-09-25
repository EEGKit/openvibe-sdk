#pragma once

#include "../../ovtk_base.h"
#include "../../ovtkIVector.h"

namespace OpenViBEToolkit
{
	template <class CParent>
	class TVector : public CParent
	{
	public:

		explicit TVector(OpenViBE::IMatrix& matrix) : m_rMatrix(matrix) { }

		uint32_t getSize() const override { return m_rMatrix.getBufferElementCount(); }

		bool setSize(const uint32_t size) override
		{
			m_rMatrix.setDimensionCount(1);
			m_rMatrix.setDimensionSize(0, size);
			return true;
		}

		double* getBuffer() override { return m_rMatrix.getBuffer(); }
		const double* getBuffer() const override { return m_rMatrix.getBuffer(); }
		const char* getElementLabel(const uint32_t index) const override { return m_rMatrix.getDimensionLabel(0, index); }

		bool setElementLabel(const uint32_t index, const char* label) override
		{
			m_rMatrix.setDimensionLabel(0, index, label);
			return true;
		}

		_IsDerivedFromClass_Final_(CParent, OV_UndefinedIdentifier)

	protected:

		OpenViBE::IMatrix& m_rMatrix;
	};

	typedef TVector<IVector> CVector;
} // namespace OpenViBEToolkit
