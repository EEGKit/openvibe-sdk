#pragma once

#include "ovtk_base.h"

namespace OpenViBEToolkit
{
	class OVTK_API IObject : public OpenViBE::IObject
	{
	public:

		_IsDerivedFromClass_(OpenViBE::IObject, OVTK_ClassId_);
	};
};


