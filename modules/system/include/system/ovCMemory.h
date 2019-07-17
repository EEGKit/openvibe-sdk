#pragma once

#include "defines.h"

namespace System
{
	class System_API Memory
	{
	public:

		static bool copy(void* pTargetBuffer, const void* pSourceBuffer, const System::uint64 ui64BufferSize);
		static bool move(void* pTargetBuffer, const void* pSourceBuffer, const System::uint64 ui64BufferSize);
		static bool set(void* pTargetBuffer, const System::uint64 ui64BufferSize, const System::uint8 ui8Value);
		static bool compare(const void* pSourceBuffer1, const void* pSourceBuffer2, const System::uint64 ui64BufferSize);

		static bool hostToLittleEndian(const System::uint16 ui16Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const System::uint32 ui32Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const System::uint64 ui64Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const System::int16 i16Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const System::int32 i32Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const System::int64 i64Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const float f32Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const double f64Value, System::uint8* pBuffer);
		static bool hostToLittleEndian(const System::float80 f80Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const System::uint16 ui16Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const System::uint32 ui32Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const System::uint64 ui64Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const System::int16 i16Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const System::int32 i32Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const System::int64 i64Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const float f32Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const double f64Value, System::uint8* pBuffer);
		static bool hostToBigEndian(const System::float80 f80Value, System::uint8* pBuffer);

		static bool littleEndianToHost(const System::uint8* pBuffer, System::uint16* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, System::uint32* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, System::uint64* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, System::int16* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, System::int32* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, System::int64* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, float* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, double* pValue);
		static bool littleEndianToHost(const System::uint8* pBuffer, System::float80* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, System::uint16* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, System::uint32* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, System::uint64* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, System::int16* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, System::int32* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, System::int64* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, float* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, double* pValue);
		static bool bigEndianToHost(const System::uint8* pBuffer, System::float80* pValue);

	private:

		Memory(void);
	};
};
