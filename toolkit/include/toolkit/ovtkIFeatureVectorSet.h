#pragma once

#include "ovtkIObject.h"

namespace OpenViBEToolkit
{
	class IFeatureVector;

	class OVTK_API IFeatureVectorSet : public IObject
	{
	public:

		virtual uint32_t getFeatureVectorCount() const = 0;
		virtual bool setFeatureVectorCount(const uint32_t ui32FeatureVectorCount) = 0;
		virtual bool addFeatureVector(const IFeatureVector& rFeatureVector) = 0;

		virtual IFeatureVector& getFeatureVector(const uint32_t index) = 0;
		virtual const IFeatureVector& getFeatureVector(const uint32_t index) const = 0;
		virtual uint32_t getLabelCount() const = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_FeatureVectorSet)

		const IFeatureVector& operator [](const uint32_t index) const { return this->getFeatureVector(index); }

		IFeatureVector& operator [](const uint32_t index) { return this->getFeatureVector(index); }
	};
} // namespace OpenViBEToolkit
