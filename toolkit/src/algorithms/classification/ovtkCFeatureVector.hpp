#ifndef __OpenViBEToolkit_CFeatureVector_H__
#define __OpenViBEToolkit_CFeatureVector_H__

#include "ovtkCVector.hpp"
#include "../../ovtkIFeatureVector.h"

namespace OpenViBEToolkit
{
	template <class CParent>
	class TFeatureVector : public OpenViBEToolkit::TVector<CParent>
	{
	public:

		explicit TFeatureVector(OpenViBE::IMatrix& rMatrix)
			: OpenViBEToolkit::TVector<CParent>(rMatrix) { }

		virtual double getLabel(void) const { return 0; }

		virtual bool setLabel(const double f64Label) { return false; }

		_IsDerivedFromClass_Final_(OpenViBEToolkit::TVector < CParent >, OV_UndefinedIdentifier);
	};

	typedef OpenViBEToolkit::TFeatureVector<OpenViBEToolkit::IFeatureVector> CFeatureVector;
};

#endif // __OpenViBEToolkit_CFeatureVector_H__
