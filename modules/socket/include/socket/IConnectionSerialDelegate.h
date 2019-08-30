#pragma once

#include "TConnectionDelegate.h"
#include "IConnectionSerial.h"

namespace Socket
{
	struct Socket_API SConnectionSerialDelegate
	{
		SConnectionDelegate oConnectionDelegate;
		bool (*fpConnect)(void*, const char*,  const unsigned long);
		unsigned int (*fpGetPendingByteCount)(void*);
		bool (*fpFlush)(void*);
		const char* (*fpGetLastError)(void*);
		void (*fpSaveLastError)(void*);
		// TODO for Android compatibility
		//bool(*fpIsErrorRaised)(void*); 
		//void(*fpClearError)(void*);
		//bool(*fSetTimeouts)(void*, const uint32_t decisecondsTimeout);
	};

	class Socket_API IConnectionSerialDelegate : public TConnectionDelegate<IConnectionSerial>
	{
	public:
		IConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate);
		~IConnectionSerialDelegate() override;

	protected:
		SConnectionSerialDelegate m_oConnectionSerialDelegate;
	};

	extern Socket_API IConnectionSerialDelegate* createConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate);
} // namespace Socket
