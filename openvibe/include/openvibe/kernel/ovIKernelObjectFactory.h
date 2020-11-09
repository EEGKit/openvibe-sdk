#pragma once

#include "ovIKernelObject.h"

namespace OpenViBE {
namespace Kernel {
/**
 * \class IKernelObjectFactory
 * \brief Kernel object factory, creates all kernel objects
 * \author Yann Renard (INRIA/IRISA)
 * \date 2006-09-26
 * \ingroup Group_Kernel
 *
 * This class allows to create kernel objects as needed.
 *
 * \todo should it be removed ?
 */
class OV_API IKernelObjectFactory : public IKernelObject
{
public:

	/**
	 * \brief Creates a new kernel object givent its class identifier
	 * \param classID [in] : the class identifier of the object to create
	 * \return a pointer on the created object in case of success.
	 * \return \c NULL in case of error.
	 */
	virtual IObject* createObject(const CIdentifier& classID) = 0;
	/**
	 * \brief Releases an object created by this factory
	 * \param obj [in] : the object to release
	 * \return \e true in case of success.
	 * \return \e false in case of error.
	 * \note The factory should have created the object in order to release it.
	 */
	virtual bool releaseObject(IObject* obj) = 0;

	_IsDerivedFromClass_(IKernelObject, OV_ClassId_Kernel_KernelObjectFactory)
};
}  // namespace Kernel
}  // namespace OpenViBE
