#pragma once
#include "ovkTMessage.h"

namespace OpenViBE {
namespace Kernel {

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
bool TMessage<T>::setIdentifier(const CIdentifier& id)
{
	if (m_id != OV_UndefinedIdentifier || id == OV_UndefinedIdentifier) { return false; }
	m_id = id;
	return true;
}

template <class T>
bool TMessage<T>::setTime(const uint64_t time)
{
	m_time = time;
	return true;
}

//___________________________________________________________________//
//                                                                   //
}  // namespace Kernel
}  // namespace OpenViBE
