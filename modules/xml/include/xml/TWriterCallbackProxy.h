#pragma once

#include "IWriter.h"

namespace XML
{

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass>
	class TWriterCallbackProxy1 : public IWriterCallback
	{
	public:
		TWriterCallbackProxy1(COwnerClass& rOwnerObject, void (COwnerClass::*mfpWrite)(const char* sString)) : m_rOwnerObject(rOwnerObject), m_mfpWrite(mfpWrite) { }

		virtual void write(const char* sString)
		{
			if (m_mfpWrite) { m_rOwnerObject.m_mfpWrite(sString); }
		}

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const char* sString);
	};

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass, void (COwnerClass::*mfpWrite)(const char* sString)>
	class TWriterCallbackProxy2 : public IWriterCallback
	{
	public:
		TWriterCallbackProxy2(COwnerClass rOwnerObject) : m_rOwnerObject(rOwnerObject), m_mfpWrite(mfpWrite) { }

		virtual void write(const char* sString)
		{
			if (mfpWrite) { m_rOwnerObject.mfpWrite(sString); }
		}

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpWrite)(const char* sString);
	};

	// ________________________________________________________________________________________________________________
	//
};
