#ifndef __OpenViBEToolkit_IFeatureVectorSet_H__
#define __OpenViBEToolkit_IFeatureVectorSet_H__

#include "ovtkIObject.h"

namespace OpenViBEToolkit
{
	class IFeatureVector;

	class OVTK_API IFeatureVectorSet : public OpenViBEToolkit::IObject
	{
	public:

		virtual uint32_t getFeatureVectorCount(void) const = 0;
		virtual bool setFeatureVectorCount(const uint32_t ui32FeatureVectorCount) = 0;
		virtual bool addFeatureVector(const OpenViBEToolkit::IFeatureVector& rFeatureVector) = 0;

		virtual OpenViBEToolkit::IFeatureVector& getFeatureVector(const uint32_t ui32Index) = 0;
		virtual const OpenViBEToolkit::IFeatureVector& getFeatureVector(const uint32_t ui32Index) const = 0;
		virtual uint32_t getLabelCount(void) const = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_FeatureVectorSet);

		const OpenViBEToolkit::IFeatureVector& operator [](const uint32_t ui32Index) const
		{
			return this->getFeatureVector(ui32Index);
		}

		OpenViBEToolkit::IFeatureVector& operator [](const uint32_t ui32Index)
		{
			return this->getFeatureVector(ui32Index);
		}
	};
};

#endif // __OpenViBEToolkit_IFeatureVectorSet_H__
