#pragma once

#include "IWriter.h"

namespace EBML
{

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass>
	class TWriterCallbackProxy1 : public IWriterCallback
	{
	public:
		TWriterCallbackProxy1(COwnerClass& rOwnerObject, void (COwnerClass::*mfpWrite)(const void* pBuffer, uint64_t size))
			: m_rOwnerObject(rOwnerObject), m_mfpWrite(mfpWrite) { }

		void write(const void* pBuffer, const uint64_t size) override
		{
			if (m_mfpWrite)
			{
				(m_rOwnerObject.*m_mfpWrite)(pBuffer, size);
			}
		}

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const void* pBuffer, uint64_t size);
	};

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass, void (COwnerClass::*mfpWrite)(const void* pBuffer, uint64_t size)>
	class TWriterCallbackProxy2 : public IWriterCallback
	{
	public:
		TWriterCallbackProxy2(COwnerClass& rOwnerObject) : m_rOwnerObject(rOwnerObject), m_mfpWrite(mfpWrite) { }

		void write(const void* pBuffer, const uint64_t size) override
		{
			if (m_mfpWrite) { (m_rOwnerObject.*m_mfpWrite)(pBuffer, size); }
		}

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const void* pBuffer, uint64_t size);
	};

	// ________________________________________________________________________________________________________________
	//
} // namespace EBML
