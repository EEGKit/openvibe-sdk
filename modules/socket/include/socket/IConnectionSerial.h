#pragma once

#include "IConnection.h"
#include <string>

namespace Socket
{
	class Socket_API IConnectionSerial : public IConnection
	{
	public:

		virtual bool connect(const char* sURL, unsigned long ui32BaudRate) = 0;
		
		/**
		 * \brief Return the input serial pending byte count.
		 * \return The number of pending byte.
		 */
		virtual uint32_t getPendingByteCount() = 0;

		/**
		 * \brief Flush the input serial buffer.
		 * \return If the function succeeds, the return value is true, else false.
		 */
		virtual bool flush() = 0;

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

	extern Socket_API IConnectionSerial* createConnectionSerial();
};


