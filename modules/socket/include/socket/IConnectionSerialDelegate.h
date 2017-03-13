#ifndef __Socket_IConnectionSerialDelegate_H__
#define __Socket_IConnectionSerialDelegate_H__

#include "TConnectionDelegate.h"
#include "IConnectionSerial.h"

namespace Socket
{
	struct Socket_API SConnectionSerialDelegate
	{
		SConnectionDelegate oConnectionDelegate;
		bool (*fpConnect)(void*, const char*, unsigned long);
		unsigned int (*fpGetPendingByteCount)(void*);
		bool (*fpFlush)(void*);
		const char* (*fpGetLastError)(void*);
		void (*fpSaveLastError)(void*);
		// TODO for Android compatibility
		//bool(*fpIsErrorRaised)(void*); 
		//void(*fpClearError)(void*);
		//bool(*fSetTimeouts)(void*, unsigned long ui32DecisecondsTimeout);
	};

	class Socket_API IConnectionSerialDelegate : public Socket::TConnectionDelegate<Socket::IConnectionSerial>
	{
	public:
		IConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate);
		virtual ~IConnectionSerialDelegate();

	protected:
		SConnectionSerialDelegate m_oConnectionSerialDelegate;

	};

	extern Socket_API IConnectionSerialDelegate* createConnectionSerialDelegate(SConnectionSerialDelegate oConnectionSerialDelegate);
}

#endif // __Socket_IConnectionSerialDelegate_H__
