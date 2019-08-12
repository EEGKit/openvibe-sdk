#pragma once

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IMessage
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-05-30
		 * \brief Base message class
		 * \ingroup Group_Player
		 * \ingroup Group_Kernel
		 */
		class OV_API IMessage : public IKernelObject
		{
		public:

			/**
			 * \brief Gets the message identifier
			 * \return The identifier of the message.
			 */
			virtual CIdentifier getIdentifier() const = 0;
			/**
			 * \brief Gets the timestamp of the message
			 * \return the timestamp of the message.
			 */
			virtual uint64_t getTime() const = 0;
			/**
			 * \brief Sets the message identifier for this message
			 * \param rIdentifier [in] : the new identifier of the message
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setIdentifier(const CIdentifier& rIdentifier) = 0;
			/**
			 * \brief Sets the message timestamp
			 * \param ui64Time [in] : the new timestamp of the message
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setTime(uint64_t ui64Time) = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Player_Message)
		};
	}  // namespace Kernel
}  // namespace OpenViBE
