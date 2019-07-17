#pragma once

#include "IConnection.h"

#include <map>
#include <string>

namespace Socket
{
	class Socket_API IConnectionBluetooth : public Socket::IConnection
	{
	public:
	
		/**
		 * \brief Connect to the bluetooth device.
		 * \param[in] ui64BluetoothAddress the MAC address of the Bluetooth device.
		 * \return If the function succeeds, the return value is true, else false.
		 */
		virtual boolean connect(unsigned long long ui64BluetoothAddress) = 0;
		
		/**
		 * \brief Return the input serial pending byte count.
		 * \return The number of pending byte.
		 */
		virtual uint32 getPendingByteCount(void) = 0;

		/**
		 * \brief Flush the input serial buffer.
		 * \return If the function succeeds, the return value is true, else false.
		 */
		virtual const char* getLastError(void) const = 0;

		/**
		 * \brief Clear the last error registered.
		 */
		virtual void clearError(void) = 0;

		/** 
		 * \brief List the paired bluetooth devices: Name and Bluetooth MAC address.
		 *
		 * \param[out] pairedBluetoothDevicesCount the Bluetooth devices count. 
		 * \param[out] bluetoothNames an array of Bluetooth names. 
		 * \param[out] bluetoothAddresses an array of Bluetooth addresses. 
		 *
		 * \return  If the function succeeds, the return value is true, else false.
		 */
		virtual bool listPairedBluetoothDevices(unsigned int* pairedBluetoothDevicesCount, char** bluetoothNames, unsigned long long** bluetoothAddresses) = 0;

		/**
		 * \brief Convert string MAC Bluetooth address to hexadecimal.
		 * \param[out] straddr string MAC Bluetooth address.
		 * \param[out] btaddr hexadecimal MAC Bluetooth address.
		 * \retval true in case of success.
		 * \retval false in case of failure: if the address does not match: %02x:%02x:%02x:%02x:%02x:%02x
		 */
		static bool string2BluetoothAddress(const char* straddr, unsigned long long* btaddr)
		{
			unsigned int aaddr[6];

			int value = sscanf(straddr, "%02x:%02x:%02x:%02x:%02x:%02x",
							   &aaddr[0], &aaddr[1], &aaddr[2],
							   &aaddr[3], &aaddr[4], &aaddr[5]);

			if (value != 6) { return false; }

			*btaddr = 0;

			for (size_t i = 0; i < 6; i++)
			{
				unsigned long long tmpaddr = static_cast<unsigned long long>(aaddr[i] & 0xff);
				*btaddr                    = (*btaddr << 8) + tmpaddr;
			}

			return true;
		}


	protected:

#if defined TARGET_OS_Windows

		static const unsigned char m_ui8WinSocketMajorVersion = 2; // Winsock major version to use
		static const unsigned char m_ui8WinSocketMinorVersion = 2; // Winsock minor version to use

		static bool b_IsWinsockInitialized;

#elif defined TARGET_OS_Linux || defined TARGET_OS_MacOS

#endif
	};

	extern Socket_API Socket::IConnectionBluetooth* createConnectionBluetooth(void);
};
