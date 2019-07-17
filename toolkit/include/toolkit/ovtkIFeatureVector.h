#ifndef __OpenViBEToolkit_IFeatureVector_H__
#define __OpenViBEToolkit_IFeatureVector_H__

#include "ovtkIVector.h"

namespace OpenViBEToolkit
{
	class OVTK_API IFeatureVector : public OpenViBEToolkit::IVector
	{
	public:

		virtual double getLabel(void) const =0;
		virtual bool setLabel(const double f64Label) =0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_FeatureVector);
	};
};

#endif // __OpenViBEToolkit_IFeatureVector_H__
