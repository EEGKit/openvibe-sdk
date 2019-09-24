#pragma once
#include "ovkTMessage.h"

namespace OpenViBE
{
	namespace Kernel
	{

		//___________________________________________________________________//
		//                                                                   //

		template <class T>
		TMessage<T>::TMessage(const IKernelContext& ctx)
			: T(ctx), m_oIdentifier(OV_UndefinedIdentifier) {}

		//___________________________________________________________________//
		//                                                                   //

		template <class T>
		CIdentifier TMessage<T>::getIdentifier() const { return m_oIdentifier; }

		template <class T>
		uint64_t TMessage<T>::getTime() const { return m_ui64Time; }

		template <class T>
		bool TMessage<T>::setIdentifier(const CIdentifier& identifier)
		{
			if (m_oIdentifier != OV_UndefinedIdentifier) { return false; }
			if (identifier == OV_UndefinedIdentifier) { return false; }
			m_oIdentifier = identifier;
			return true;
		}

		template <class T>
		bool TMessage<T>::setTime(const uint64_t ui64Time)
		{
			m_ui64Time = ui64Time;
			return true;
		}

		//___________________________________________________________________//
		//                                                                   //
	} // namespace Kernel
} // namespace OpenViBE
