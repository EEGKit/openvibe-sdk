#pragma once

#include "ovtkIVector.h"

namespace OpenViBEToolkit
{
	class OVTK_API IFeatureVector : public IVector
	{
	public:

		virtual double getLabel(void) const = 0;
		virtual bool setLabel(double f64Label) = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_FeatureVector);
	};
};


