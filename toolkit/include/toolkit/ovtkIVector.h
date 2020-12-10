#pragma once

#include "ovtk_base.h"

namespace OpenViBE {
namespace Toolkit {
class OVTK_API IVector : public IObject
{
public:

	virtual uint32_t getSize() const = 0;
	virtual bool setSize(const uint32_t size) = 0;
	virtual double* getBuffer() = 0;
	virtual const double* getBuffer() const = 0;
	virtual const char* getElementLabel(const uint32_t index) const = 0;
	virtual bool setElementLabel(const uint32_t index, const char* sElementLabel) = 0;

	_IsDerivedFromClass_(IObject, OVTK_ClassId_Vector)

	const double& operator [](const uint32_t index) const { return this->getBuffer()[index]; }
	double& operator [](const uint32_t index) { return this->getBuffer()[index]; }
};
}  // namespace Toolkit
}  // namespace OpenViBE
