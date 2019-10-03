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
			: T(ctx), m_id(OV_UndefinedIdentifier) {}

		//___________________________________________________________________//
		//                                                                   //

		template <class T>
		CIdentifier TMessage<T>::getIdentifier() const { return m_id; }

		template <class T>
		uint64_t TMessage<T>::getTime() const { return m_time; }

		template <class T>
		bool TMessage<T>::setIdentifier(const CIdentifier& identifier)
		{
			if (m_id != OV_UndefinedIdentifier) { return false; }
			if (identifier == OV_UndefinedIdentifier) { return false; }
			m_id = identifier;
			return true;
		}

		template <class T>
		bool TMessage<T>::setTime(const uint64_t ui64Time)
		{
			m_time = ui64Time;
			return true;
		}

		//___________________________________________________________________//
		//                                                                   //
	} // namespace Kernel
} // namespace OpenViBE
