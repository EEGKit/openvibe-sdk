#pragma once

#include "ovtkCVector.hpp"
#include "../../ovtkIFeatureVector.h"

namespace OpenViBEToolkit
{
	template <class CParent>
	class TFeatureVector final : public TVector<CParent>
	{
	public:

		explicit TFeatureVector(OpenViBE::IMatrix& rMatrix)
			: TVector<CParent>(rMatrix) { }

		double getLabel() const override { return 0; }

		bool setLabel(const double /*label*/) override { return false; }

		_IsDerivedFromClass_Final_(OpenViBEToolkit::TVector < CParent >, OV_UndefinedIdentifier)
	};

	typedef TFeatureVector<IFeatureVector> CFeatureVector;
} // namespace OpenViBEToolkit
