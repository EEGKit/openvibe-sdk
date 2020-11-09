#pragma once

#include "IReader.h"

namespace XML {

// ________________________________________________________________________________________________________________
//

template <class TOwnerClass>
class TReaderCallbackProxy1 final : public IReaderCallback
{
public:
	TReaderCallbackProxy1(TOwnerClass& ownerObject,
						  void (TOwnerClass::*mfpOpenChild)(const char* name, const char** attributeName, const char** attributeValue, size_t nAttribute),
						  void (TOwnerClass::*mfpProcessChildData)(const char* data), void (TOwnerClass::*mfpCloseChild)())
		: m_ownerObject(ownerObject), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

	void openChild(const char* name, const char** attributeName, const char** attributeValue, const size_t nAttribute) override
	{
		if (m_mfpOpenChild) { m_ownerObject.m_mfpOpenChild(name, attributeName, attributeValue, nAttribute); }
	}

	void processChildData(const char* data) override { if (m_mfpProcessChildData) { m_ownerObject.m_mfpProcessChildData(data); } }
	void closeChild() override { if (m_mfpCloseChild) { m_ownerObject.m_mfpCloseChild(); } }

protected:
	TOwnerClass& m_ownerObject;
	void (TOwnerClass::*m_mfpOpenChild)(const char* name, const char** attributeName, const char** attributeValue, size_t nAttribute);
	void (TOwnerClass::*m_mfpProcessChildData)(const char* data);
	void (TOwnerClass::*m_mfpCloseChild)();
};

// ________________________________________________________________________________________________________________
//

template <class TOwnerClass,
		  void (TOwnerClass::*TMfpOpenChild)(const char* name, const char** attributeName, const char** attributeValue, size_t nAttribute),
		  void (TOwnerClass::*TMfpProcessChildData)(const char* data), void (TOwnerClass::*mfpCloseChild)()>
class TReaderCallbackProxy2 final : public IReaderCallback
{
public:
	TReaderCallbackProxy2(TOwnerClass ownerObject)
		: m_ownerObject(ownerObject), m_mfpOpenChild(TMfpOpenChild), m_mfpProcessChildData(TMfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

	void openChild(const char* name, const char** attributeName, const char** attributeValue, const size_t nAttribute) override
	{
		if (TMfpOpenChild) { m_ownerObject.mfpOpenChild(name, attributeName, attributeValue, nAttribute); }
	}

	void processChildData(const char* data) override { if (TMfpProcessChildData) { m_ownerObject.mfpProcessChildData(data); } }
	void closeChild() override { if (mfpCloseChild) { m_ownerObject.mfpCloseChild(); } }

protected:
	TOwnerClass& m_ownerObject;
	void (TOwnerClass::*m_mfpOpenChild)(const char* name, const char** attributeName, const char** attributeValue, size_t nAttribute);
	void (TOwnerClass::*m_mfpProcessChildData)(const char* data);
	void (TOwnerClass::*m_mfpCloseChild)();
};
}  // namespace XML
