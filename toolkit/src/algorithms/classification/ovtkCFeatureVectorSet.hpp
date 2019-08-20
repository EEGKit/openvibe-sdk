#pragma once

#include "../../ovtkIFeatureVector.h"
#include "../../ovtkIFeatureVectorSet.h"

#include <map>

namespace OpenViBEToolkit
{
	class CInternalFeatureVector : public IFeatureVector
	{
	public:

		CInternalFeatureVector();
		uint32_t getSize() const override;
		bool setSize(const uint32_t ui32Size) override;
		double* getBuffer() override;
		const double* getBuffer() const override;
		const char* getElementLabel(const uint32_t ui32Index) const override;
		bool setElementLabel(const uint32_t ui32Index, const char* sElementLabel) override;
		double getLabel() const override;
		bool setLabel(const double f64Label) override;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVector, OV_UndefinedIdentifier)

		const OpenViBE::IMatrix* m_pMatrix = nullptr;
		uint32_t m_ui32DimensionIndex      = 0;
		uint32_t m_ui32BufferElementCount  = 0;
		const double* m_pBuffer            = nullptr;
	};

	class CFeatureVectorSet : public IFeatureVectorSet
	{
	public:

		explicit CFeatureVectorSet(const OpenViBE::IMatrix& rMatrix);
		uint32_t getFeatureVectorCount() const override;
		bool setFeatureVectorCount(const uint32_t ui32FeatureVectorCount) override;
		bool addFeatureVector(const IFeatureVector& rFeatureVector) override;
		IFeatureVector& getFeatureVector(const uint32_t ui32Index) override;
		const IFeatureVector& getFeatureVector(const uint32_t ui32Index) const override;
		uint32_t getLabelCount() const override;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVectorSet, OV_UndefinedIdentifier)

	protected:

		const OpenViBE::IMatrix& m_rMatrix;
		std::map<uint32_t, CInternalFeatureVector> m_vFeatureVector;
	};
}  // namespace OpenViBEToolkit
