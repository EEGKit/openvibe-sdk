#pragma once

#include "defines.h"

namespace System
{
	class System_API Memory
	{
	public:

		static bool copy(void* pTargetBuffer, const void* pSourceBuffer, uint64_t ui64BufferSize);
		static bool move(void* pTargetBuffer, const void* pSourceBuffer, uint64_t ui64BufferSize);
		static bool set(void* pTargetBuffer, uint64_t ui64BufferSize, uint8_t ui8Value);
		static bool compare(const void* pSourceBuffer1, const void* pSourceBuffer2, uint64_t ui64BufferSize);

		static bool hostToLittleEndian(uint16_t ui16Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(uint32_t ui32Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(uint64_t ui64Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(int16_t i16Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(int i32Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(int64_t i64Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(float f32Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(double f64Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(long double f80Value, uint8_t* pBuffer);
		static bool hostToBigEndian(uint16_t ui16Value, uint8_t* pBuffer);
		static bool hostToBigEndian(uint32_t ui32Value, uint8_t* pBuffer);
		static bool hostToBigEndian(uint64_t ui64Value, uint8_t* pBuffer);
		static bool hostToBigEndian(int16_t i16Value, uint8_t* pBuffer);
		static bool hostToBigEndian(int i32Value, uint8_t* pBuffer);
		static bool hostToBigEndian(int64_t i64Value, uint8_t* pBuffer);
		static bool hostToBigEndian(float f32Value, uint8_t* pBuffer);
		static bool hostToBigEndian(double f64Value, uint8_t* pBuffer);
		static bool hostToBigEndian(long double f80Value, uint8_t* pBuffer);

		static bool littleEndianToHost(const uint8_t* pBuffer, uint16_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, uint32_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, uint64_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, int16_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, int* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, int64_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, float* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, double* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, long double* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, uint16_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, uint32_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, uint64_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, int16_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, int* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, int64_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, float* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, double* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, long double* pValue);

	private:

		Memory();
	};
} // namespace System
