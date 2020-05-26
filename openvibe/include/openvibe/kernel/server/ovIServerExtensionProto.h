#pragma once

#include "../ovIConfigurable.h"

namespace OpenViBE {
namespace Kernel {
/**
  * \class IServerExtensionProto
  * \brief Prototype interface for a server extension
  * \author Jozef Legeny (Inria)
  * \ingroup Group_Server
  * \ingroup Group_Kernel
  * \ingroup Group_Extend
  * \sa Plugins::IServerExtension
  * \sa Plugins::IServerExtensionDesc
  */

class OV_API IServerExtensionProto : public IKernelObject
{
public:
	virtual size_t addSetting(const CString& name, const CIdentifier& typeID, const CString& value) = 0;
	_IsDerivedFromClass_(IKernelObject, OV_ClassId_Kernel_Server_ServerExtensionProto)
};
}  // namespace Kernel
}  // namespace OpenViBE
