#pragma once

#include "IReader.h"

namespace XML
{

	// ________________________________________________________________________________________________________________
	//

	template <class TOwnerClass>
	class TReaderCallbackProxy1 final : public IReaderCallback
	{
	public:
		TReaderCallbackProxy1(TOwnerClass& rOwnerObject,
							  void (TOwnerClass::*mfpOpenChild)(const char* name, const char** sAttributeName, const char** sAttributeValue,
																uint64_t nAttribute),
							  void (TOwnerClass::*mfpProcessChildData)(const char* sData), void (TOwnerClass::*mfpCloseChild)())
			: m_rOwnerObject(rOwnerObject), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		void openChild(const char* name, const char** sAttributeName, const char** sAttributeValue, uint64_t nAttribute) override
		{
			if (m_mfpOpenChild) { m_rOwnerObject.m_mfpOpenChild(name, sAttributeName, sAttributeValue, nAttribute); }
		}

		void processChildData(const char* sData) override { if (m_mfpProcessChildData) { m_rOwnerObject.m_mfpProcessChildData(sData); } }
		void closeChild() override { if (m_mfpCloseChild) { m_rOwnerObject.m_mfpCloseChild(); } }

	protected:
		TOwnerClass& m_rOwnerObject;
		void (TOwnerClass::*m_mfpOpenChild)(const char* name, const char** sAttributeName, const char** sAttributeValue, uint64_t nAttribute);
		void (TOwnerClass::*m_mfpProcessChildData)(const char* sData);
		void (TOwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass, void (COwnerClass::*mfpOpenChild)(const char* name, const char** sAttributeName, const char** sAttributeValue,
																   uint64_t nAttribute), void (COwnerClass::*mfpProcessChildData)(const char* sData),
			  void (COwnerClass::*mfpCloseChild)()>
	class TReaderCallbackProxy2 final : public IReaderCallback
	{
	public:
		TReaderCallbackProxy2(COwnerClass rOwnerObject)
			: m_rOwnerObject(rOwnerObject), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		void openChild(const char* name, const char** sAttributeName, const char** sAttributeValue, uint64_t nAttribute) override
		{
			if (mfpOpenChild) { m_rOwnerObject.mfpOpenChild(name, sAttributeName, sAttributeValue, nAttribute); }
		}

		void processChildData(const char* sData) override { if (mfpProcessChildData) { m_rOwnerObject.mfpProcessChildData(sData); } }
		void closeChild() override { if (mfpCloseChild) { m_rOwnerObject.mfpCloseChild(); } }

	protected:
		COwnerClass& m_rOwnerObject;
		void (COwnerClass::*m_mfpOpenChild)(const char* name, const char** sAttributeName, const char** sAttributeValue, uint64_t nAttribute);
		void (COwnerClass::*m_mfpProcessChildData)(const char* sData);
		void (COwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//
}
