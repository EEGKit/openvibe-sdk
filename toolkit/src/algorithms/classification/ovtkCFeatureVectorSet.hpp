#ifndef __OpenViBEToolkit_CFeatureVectorSet_H__
#define __OpenViBEToolkit_CFeatureVectorSet_H__

#include "../../ovtkIFeatureVector.h"
#include "../../ovtkIFeatureVectorSet.h"

#include <map>

namespace OpenViBEToolkit
{
	class CInternalFeatureVector : public OpenViBEToolkit::IFeatureVector
	{
	public:

		CInternalFeatureVector(void);

		virtual OpenViBE::uint32 getSize(void) const;
		virtual bool setSize(const OpenViBE::uint32 ui32Size);
		virtual double* getBuffer(void);
		virtual const double* getBuffer(void) const;
		virtual const char* getElementLabel(const OpenViBE::uint32 ui32Index) const;
		virtual bool setElementLabel(const OpenViBE::uint32 ui32Index, const char* sElementLabel);

		virtual double getLabel(void) const;
		virtual bool setLabel(const double f64Label);

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVector, OV_UndefinedIdentifier);

	public:

		const OpenViBE::IMatrix* m_pMatrix;
		OpenViBE::uint32 m_ui32DimensionIndex;
		OpenViBE::uint32 m_ui32BufferElementCount;
		const double* m_pBuffer;
	};

	class CFeatureVectorSet : public OpenViBEToolkit::IFeatureVectorSet
	{
	public:

		explicit CFeatureVectorSet(const OpenViBE::IMatrix& rMatrix);

		virtual OpenViBE::uint32 getFeatureVectorCount(void) const;
		virtual bool setFeatureVectorCount(const OpenViBE::uint32 ui32FeatureVectorCount);
		virtual bool addFeatureVector(const OpenViBEToolkit::IFeatureVector& rFeatureVector);
		virtual OpenViBEToolkit::IFeatureVector& getFeatureVector(const OpenViBE::uint32 ui32Index);
		virtual const OpenViBEToolkit::IFeatureVector& getFeatureVector(const OpenViBE::uint32 ui32Index) const;
		virtual OpenViBE::uint32 getLabelCount(void) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::IFeatureVectorSet, OV_UndefinedIdentifier);

	protected:

		const OpenViBE::IMatrix& m_rMatrix;
		std::map<OpenViBE::uint32, CInternalFeatureVector> m_vFeatureVector;
	};
};

#endif // __OpenViBEToolkit_CFeatureVectorSet_H__
