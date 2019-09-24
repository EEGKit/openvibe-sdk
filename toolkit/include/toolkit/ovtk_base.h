#pragma once

#include "ovtk_defines.h"

#include <openvibe/ov_all.h>

namespace EBML
{
	class IWriter;
	class IWriterCallback;
	class IWriterHelper;
	class IReader;
	class IReaderCallback;
	class IReaderHelper;
} // namespace EBML

namespace OpenViBEToolkit
{
	template <class THandledType>
	class TScopeHandle
	{
	public:

		TScopeHandle(THandledType& rHandler, THandledType& rHandledValue) : m_rHandler(rHandler), m_oLastHandledValue(rHandler) { m_rHandler = rHandledValue; }
		~TScopeHandle() { m_rHandler = m_oLastHandledValue; }

	private:

		THandledType& m_rHandler;
		THandledType m_oLastHandledValue;
	};
} // namespace OpenViBEToolkit
