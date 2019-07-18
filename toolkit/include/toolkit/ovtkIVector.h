#ifndef __OpenViBEToolkit_IVector_H__
#define __OpenViBEToolkit_IVector_H__

#include "ovtkIObject.h"

namespace OpenViBEToolkit
{
	class OVTK_API IVector : public IObject
	{
	public:

		virtual uint32_t getSize(void) const = 0;
		virtual bool setSize(const uint32_t ui32Size) = 0;
		virtual double* getBuffer(void) = 0;
		virtual const double* getBuffer(void) const = 0;
		virtual const char* getElementLabel(const uint32_t ui32Index) const = 0;
		virtual bool setElementLabel(const uint32_t ui32Index, const char* sElementLabel) = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_Vector);

		const double& operator [](const uint32_t ui32Index) const
		{
			return this->getBuffer()[ui32Index];
		}

		double& operator [](const uint32_t ui32Index)
		{
			return this->getBuffer()[ui32Index];
		}
	};
};

#endif // __OpenViBEToolkit_IVector_H__
