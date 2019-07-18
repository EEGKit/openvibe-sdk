#pragma once

#include "defines.h"

namespace System
{
	class System_API Memory
	{
	public:

		static bool copy(void* pTargetBuffer, const void* pSourceBuffer, const uint64_t ui64BufferSize);
		static bool move(void* pTargetBuffer, const void* pSourceBuffer, const uint64_t ui64BufferSize);
		static bool set(void* pTargetBuffer, const uint64_t ui64BufferSize, const uint8_t ui8Value);
		static bool compare(const void* pSourceBuffer1, const void* pSourceBuffer2, const uint64_t ui64BufferSize);

		static bool hostToLittleEndian(const uint16_t ui16Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const uint32_t ui32Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const uint64_t ui64Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const int16_t i16Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const int32_t i32Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const int64_t i64Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const float f32Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const double f64Value, uint8_t* pBuffer);
		static bool hostToLittleEndian(const System::float80 f80Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const uint16_t ui16Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const uint32_t ui32Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const uint64_t ui64Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const int16_t i16Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const int32_t i32Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const int64_t i64Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const float f32Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const double f64Value, uint8_t* pBuffer);
		static bool hostToBigEndian(const System::float80 f80Value, uint8_t* pBuffer);

		static bool littleEndianToHost(const uint8_t* pBuffer, uint16_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, uint32_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, uint64_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, int16_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, int32_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, int64_t* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, float* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, double* pValue);
		static bool littleEndianToHost(const uint8_t* pBuffer, System::float80* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, uint16_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, uint32_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, uint64_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, int16_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, int32_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, int64_t* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, float* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, double* pValue);
		static bool bigEndianToHost(const uint8_t* pBuffer, System::float80* pValue);

	private:

		Memory(void);
	};
};
