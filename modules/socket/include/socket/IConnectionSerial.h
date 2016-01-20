#ifndef __Socket_IConnectionSerial_H__
#define __Socket_IConnectionSerial_H__

#include "IConnection.h"
#include <string>

namespace Socket
{
	class Socket_API IConnectionSerial: public Socket::IConnection
	{
	public:

		virtual boolean connect(const char* sURL, unsigned long ui32BaudRate) = 0;
		
		/**
		 * \brief Return the input serial pending byte count.
		 * \return The number of pending byte.
		 */
		virtual uint32 getPendingByteCount() = 0;

		/**
		 * \brief Flush the input serial buffer.
		 * \return If the function succeeds, the return value is true, else false.
		 */
		virtual boolean flush() = 0;

		/**
		 * \brief Flush the input serial buffer.
		 * \return If the function succeeds, the return value is true, else false.
		 */
		virtual const char* getLastError() = 0;

		/**
		 * \brief Checks if an error raised.
		 * \return True if the m_sLastError is not empty, else false.
		 */
		virtual bool isErrorRaised() = 0;

		/**
		 * \brief Clear the last error registered.
		 */
		virtual void clearError() = 0;

		/**
		 * \brief Set tiemouts for read and write function.
		 * \param ui32DecisecondsTimeout [in] Timeout in deciseconds.
		 * \return True if succeed, else false.
		 */
		virtual bool setTimeouts(unsigned long ui32DecisecondsTimeout) = 0;

	protected:
		void saveLastError();

	};

	extern Socket_API Socket::IConnectionSerial* createConnectionSerial(void);
};

#endif // __Socket_IConnectionSerial_H__
