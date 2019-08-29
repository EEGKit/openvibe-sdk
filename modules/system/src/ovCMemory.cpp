#include "system/ovCMemory.h"

#include <cstring>

using namespace System;

bool Memory::copy(void* pTargetBuffer, const void* pSourceBuffer, const uint64_t size)
{
	// TODO take 64bits size into consideration
	if (size == 0) { return true; }

	memcpy(pTargetBuffer, pSourceBuffer, size_t(size));

	return true;
}

bool Memory::move(void* pTargetBuffer, const void* pSourceBuffer, const uint64_t size)
{
	// $$$ TODO take 64bits size into consideration
	if (size == 0) { return true; }

	memmove(pTargetBuffer, pSourceBuffer, size_t(size));

	return true;
}

bool Memory::set(void* pTargetBuffer, const uint64_t size, const uint8_t ui8Value)
{
	if (size == 0) { return true; }
	// $$$ TODO take 64bits size into consideration
	memset(pTargetBuffer, ui8Value, size_t(size));
	return true;
}

bool Memory::compare(const void* pSourceBuffer1, const void* pSourceBuffer2, const uint64_t size)
{
	if (size == 0) { return true; }
	// $$$ TODO take 64bits size into consideration
	return memcmp(pSourceBuffer1, pSourceBuffer2, size_t(size)) == 0;
}

// ________________________________________________________________________________________________________________
//

template <typename T>
bool __bigEndianToHost(const uint8_t* buffer, T* pValue)
{
	if (!buffer) return false;
	if (!pValue) return false;
	memset(pValue, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		((uint8_t*)pValue)[i] = buffer[sizeof(T) - 1 - i];
	}
	return true;
}

template <typename T>
bool __littleEndianToHost(const uint8_t* buffer, T* pValue)
{
	if (!buffer) return false;
	if (!pValue) return false;
	memset(pValue, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++) { ((uint8_t*)pValue)[i] = buffer[i]; }
	return true;
}

template <typename T>
bool __hostToBigEndian(const T& rValue, uint8_t* buffer)
{
	if (!buffer) return false;
	memset(buffer, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		buffer[i] = ((uint8_t*)&rValue)[sizeof(T) - 1 - i];
	}
	return true;
}

template <typename T>
bool __hostToLittleEndian(const T& rValue, uint8_t* buffer)
{
	if (!buffer) return false;
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		buffer[i] = uint8_t((rValue >> (i * 8)) & 0xff);
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

bool Memory::hostToLittleEndian(const uint16_t ui16Value, uint8_t* buffer)
{
	return __hostToLittleEndian<uint16_t>(ui16Value, buffer);
}

bool Memory::hostToLittleEndian(const uint32_t ui32Value, uint8_t* buffer)
{
	return __hostToLittleEndian<uint32_t>(ui32Value, buffer);
}

bool Memory::hostToLittleEndian(const uint64_t ui64Value, uint8_t* buffer)
{
	return __hostToLittleEndian<uint64_t>(ui64Value, buffer);
}

bool Memory::hostToLittleEndian(const int16_t i16Value, uint8_t* buffer)
{
	return __hostToLittleEndian<int16_t>(i16Value, buffer);
}

bool Memory::hostToLittleEndian(const int i32Value, uint8_t* buffer)
{
	return __hostToLittleEndian<int>(i32Value, buffer);
}

bool Memory::hostToLittleEndian(const int64_t i64Value, uint8_t* buffer)
{
	return __hostToLittleEndian<int64_t>(i64Value, buffer);
}

bool Memory::hostToLittleEndian(const float f32Value, uint8_t* buffer)
{
	uint32_t ui32Value;
	memcpy(&ui32Value, &f32Value, sizeof(ui32Value));
	return hostToLittleEndian(ui32Value, buffer);
}

bool Memory::hostToLittleEndian(const double f64Value, uint8_t* buffer)
{
	uint64_t ui64Value;
	memcpy(&ui64Value, &f64Value, sizeof(ui64Value));
	return hostToLittleEndian(ui64Value, buffer);
}

bool Memory::hostToLittleEndian(const long double f80Value, uint8_t* buffer)
{
	// $$$ TODO
	return false;
}


// ________________________________________________________________________________________________________________
//

bool Memory::hostToBigEndian(const uint16_t ui16Value, uint8_t* buffer)
{
	return __hostToBigEndian<uint16_t>(ui16Value, buffer);
}

bool Memory::hostToBigEndian(const uint32_t ui32Value, uint8_t* buffer)
{
	return __hostToBigEndian<uint32_t>(ui32Value, buffer);
}

bool Memory::hostToBigEndian(const uint64_t ui64Value, uint8_t* buffer)
{
	return __hostToBigEndian<uint64_t>(ui64Value, buffer);
}

bool Memory::hostToBigEndian(const int16_t i16Value, uint8_t* buffer)
{
	return __hostToBigEndian<int16_t>(i16Value, buffer);
}

bool Memory::hostToBigEndian(const int i32Value, uint8_t* buffer)
{
	return __hostToBigEndian<int>(i32Value, buffer);
}

bool Memory::hostToBigEndian(const int64_t i64Value, uint8_t* buffer)
{
	return __hostToBigEndian<int64_t>(i64Value, buffer);
}

bool Memory::hostToBigEndian(const float f32Value, uint8_t* buffer)
{
	uint32_t ui32Value;
	memcpy(&ui32Value, &f32Value, sizeof(ui32Value));
	return hostToBigEndian(ui32Value, buffer);
}

bool Memory::hostToBigEndian(const double f64Value, uint8_t* buffer)
{
	uint64_t ui64Value;
	memcpy(&ui64Value, &f64Value, sizeof(ui64Value));
	return hostToBigEndian(ui64Value, buffer);
}

bool Memory::hostToBigEndian(const long double f80Value, uint8_t* buffer)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::littleEndianToHost(const uint8_t* buffer, uint16_t* pValue)
{
	return __littleEndianToHost<uint16_t>(buffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* buffer, uint32_t* pValue)
{
	return __littleEndianToHost<uint32_t>(buffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* buffer, uint64_t* pValue)
{
	return __littleEndianToHost<uint64_t>(buffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* buffer, int16_t* pValue)
{
	return __littleEndianToHost<int16_t>(buffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* buffer, int* pValue)
{
	return __littleEndianToHost<int>(buffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* buffer, int64_t* pValue)
{
	return __littleEndianToHost<int64_t>(buffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* buffer, float* pValue)
{
	uint32_t ui32Value;
	bool b = __littleEndianToHost<uint32_t>(buffer, &ui32Value);
	memcpy(pValue, &ui32Value, sizeof(float));
	return b;
}

bool Memory::littleEndianToHost(const uint8_t* buffer, double* pValue)
{
	uint64_t ui64Value;
	bool b = __littleEndianToHost<uint64_t>(buffer, &ui64Value);
	memcpy(pValue, &ui64Value, sizeof(double));
	return b;
}

bool Memory::littleEndianToHost(const uint8_t* buffer, long double* pValue)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::bigEndianToHost(const uint8_t* buffer, uint16_t* pValue)
{
	return __bigEndianToHost<uint16_t>(buffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* buffer, uint32_t* pValue)
{
	return __bigEndianToHost<uint32_t>(buffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* buffer, uint64_t* pValue)
{
	return __bigEndianToHost<uint64_t>(buffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* buffer, int16_t* pValue)
{
	return __bigEndianToHost<int16_t>(buffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* buffer, int* pValue)
{
	return __bigEndianToHost<int>(buffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* buffer, int64_t* pValue)
{
	return __bigEndianToHost<int64_t>(buffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* buffer, float* pValue)
{
	uint32_t ui32Value;
	bool b = __bigEndianToHost<uint32_t>(buffer, &ui32Value);
	memcpy(pValue, &ui32Value, sizeof(float));
	return b;
}

bool Memory::bigEndianToHost(const uint8_t* buffer, double* pValue)
{
	uint64_t ui64Value;
	bool b = __bigEndianToHost<uint64_t>(buffer, &ui64Value);
	memcpy(pValue, &ui64Value, sizeof(double));
	return b;
}

bool Memory::bigEndianToHost(const uint8_t* buffer, long double* pValue)
{
	// $$$ TODO
	return false;
}
