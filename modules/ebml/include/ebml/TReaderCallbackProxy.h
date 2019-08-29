#pragma once

#include "IReader.h"

namespace EBML
{

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass>
	class TReaderCallbackProxy1 final : public IReaderCallback
	{
	public:
		TReaderCallbackProxy1(COwnerClass& rOwnerObject, bool (COwnerClass::*mfpIsMasterChild)(const CIdentifier& rIdentifier), void (COwnerClass::*mfpOpenChild)(const CIdentifier& rIdentifier),
							  void (COwnerClass::*mfpProcessChildData)(const void* buffer, uint64_t size), void (COwnerClass::*mfpCloseChild)())
			: m_rOwnerObject(rOwnerObject), m_mfpIsMasterChild(mfpIsMasterChild), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		bool isMasterChild(const CIdentifier& rIdentifier) override
		{
			if (m_mfpIsMasterChild) { return (m_rOwnerObject.*m_mfpIsMasterChild)(rIdentifier); }
			return false;
		}

		void openChild(const CIdentifier& rIdentifier) override
		{
			if (m_mfpOpenChild) { (m_rOwnerObject.*m_mfpOpenChild)(rIdentifier); }
		}

		void processChildData(const void* buffer, const uint64_t size) override
		{
			if (m_mfpProcessChildData) { (m_rOwnerObject.*m_mfpProcessChildData)(buffer, size); }
		}

		void closeChild() override
		{
			if (m_mfpCloseChild) { (m_rOwnerObject.*m_mfpCloseChild)(); }
		}

	protected:
		COwnerClass& m_rOwnerObject;
		bool (COwnerClass::*m_mfpIsMasterChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpOpenChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpProcessChildData)(const void* buffer, uint64_t size);
		void (COwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass, bool (COwnerClass::*mfpIsMasterChild)(const CIdentifier& rIdentifier), void (COwnerClass::*mfpOpenChild)(const CIdentifier& rIdentifier), void (COwnerClass::*mfpProcessChildData)(const void* buffer, uint64_t size), void (COwnerClass::*mfpCloseChild)()>
	class TReaderCallbackProxy2 final : public IReaderCallback
	{
	public:
		TReaderCallbackProxy2(COwnerClass& rOwnerObject)
			: m_rOwnerObject(rOwnerObject), m_mfpIsMasterChild(mfpIsMasterChild), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		bool isMasterChild(const CIdentifier& rIdentifier) override
		{
			if (m_mfpIsMasterChild) { return (m_rOwnerObject.*m_mfpIsMasterChild)(rIdentifier); }
			return false;
		}

		void openChild(const CIdentifier& rIdentifier) override
		{
			if (m_mfpOpenChild) { (m_rOwnerObject.*m_mfpOpenChild)(rIdentifier); }
		}

		void processChildData(const void* buffer, const uint64_t size) override
		{
			if (m_mfpProcessChildData) { (m_rOwnerObject.*m_mfpProcessChildData)(buffer, size); }
		}

		void closeChild() override
		{
			if (m_mfpCloseChild) { (m_rOwnerObject.*m_mfpCloseChild)(); }
		}

	protected:
		COwnerClass& m_rOwnerObject;
		bool (COwnerClass::*m_mfpIsMasterChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpOpenChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpProcessChildData)(const void* buffer, uint64_t size);
		void (COwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//
} // namespace EBML
