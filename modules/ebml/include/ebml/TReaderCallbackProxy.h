#pragma once

#include "IReader.h"

namespace EBML
{

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass>
	class TReaderCallbackProxy1 : public IReaderCallback
	{
	public:
		TReaderCallbackProxy1(COwnerClass& rOwnerObject, bool (COwnerClass::*mfpIsMasterChild)(const CIdentifier& rIdentifier), void (COwnerClass::*mfpOpenChild)(const CIdentifier& rIdentifier),
							  void (COwnerClass::*mfpProcessChildData)(const void* pBuffer, uint64_t ui64BufferSize), void (COwnerClass::*mfpCloseChild)())
			: m_rOwnerObject(rOwnerObject), m_mfpIsMasterChild(mfpIsMasterChild), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		virtual bool isMasterChild(const CIdentifier& rIdentifier)
		{
			if (m_mfpIsMasterChild) { return (m_rOwnerObject.*m_mfpIsMasterChild)(rIdentifier); }
			return false;
		}

		virtual void openChild(const CIdentifier& rIdentifier)
		{
			if (m_mfpOpenChild) { (m_rOwnerObject.*m_mfpOpenChild)(rIdentifier); }
		}

		virtual void processChildData(const void* pBuffer, const uint64_t ui64BufferSize)
		{
			if (m_mfpProcessChildData) { (m_rOwnerObject.*m_mfpProcessChildData)(pBuffer, ui64BufferSize); }
		}

		virtual void closeChild()
		{
			if (m_mfpCloseChild) { (m_rOwnerObject.*m_mfpCloseChild)(); }
		}

	protected:
		COwnerClass& m_rOwnerObject;
		bool (COwnerClass::*m_mfpIsMasterChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpOpenChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpProcessChildData)(const void* pBuffer, uint64_t ui64BufferSize);
		void (COwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//

	template <class COwnerClass, bool (COwnerClass::*mfpIsMasterChild)(const CIdentifier& rIdentifier), void (COwnerClass::*mfpOpenChild)(const CIdentifier& rIdentifier), void (COwnerClass::*mfpProcessChildData)(const void* pBuffer, uint64_t ui64BufferSize), void (COwnerClass::*mfpCloseChild)()>
	class TReaderCallbackProxy2 : public IReaderCallback
	{
	public:
		TReaderCallbackProxy2(COwnerClass& rOwnerObject)
			: m_rOwnerObject(rOwnerObject), m_mfpIsMasterChild(mfpIsMasterChild), m_mfpOpenChild(mfpOpenChild), m_mfpProcessChildData(mfpProcessChildData), m_mfpCloseChild(mfpCloseChild) { }

		virtual bool isMasterChild(const CIdentifier& rIdentifier)
		{
			if (m_mfpIsMasterChild) { return (m_rOwnerObject.*m_mfpIsMasterChild)(rIdentifier); }
			return false;
		}

		virtual void openChild(const CIdentifier& rIdentifier)
		{
			if (m_mfpOpenChild) { (m_rOwnerObject.*m_mfpOpenChild)(rIdentifier); }
		}

		virtual void processChildData(const void* pBuffer, const uint64_t ui64BufferSize)
		{
			if (m_mfpProcessChildData) { (m_rOwnerObject.*m_mfpProcessChildData)(pBuffer, ui64BufferSize); }
		}

		virtual void closeChild()
		{
			if (m_mfpCloseChild) { (m_rOwnerObject.*m_mfpCloseChild)(); }
		}

	protected:
		COwnerClass& m_rOwnerObject;
		bool (COwnerClass::*m_mfpIsMasterChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpOpenChild)(const CIdentifier& rIdentifier);
		void (COwnerClass::*m_mfpProcessChildData)(const void* pBuffer, uint64_t ui64BufferSize);
		void (COwnerClass::*m_mfpCloseChild)();
	};

	// ________________________________________________________________________________________________________________
	//
}  // namespace EBML
