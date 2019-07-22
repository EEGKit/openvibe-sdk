#pragma once

#include "ovIMessage.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IMessageEvent
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-05-30
		 * \brief Event message
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 */
		class OV_API IMessageEvent : public IMessage
		{
		public:

			_IsDerivedFromClass_(OpenViBE::Kernel::IMessage, OV_ClassId_Kernel_Player_MessageEvent)
		};
	}
}


