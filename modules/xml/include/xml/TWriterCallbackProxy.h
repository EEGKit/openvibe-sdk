#pragma once

#include "IWriter.h"

namespace XML
{

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass>
	class TWriterCallbackProxy1 final : public IWriterCallback
	{
	public:
		TWriterCallbackProxy1(COwnerClass& rOwnerObject, void (COwnerClass::*mfpWrite)(const char* sString)) : m_rOwnerObject(rOwnerObject),
																											   m_mfpWrite(mfpWrite) { }

		void write(const char* sString) override { if (m_mfpWrite) { m_rOwnerObject.m_mfpWrite(sString); } }

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const char* sString);
	};

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass, void (COwnerClass::*mfpWrite)(const char* sString)>
	class TWriterCallbackProxy2 final : public IWriterCallback
	{
	public:
		TWriterCallbackProxy2(COwnerClass rOwnerObject) : m_rOwnerObject(rOwnerObject), m_mfpWrite(mfpWrite) { }

		void write(const char* sString) override { if (mfpWrite) { m_rOwnerObject.mfpWrite(sString); } }

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const char* sString);
	};

	// ________________________________________________________________________________________________________________
	//
} // namespace XML
