#pragma once

#include "ovIMessage.h"

namespace OpenViBE {
namespace Kernel {
/**
 * \class IMessageClock
 * \author Yann Renard (INRIA/IRISA)
 * \date 2006-05-30
 * \brief Clock message
 * \ingroup Group_Player
 * \ingroup Group_Kernel
 */
class OV_API IMessageClock : public IMessage
{
public:

	_IsDerivedFromClass_(IMessage, OV_ClassId_Kernel_Player_MessageClock)
};
}  // namespace Kernel
}  // namespace OpenViBE
