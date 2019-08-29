#pragma once

#include "defines.h"

namespace System
{
	class System_API Memory
	{
	public:

		static bool copy(void* pTargetBuffer, const void* pSourceBuffer, uint64_t size);
		static bool move(void* pTargetBuffer, const void* pSourceBuffer, uint64_t size);
		static bool set(void* pTargetBuffer, uint64_t size, uint8_t ui8Value);
		static bool compare(const void* pSourceBuffer1, const void* pSourceBuffer2, uint64_t size);

		static bool hostToLittleEndian(uint16_t ui16Value, uint8_t* buffer);
		static bool hostToLittleEndian(uint32_t ui32Value, uint8_t* buffer);
		static bool hostToLittleEndian(uint64_t ui64Value, uint8_t* buffer);
		static bool hostToLittleEndian(int16_t i16Value, uint8_t* buffer);
		static bool hostToLittleEndian(int i32Value, uint8_t* buffer);
		static bool hostToLittleEndian(int64_t i64Value, uint8_t* buffer);
		static bool hostToLittleEndian(float f32Value, uint8_t* buffer);
		static bool hostToLittleEndian(double f64Value, uint8_t* buffer);
		static bool hostToLittleEndian(long double f80Value, uint8_t* buffer);
		static bool hostToBigEndian(uint16_t ui16Value, uint8_t* buffer);
		static bool hostToBigEndian(uint32_t ui32Value, uint8_t* buffer);
		static bool hostToBigEndian(uint64_t ui64Value, uint8_t* buffer);
		static bool hostToBigEndian(int16_t i16Value, uint8_t* buffer);
		static bool hostToBigEndian(int i32Value, uint8_t* buffer);
		static bool hostToBigEndian(int64_t i64Value, uint8_t* buffer);
		static bool hostToBigEndian(float f32Value, uint8_t* buffer);
		static bool hostToBigEndian(double f64Value, uint8_t* buffer);
		static bool hostToBigEndian(long double f80Value, uint8_t* buffer);

		static bool littleEndianToHost(const uint8_t* buffer, uint16_t* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, uint32_t* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, uint64_t* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, int16_t* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, int* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, int64_t* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, float* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, double* pValue);
		static bool littleEndianToHost(const uint8_t* buffer, long double* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, uint16_t* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, uint32_t* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, uint64_t* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, int16_t* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, int* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, int64_t* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, float* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, double* pValue);
		static bool bigEndianToHost(const uint8_t* buffer, long double* pValue);

	private:

		Memory();
	};
} // namespace System
