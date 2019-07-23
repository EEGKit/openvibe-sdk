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
}

namespace OpenViBEToolkit
{
	template <class CHandledType>
	class TScopeHandle
	{
	public:

		TScopeHandle(CHandledType& rHandler, CHandledType& rHandledValue)
			: m_oLastHandledValue(rHandler), m_rHandler(rHandler)
		{
			m_rHandler = rHandledValue;
		}

		~TScopeHandle() { m_rHandler = m_oLastHandledValue; }

	private:

		CHandledType& m_rHandler;
		CHandledType m_oLastHandledValue;
	};
}  // namespace OpenViBEToolkit
