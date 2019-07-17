#include "system/ovCMemory.h"

#include <cstring>

using namespace System;

bool Memory::copy(void* pTargetBuffer, const void* pSourceBuffer, const uint64 ui64BufferSize)
{
	// TODO take 64bits size into consideration
	if (ui64BufferSize == 0) { return true; }

	::memcpy(pTargetBuffer, pSourceBuffer, static_cast<size_t>(ui64BufferSize));

	return true;
}

bool Memory::move(void* pTargetBuffer, const void* pSourceBuffer, const uint64 ui64BufferSize)
{
	// $$$ TODO take 64bits size into consideration
	if (ui64BufferSize == 0) { return true; }

	::memmove(pTargetBuffer, pSourceBuffer, static_cast<size_t>(ui64BufferSize));

	return true;
}

bool Memory::set(void* pTargetBuffer, const uint64 ui64BufferSize, const uint8 ui8Value)
{
	if (ui64BufferSize == 0) { return true; }
	// $$$ TODO take 64bits size into consideration
	::memset(pTargetBuffer, ui8Value, static_cast<size_t>(ui64BufferSize));
	return true;
}

bool Memory::compare(const void* pSourceBuffer1, const void* pSourceBuffer2, const uint64 ui64BufferSize)
{
	if (ui64BufferSize == 0) { return true; }
	// $$$ TODO take 64bits size into consideration
	return ::memcmp(pSourceBuffer1, pSourceBuffer2, static_cast<size_t>(ui64BufferSize)) == 0;
}

// ________________________________________________________________________________________________________________
//

template <typename T>
bool __bigEndianToHost(const uint8* pBuffer, T* pValue)
{
	if (!pBuffer) return false;
	if (!pValue) return false;
	memset(pValue, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		((uint8*)pValue)[i] = pBuffer[sizeof(T) - 1 - i];
	}
	return true;
}

template <typename T>
bool __littleEndianToHost(const uint8* pBuffer, T* pValue)
{
	if (!pBuffer) return false;
	if (!pValue) return false;
	memset(pValue, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		((uint8*)pValue)[i] = pBuffer[i];
	}
	return true;
}

template <typename T>
bool __hostToBigEndian(const T& rValue, uint8* pBuffer)
{
	if (!pBuffer) return false;
	memset(pBuffer, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		pBuffer[i] = ((uint8*)&rValue)[sizeof(T) - 1 - i];
	}
	return true;
}

template <typename T>
bool __hostToLittleEndian(const T& rValue, uint8* pBuffer)
{
	if (!pBuffer) return false;
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		pBuffer[i] = static_cast<uint8>((rValue >> (i * 8)) & 0xff);
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

bool Memory::hostToLittleEndian(const uint16 ui16Value, uint8* pBuffer)
{
	return __hostToLittleEndian<uint16>(ui16Value, pBuffer);
}

bool Memory::hostToLittleEndian(const uint32 ui32Value, uint8* pBuffer)
{
	return __hostToLittleEndian<uint32>(ui32Value, pBuffer);
}

bool Memory::hostToLittleEndian(const uint64 ui64Value, uint8* pBuffer)
{
	return __hostToLittleEndian<uint64>(ui64Value, pBuffer);
}

bool Memory::hostToLittleEndian(const int16 i16Value, uint8* pBuffer)
{
	return __hostToLittleEndian<int16>(i16Value, pBuffer);
}

bool Memory::hostToLittleEndian(const int32 i32Value, uint8* pBuffer)
{
	return __hostToLittleEndian<int32>(i32Value, pBuffer);
}

bool Memory::hostToLittleEndian(const int64 i64Value, uint8* pBuffer)
{
	return __hostToLittleEndian<int64>(i64Value, pBuffer);
}

bool Memory::hostToLittleEndian(const float f32Value, uint8* pBuffer)
{
	uint32 ui32Value;
	::memcpy(&ui32Value, &f32Value, sizeof(ui32Value));
	return hostToLittleEndian(ui32Value, pBuffer);
}

bool Memory::hostToLittleEndian(const double f64Value, uint8* pBuffer)
{
	uint64 ui64Value;
	::memcpy(&ui64Value, &f64Value, sizeof(ui64Value));
	return hostToLittleEndian(ui64Value, pBuffer);
}

bool Memory::hostToLittleEndian(const float80 f80Value, uint8* pBuffer)
{
	// $$$ TODO
	return false;
}


// ________________________________________________________________________________________________________________
//

bool Memory::hostToBigEndian(const uint16 ui16Value, uint8* pBuffer)
{
	return __hostToBigEndian<uint16>(ui16Value, pBuffer);
}

bool Memory::hostToBigEndian(const uint32 ui32Value, uint8* pBuffer)
{
	return __hostToBigEndian<uint32>(ui32Value, pBuffer);
}

bool Memory::hostToBigEndian(const uint64 ui64Value, uint8* pBuffer)
{
	return __hostToBigEndian<uint64>(ui64Value, pBuffer);
}

bool Memory::hostToBigEndian(const int16 i16Value, uint8* pBuffer)
{
	return __hostToBigEndian<int16>(i16Value, pBuffer);
}

bool Memory::hostToBigEndian(const int32 i32Value, uint8* pBuffer)
{
	return __hostToBigEndian<int32>(i32Value, pBuffer);
}

bool Memory::hostToBigEndian(const int64 i64Value, uint8* pBuffer)
{
	return __hostToBigEndian<int64>(i64Value, pBuffer);
}

bool Memory::hostToBigEndian(const float f32Value, uint8* pBuffer)
{
	uint32 ui32Value;
	::memcpy(&ui32Value, &f32Value, sizeof(ui32Value));
	return hostToBigEndian(ui32Value, pBuffer);
}

bool Memory::hostToBigEndian(const double f64Value, uint8* pBuffer)
{
	uint64 ui64Value;
	::memcpy(&ui64Value, &f64Value, sizeof(ui64Value));
	return hostToBigEndian(ui64Value, pBuffer);
}

bool Memory::hostToBigEndian(const float80 f80Value, uint8* pBuffer)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::littleEndianToHost(const uint8* pBuffer, uint16* pValue)
{
	return __littleEndianToHost<uint16>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8* pBuffer, uint32* pValue)
{
	return __littleEndianToHost<uint32>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8* pBuffer, uint64* pValue)
{
	return __littleEndianToHost<uint64>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8* pBuffer, int16* pValue)
{
	return __littleEndianToHost<int16>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8* pBuffer, int32* pValue)
{
	return __littleEndianToHost<int32>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8* pBuffer, int64* pValue)
{
	return __littleEndianToHost<int64>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8* pBuffer, float* pValue)
{
	uint32 ui32Value;
	bool b = __littleEndianToHost<uint32>(pBuffer, &ui32Value);
	::memcpy(pValue, &ui32Value, sizeof(float));
	return b;
}

bool Memory::littleEndianToHost(const uint8* pBuffer, double* pValue)
{
	uint64 ui64Value;
	bool b = __littleEndianToHost<uint64>(pBuffer, &ui64Value);
	::memcpy(pValue, &ui64Value, sizeof(double));
	return b;
}

bool Memory::littleEndianToHost(const uint8* pBuffer, float80* pValue)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::bigEndianToHost(const uint8* pBuffer, uint16* pValue)
{
	return __bigEndianToHost<uint16>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8* pBuffer, uint32* pValue)
{
	return __bigEndianToHost<uint32>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8* pBuffer, uint64* pValue)
{
	return __bigEndianToHost<uint64>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8* pBuffer, int16* pValue)
{
	return __bigEndianToHost<int16>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8* pBuffer, int32* pValue)
{
	return __bigEndianToHost<int32>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8* pBuffer, int64* pValue)
{
	return __bigEndianToHost<int64>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8* pBuffer, float* pValue)
{
	uint32 ui32Value;
	bool b = __bigEndianToHost<uint32>(pBuffer, &ui32Value);
	::memcpy(pValue, &ui32Value, sizeof(float));
	return b;
}

bool Memory::bigEndianToHost(const uint8* pBuffer, double* pValue)
{
	uint64 ui64Value;
	bool b = __bigEndianToHost<uint64>(pBuffer, &ui64Value);
	::memcpy(pValue, &ui64Value, sizeof(double));
	return b;
}

bool Memory::bigEndianToHost(const uint8* pBuffer, float80* pValue)
{
	// $$$ TODO
	return false;
}
