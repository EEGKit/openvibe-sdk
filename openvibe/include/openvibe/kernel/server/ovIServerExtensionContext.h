#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		  * \class IServerExtensionContext
		  * \author Jozef Legeny (Inria)
		  * \date 2013-06-12
		  * \brief Static context of a Server Extension object
		  * \ingroup Group_Server
		  * \ingroup Group_Kernel
		  * \ingroup Group_Extend
		  * \sa OpenViBE::Plugins::IServerExtension
		  */
		class OV_API IServerExtensionContext : public IKernelObject
		{
		public:

			/**
			 * \brief Gets a reference on the current log manager
			 * \return a reference on the current log manager
			 */
			virtual Kernel::ILogManager& getLogManager() = 0;
		};
	}
}
