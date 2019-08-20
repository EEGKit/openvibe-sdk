#pragma once

#include "../../ovtk_base.h"
#include "../../ovtkIVector.h"

namespace OpenViBEToolkit
{
	template <class CParent>
	class TVector : public CParent
	{
	public:

		explicit TVector(OpenViBE::IMatrix& rMatrix)
			: m_rMatrix(rMatrix) { }

		virtual uint32_t getSize() const { return m_rMatrix.getBufferElementCount(); }

		virtual bool setSize(const uint32_t ui32Size)
		{
			m_rMatrix.setDimensionCount(1);
			m_rMatrix.setDimensionSize(0, ui32Size);
			return true;
		}

		virtual double* getBuffer() { return m_rMatrix.getBuffer(); }

		virtual const double* getBuffer() const { return m_rMatrix.getBuffer(); }

		virtual const char* getElementLabel(const uint32_t ui32Index) const
		{
			return m_rMatrix.getDimensionLabel(0, ui32Index);
		}

		virtual bool setElementLabel(const uint32_t ui32Index, const char* sElementLabel)
		{
			m_rMatrix.setDimensionLabel(0, ui32Index, sElementLabel);
			return true;
		}

		_IsDerivedFromClass_Final_(CParent, OV_UndefinedIdentifier)

	protected:

		OpenViBE::IMatrix& m_rMatrix;
	};

	typedef TVector<IVector> CVector;
}  // namespace OpenViBEToolkit
