#pragma once

#include "IWriter.h"

namespace XML
{

	// ________________________________________________________________________________________________________________
	//

	template <class TOwnerClass>
	class TWriterCallbackProxy1 final : public IWriterCallback
	{
	public:
		TWriterCallbackProxy1(TOwnerClass& rOwnerObject, void (TOwnerClass::*mfpWrite)(const char* sString)) : m_rOwnerObject(rOwnerObject),
																											   m_mfpWrite(mfpWrite) { }

		void write(const char* sString) override { if (m_mfpWrite) { m_rOwnerObject.m_mfpWrite(sString); } }

	protected:
		TOwnerClass& m_rOwnerObject;
		void (TOwnerClass::*m_mfpWrite)(const char* sString);
	};

	// ________________________________________________________________________________________________________________
	//

	template <class TOwnerClass, void (TOwnerClass::*mfpWrite)(const char* sString)>
	class TWriterCallbackProxy2 final : public IWriterCallback
	{
	public:
		TWriterCallbackProxy2(TOwnerClass rOwnerObject) : m_rOwnerObject(rOwnerObject), m_mfpWrite(mfpWrite) { }

		void write(const char* sString) override { if (mfpWrite) { m_rOwnerObject.mfpWrite(sString); } }

	protected:
		TOwnerClass& m_rOwnerObject;
		void (TOwnerClass::*m_mfpWrite)(const char* sString);
	};

	// ________________________________________________________________________________________________________________
	//
} // namespace XML
