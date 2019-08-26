#pragma once

#include "ovtkIObject.h"

namespace OpenViBEToolkit
{
	class OVTK_API IVector : public IObject
	{
	public:

		virtual uint32_t getSize() const = 0;
		virtual bool setSize(const uint32_t ui32Size) = 0;
		virtual double* getBuffer() = 0;
		virtual const double* getBuffer() const = 0;
		virtual const char* getElementLabel(const uint32_t index) const = 0;
		virtual bool setElementLabel(const uint32_t index, const char* sElementLabel) = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_Vector)

		const double& operator [](const uint32_t index) const { return this->getBuffer()[index]; }
		double& operator [](const uint32_t index) { return this->getBuffer()[index]; }
	};
} // namespace OpenViBEToolkit
