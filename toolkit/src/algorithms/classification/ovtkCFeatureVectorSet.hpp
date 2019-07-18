#ifndef __OpenViBEToolkit_CFeatureVectorSet_H__
#define __OpenViBEToolkit_CFeatureVectorSet_H__

#include "../../ovtkIFeatureVector.h"
#include "../../ovtkIFeatureVectorSet.h"

#include <map>

namespace OpenViBEToolkit
{
	class CInternalFeatureVector : public IFeatureVector
	{
	public:

		CInternalFeatureVector(void);

		virtual uint32_t getSize(void) const;
		virtual bool setSize(const uint32_t ui32Size);
		virtual double* getBuffer(void);
		virtual const double* getBuffer(void) const;
		virtual const char* getElementLabel(const uint32_t ui32Index) const;
		virtual bool setElementLabel(const uint32_t ui32Index, const char* sElementLabel);

		virtual double getLabel(void) const;
		virtual bool setLabel(const double f64Label);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVector, OV_UndefinedIdentifier);

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

		virtual uint32_t getFeatureVectorCount(void) const;
		virtual bool setFeatureVectorCount(const uint32_t ui32FeatureVectorCount);
		virtual bool addFeatureVector(const IFeatureVector& rFeatureVector);
		virtual IFeatureVector& getFeatureVector(const uint32_t ui32Index);
		virtual const IFeatureVector& getFeatureVector(const uint32_t ui32Index) const;
		virtual uint32_t getLabelCount(void) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVectorSet, OV_UndefinedIdentifier);

	protected:

		const OpenViBE::IMatrix& m_rMatrix;
		std::map<uint32_t, CInternalFeatureVector> m_vFeatureVector;
	};
};

#endif // __OpenViBEToolkit_CFeatureVectorSet_H__
