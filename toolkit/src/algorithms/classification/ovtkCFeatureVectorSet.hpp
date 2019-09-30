#pragma once

#include "../../ovtkIFeatureVector.h"
#include "../../ovtkIFeatureVectorSet.h"

#include <map>

namespace OpenViBEToolkit
{
	class CInternalFeatureVector final : public IFeatureVector
	{
	public:

		CInternalFeatureVector() { }
		uint32_t getSize() const override { return m_Size; }
		bool setSize(const uint32_t /*size*/) override { return false; }
		double* getBuffer() override { return nullptr; }
		const double* getBuffer() const override { return m_Buffer; }
		const char* getElementLabel(const uint32_t index) const override { return m_Matrix->getDimensionLabel(m_DimensionIdx, index); }
		bool setElementLabel(const uint32_t /*index*/, const char* /*elementLabel*/) override { return false; }
		double getLabel() const override { return m_Buffer[m_Size]; }
		bool setLabel(const double /*label*/) override { return false; }

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVector, OV_UndefinedIdentifier)

		const OpenViBE::IMatrix* m_Matrix = nullptr;
		uint32_t m_DimensionIdx            = 0;
		uint32_t m_Size                    = 0;
		const double* m_Buffer            = nullptr;
	};

	class CFeatureVectorSet final : public IFeatureVectorSet
	{
	public:

		explicit CFeatureVectorSet(const OpenViBE::IMatrix& rMatrix);
		uint32_t getFeatureVectorCount() const override { return m_matrix.getDimensionSize(0); }
		bool setFeatureVectorCount(const uint32_t /*nFeatureVector*/) override { return false; }
		bool addFeatureVector(const IFeatureVector& /*featureVector*/) override { return false; }
		IFeatureVector& getFeatureVector(const uint32_t index) override;
		const IFeatureVector& getFeatureVector(const uint32_t index) const override;
		uint32_t getLabelCount() const override;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVectorSet, OV_UndefinedIdentifier)

	protected:

		const OpenViBE::IMatrix& m_matrix;
		std::map<uint32_t, CInternalFeatureVector> m_features;
	};
} // namespace OpenViBEToolkit
