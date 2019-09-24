#pragma once

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
	class Socket_API IConnectionParallel : public IConnection
	{
	public:

		virtual bool connect(unsigned short ui16PortNumber) = 0;

		virtual std::string getLastError() = 0;

	protected:

#if defined TARGET_OS_Windows
		typedef bool (CALLBACK * LPFNTVICPORTOPEN)();
		typedef void (CALLBACK * LPFNTVICPORTCLOSE)();
		typedef bool (CALLBACK * LPFNTVICPORTISDRIVEROPENED)();
		typedef bool (CALLBACK * LPFNTVICPORTWRITE)(unsigned short, unsigned char);

		HMODULE m_hmodTVicPort = nullptr;

		LPFNTVICPORTOPEN m_lpfnTVicPortOpen = nullptr;
		LPFNTVICPORTCLOSE m_lpfnTVicPortClose = nullptr;
		LPFNTVICPORTISDRIVEROPENED m_lpfnTVicIsDriverOpened = nullptr;
		LPFNTVICPORTWRITE m_lpfnTVicPortWrite = nullptr;
#endif
	};

	extern Socket_API IConnectionParallel* createConnectionParallel();
} // namespace Socket
