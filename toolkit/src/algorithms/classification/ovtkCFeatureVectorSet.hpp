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

		virtual uint32_t getSize() const;
		virtual bool setSize(const uint32_t ui32Size);
		virtual double* getBuffer();
		virtual const double* getBuffer() const;
		virtual const char* getElementLabel(const uint32_t ui32Index) const;
		virtual bool setElementLabel(const uint32_t ui32Index, const char* sElementLabel);

		virtual double getLabel() const;
		virtual bool setLabel(const double f64Label);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVector, OV_UndefinedIdentifier)

		const OpenViBE::IMatrix* m_pMatrix = nullptr;
		uint32_t m_ui32DimensionIndex = 0;
		uint32_t m_ui32BufferElementCount = 0;
		const double* m_pBuffer = nullptr;
	};

	class CFeatureVectorSet : public IFeatureVectorSet
	{
	public:

		explicit CFeatureVectorSet(const OpenViBE::IMatrix& rMatrix);

		virtual uint32_t getFeatureVectorCount() const;
		virtual bool setFeatureVectorCount(uint32_t ui32FeatureVectorCount);
		virtual bool addFeatureVector(const IFeatureVector& rFeatureVector);
		virtual IFeatureVector& getFeatureVector(const uint32_t ui32Index);
		virtual const IFeatureVector& getFeatureVector(const uint32_t ui32Index) const;
		virtual uint32_t getLabelCount() const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVectorSet, OV_UndefinedIdentifier)

	protected:

		const OpenViBE::IMatrix& m_rMatrix;
		std::map<uint32_t, CInternalFeatureVector> m_vFeatureVector;
	};
}  // namespace OpenViBEToolkit
