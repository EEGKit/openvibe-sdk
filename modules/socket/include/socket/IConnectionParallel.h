#ifndef __Socket_IConnectionParallel_H__
#define __Socket_IConnectionParallel_H__

#include "IConnection.h"
#include <string>

#if defined TARGET_OS_Windows
	#include <windows.h> 
#endif

namespace Socket
{
	/**
	 * \brief The IConnectionParallel class provides the possibility to communicate with a parallel port.
	 * On Windows, you must have TVicPort library installed (available for free: http://entechtaiwan.com/dev/port/index.shtm).
	 */
	class Socket_API IConnectionParallel : public Socket::IConnection
	{
	public:

		virtual boolean connect(unsigned short ui16PortNumber) = 0;

		virtual std::string getLastError() = 0;

	protected:

#if defined TARGET_OS_Windows
		typedef bool (CALLBACK * LPFNTVICPORTOPEN)();
		typedef void (CALLBACK * LPFNTVICPORTCLOSE)();
		typedef bool (CALLBACK * LPFNTVICPORTISDRIVEROPENED)();
		typedef bool (CALLBACK * LPFNTVICPORTWRITE)(unsigned short, unsigned char);

		HMODULE m_hmodTVicPort;

		LPFNTVICPORTOPEN m_lpfnTVicPortOpen;
		LPFNTVICPORTCLOSE m_lpfnTVicPortClose;
		LPFNTVICPORTISDRIVEROPENED m_lpfnTVicIsDriverOpened;
		LPFNTVICPORTWRITE m_lpfnTVicPortWrite;
#endif
	};

	extern Socket_API Socket::IConnectionParallel* createConnectionParallel(void);
};

#endif // __Socket_IConnectionParallel_H__
