#pragma once

#include "ovtkCVector.hpp"
#include "../../ovtkIFeatureVector.h"

namespace OpenViBEToolkit
{
	template <class TParent>
	class TFeatureVector final : public TVector<TParent>
	{
	public:

		explicit TFeatureVector(OpenViBE::IMatrix& rMatrix)
			: TVector<TParent>(rMatrix) { }

		double getLabel() const override { return 0; }

		bool setLabel(const double /*label*/) override { return false; }

		_IsDerivedFromClass_Final_(OpenViBEToolkit::TVector < TParent >, OV_UndefinedIdentifier)
	};

	typedef TFeatureVector<IFeatureVector> CFeatureVector;
} // namespace OpenViBEToolkit
