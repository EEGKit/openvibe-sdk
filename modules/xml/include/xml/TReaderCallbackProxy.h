#pragma once

#include "IReader.h"

namespace XML
{

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass>
	class TReaderCallbackProxy1 : public IReaderCallback
	{
	public:
		TReaderCallbackProxy1(COwnerClass& rOwnerObject,
							  void (COwnerClass::*mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount),
							  void (COwnerClass::*mfpProcessChildData)(const char* sData), void (COwnerClass::*mfpCloseChild)())
			: m_rOwnerObject(rOwnerObject), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount)
		{
			if (m_mfpOpenChild) { m_rOwnerObject.m_mfpOpenChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount); }
		}

		virtual void processChildData(const char* sData) { if (m_mfpProcessChildData) { m_rOwnerObject.m_mfpProcessChildData(sData); } }

		virtual void closeChild() { if (m_mfpCloseChild) { m_rOwnerObject.m_mfpCloseChild(); } }

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount);
		void (COwnerClass::*m_mfpProcessChildData)(const char* sData);
		void (COwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass, void (COwnerClass::*mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount), void (COwnerClass::*mfpProcessChildData)(const char* sData), void (COwnerClass::*mfpCloseChild)()>
	class TReaderCallbackProxy2 : public IReaderCallback
	{
	public:
		TReaderCallbackProxy2(COwnerClass rOwnerObject)
			: m_rOwnerObject(rOwnerObject), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount)
		{
			if (mfpOpenChild) { m_rOwnerObject.mfpOpenChild(sName, sAttributeName, sAttributeValue, ui64AttributeCount); }
		}

		virtual void processChildData(const char* sData) { if (mfpProcessChildData) { m_rOwnerObject.mfpProcessChildData(sData); } }

		virtual void closeChild() { if (mfpCloseChild) { m_rOwnerObject.mfpCloseChild(); } }

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpOpenChild)(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount);
		void (COwnerClass::*m_mfpProcessChildData)(const char* sData);
		void (COwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//
};
