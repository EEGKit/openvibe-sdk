#include "IConnectionSerialDelegate.h"

namespace Socket
{
	IConnectionSerialDelegate::IConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate) : TConnectionDelegate<IConnectionSerial>(oConnectionSerialDelegate.oConnectionDelegate) { }

	IConnectionSerialDelegate::~IConnectionSerialDelegate() { }

	class CConnectionSerialDelegate : public Socket::IConnectionSerialDelegate
	{
	public:
		explicit CConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate) : IConnectionSerialDelegate(oConnectionSerialDelegate)
		{
			m_oConnectionSerialDelegate = oConnectionSerialDelegate;
		}

		virtual ~CConnectionSerialDelegate() { }

		bool connect(const char* sURL, unsigned long ui32BaudRate)
		{
			return m_oConnectionSerialDelegate.fpConnect(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData, sURL, ui32BaudRate);
		}

		uint32 getPendingByteCount() { return m_oConnectionSerialDelegate.fpGetPendingByteCount(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData); }

		bool flush() { return m_oConnectionSerialDelegate.fpFlush(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData); }

		const char* getLastError() { return m_oConnectionSerialDelegate.fpGetLastError(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData); }

		bool isErrorRaised()
		{
			return false;
			// return m_oConnectionSerialDelegate.fpIsErrorRaised(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData);
		}

		void clearError()
		{
			return;
			// return m_oConnectionSerialDelegate.fpClearError(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData);
		}

		bool setTimeouts(unsigned long ui32DecisecondsTimeout)
		{
			return true;
			// return m_oConnectionSerialDelegate.fpSetTimeouts(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData, ui32DecisecondsTimeout);
		}
	};

	IConnectionSerialDelegate* createConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate)
	{
		return new CConnectionSerialDelegate(oConnectionSerialDelegate);
	}
}
