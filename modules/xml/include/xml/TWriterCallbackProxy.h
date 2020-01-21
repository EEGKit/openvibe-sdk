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
		TWriterCallbackProxy1(TOwnerClass& ownerObject, void (TOwnerClass::*mfpWrite)(const char* str))
			: m_ownerObject(ownerObject), m_mfpWrite(mfpWrite) { }

		void write(const char* str) override { if (m_mfpWrite) { m_ownerObject.m_mfpWrite(str); } }

	protected:
		TOwnerClass& m_ownerObject;
		void (TOwnerClass::*m_mfpWrite)(const char* str);
	};

	// ________________________________________________________________________________________________________________
	//

	template <class TOwnerClass, void (TOwnerClass::*TMfpWrite)(const char* str)>
	class TWriterCallbackProxy2 final : public IWriterCallback
	{
	public:
		TWriterCallbackProxy2(TOwnerClass ownerObject) : m_ownerObject(ownerObject), m_mfpWrite(TMfpWrite) { }

		void write(const char* str) override { if (TMfpWrite) { m_ownerObject.mfpWrite(str); } }

	protected:
		TOwnerClass& m_ownerObject;
		void (TOwnerClass::*m_mfpWrite)(const char* str);
	};

	// ________________________________________________________________________________________________________________
	//
} // namespace XML
