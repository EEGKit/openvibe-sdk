#pragma once

#include "ovtkIObject.h"

namespace OpenViBEToolkit
{
	class IFeatureVector;

	class OVTK_API IFeatureVectorSet : public IObject
	{
	public:

		virtual uint32_t getFeatureVectorCount(void) const = 0;
		virtual bool setFeatureVectorCount(uint32_t ui32FeatureVectorCount) = 0;
		virtual bool addFeatureVector(const IFeatureVector& rFeatureVector) = 0;

		virtual IFeatureVector& getFeatureVector(uint32_t ui32Index) = 0;
		virtual const IFeatureVector& getFeatureVector(uint32_t ui32Index) const = 0;
		virtual uint32_t getLabelCount(void) const = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_FeatureVectorSet);

		const IFeatureVector& operator [](const uint32_t ui32Index) const
		{
			return this->getFeatureVector(ui32Index);
		}

		IFeatureVector& operator [](const uint32_t ui32Index)
		{
			return this->getFeatureVector(ui32Index);
		}
	};
};


