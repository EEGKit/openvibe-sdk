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
		virtual bool setSize(uint32_t ui32Size);
		virtual double* getBuffer();
		virtual const double* getBuffer() const;
		virtual const char* getElementLabel(uint32_t ui32Index) const;
		virtual bool setElementLabel(uint32_t ui32Index, const char* sElementLabel);

		virtual double getLabel() const;
		virtual bool setLabel(double f64Label);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVector, OV_UndefinedIdentifier)

	public:

		const OpenViBE::IMatrix* m_pMatrix;
		uint32_t m_ui32DimensionIndex;
		uint32_t m_ui32BufferElementCount;
		const double* m_pBuffer;
	};

	class CFeatureVectorSet : public IFeatureVectorSet
	{
	public:

		explicit CFeatureVectorSet(const OpenViBE::IMatrix& rMatrix);

		virtual uint32_t getFeatureVectorCount() const;
		virtual bool setFeatureVectorCount(uint32_t ui32FeatureVectorCount);
		virtual bool addFeatureVector(const IFeatureVector& rFeatureVector);
		virtual IFeatureVector& getFeatureVector(uint32_t ui32Index);
		virtual const IFeatureVector& getFeatureVector(uint32_t ui32Index) const;
		virtual uint32_t getLabelCount() const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVectorSet, OV_UndefinedIdentifier)

	protected:

		const OpenViBE::IMatrix& m_rMatrix;
		std::map<uint32_t, CInternalFeatureVector> m_vFeatureVector;
	};
};


