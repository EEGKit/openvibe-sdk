#pragma once

#include "defines.h"

namespace System
{
	class System_API Memory
	{
	public:

		static bool copy(void* pTargetBuffer, const void* pSourceBuffer, uint64_t size);
		static bool move(void* pTargetBuffer, const void* pSourceBuffer, uint64_t size);
		static bool set(void* pTargetBuffer, uint64_t size, uint8_t value);
		static bool compare(const void* pSourceBuffer1, const void* pSourceBuffer2, uint64_t size);

		static bool hostToLittleEndian(uint16_t value, uint8_t* buffer);
		static bool hostToLittleEndian(uint32_t value, uint8_t* buffer);
		static bool hostToLittleEndian(uint64_t value, uint8_t* buffer);
		static bool hostToLittleEndian(int16_t value, uint8_t* buffer);
		static bool hostToLittleEndian(int value, uint8_t* buffer);
		static bool hostToLittleEndian(int64_t value, uint8_t* buffer);
		static bool hostToLittleEndian(float value, uint8_t* buffer);
		static bool hostToLittleEndian(double value, uint8_t* buffer);
		static bool hostToLittleEndian(long double value, uint8_t* buffer);
		static bool hostToBigEndian(uint16_t value, uint8_t* buffer);
		static bool hostToBigEndian(uint32_t value, uint8_t* buffer);
		static bool hostToBigEndian(uint64_t value, uint8_t* buffer);
		static bool hostToBigEndian(int16_t value, uint8_t* buffer);
		static bool hostToBigEndian(int value, uint8_t* buffer);
		static bool hostToBigEndian(int64_t value, uint8_t* buffer);
		static bool hostToBigEndian(float value, uint8_t* buffer);
		static bool hostToBigEndian(double value, uint8_t* buffer);
		static bool hostToBigEndian(long double value, uint8_t* buffer);

		static bool littleEndianToHost(const uint8_t* buffer, uint16_t* value);
		static bool littleEndianToHost(const uint8_t* buffer, uint32_t* value);
		static bool littleEndianToHost(const uint8_t* buffer, uint64_t* value);
		static bool littleEndianToHost(const uint8_t* buffer, int16_t* value);
		static bool littleEndianToHost(const uint8_t* buffer, int* value);
		static bool littleEndianToHost(const uint8_t* buffer, int64_t* value);
		static bool littleEndianToHost(const uint8_t* buffer, float* value);
		static bool littleEndianToHost(const uint8_t* buffer, double* value);
		static bool littleEndianToHost(const uint8_t* buffer, long double* value);
		static bool bigEndianToHost(const uint8_t* buffer, uint16_t* value);
		static bool bigEndianToHost(const uint8_t* buffer, uint32_t* value);
		static bool bigEndianToHost(const uint8_t* buffer, uint64_t* value);
		static bool bigEndianToHost(const uint8_t* buffer, int16_t* value);
		static bool bigEndianToHost(const uint8_t* buffer, int* value);
		static bool bigEndianToHost(const uint8_t* buffer, int64_t* value);
		static bool bigEndianToHost(const uint8_t* buffer, float* value);
		static bool bigEndianToHost(const uint8_t* buffer, double* value);
		static bool bigEndianToHost(const uint8_t* buffer, long double* value);

	private:

		Memory();
	};
} // namespace System
