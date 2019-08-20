#include "IConnectionSerialDelegate.h"

namespace Socket
{
	IConnectionSerialDelegate::IConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate) : TConnectionDelegate<IConnectionSerial>(oConnectionSerialDelegate.oConnectionDelegate) { }

	IConnectionSerialDelegate::~IConnectionSerialDelegate() { }

	class CConnectionSerialDelegate : public IConnectionSerialDelegate
	{
	public:
		explicit CConnectionSerialDelegate(SConnectionSerialDelegate connectionSerialDelegate) : IConnectionSerialDelegate(connectionSerialDelegate)
		{
			m_oConnectionSerialDelegate = connectionSerialDelegate;
		}

		~CConnectionSerialDelegate() override { }

		bool connect(const char* url, const unsigned long baudRate) override
		{
			return m_oConnectionSerialDelegate.fpConnect(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData, url, baudRate);
		}

		uint32_t getPendingByteCount() override { return m_oConnectionSerialDelegate.fpGetPendingByteCount(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData); }

		bool flush() override { return m_oConnectionSerialDelegate.fpFlush(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData); }

		const char* getLastError() override { return m_oConnectionSerialDelegate.fpGetLastError(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData); }

		bool isErrorRaised() override { return false; }	// return m_oConnectionSerialDelegate.fpIsErrorRaised(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData);
		void clearError() override { }					// return m_oConnectionSerialDelegate.fpClearError(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData);
		bool setTimeouts(unsigned long ui32DecisecondsTimeout) override { return true; }	// return m_oConnectionSerialDelegate.fpSetTimeouts(m_oConnectionSerialDelegate.oConnectionDelegate.pUserData, ui32DecisecondsTimeout);
	};

	IConnectionSerialDelegate* createConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate) { return new CConnectionSerialDelegate(oConnectionSerialDelegate); }
} // namespace Socket
