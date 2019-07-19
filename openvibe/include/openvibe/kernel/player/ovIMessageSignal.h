#pragma once

#include "ovIMessage.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IMessageSignal
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-05-30
		 * \brief Signal message
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 */
		class OV_API IMessageSignal : public IMessage
		{
		public:

			_IsDerivedFromClass_(OpenViBE::Kernel::IMessage, OV_ClassId_Kernel_Player_MessageSignal)
		};
	};
};


