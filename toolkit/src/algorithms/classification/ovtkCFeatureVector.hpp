#pragma once

#include "ovtkCVector.hpp"
#include "../../ovtkIFeatureVector.h"

namespace OpenViBEToolkit
{
	template <class CParent>
	class TFeatureVector : public TVector<CParent>
	{
	public:

		explicit TFeatureVector(OpenViBE::IMatrix& rMatrix)
			: TVector<CParent>(rMatrix) { }

		virtual double getLabel() const { return 0; }

		virtual bool setLabel(const double f64Label) { return false; }

		_IsDerivedFromClass_Final_(OpenViBEToolkit::TVector < CParent >, OV_UndefinedIdentifier);
	};

	typedef TFeatureVector<IFeatureVector> CFeatureVector;
};


