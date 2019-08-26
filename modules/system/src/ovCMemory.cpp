#include "system/ovCMemory.h"

#include <cstring>

using namespace System;

bool Memory::copy(void* pTargetBuffer, const void* pSourceBuffer, const uint64_t ui64BufferSize)
{
	// TODO take 64bits size into consideration
	if (ui64BufferSize == 0) { return true; }

	memcpy(pTargetBuffer, pSourceBuffer, size_t(ui64BufferSize));

	return true;
}

bool Memory::move(void* pTargetBuffer, const void* pSourceBuffer, const uint64_t ui64BufferSize)
{
	// $$$ TODO take 64bits size into consideration
	if (ui64BufferSize == 0) { return true; }

	memmove(pTargetBuffer, pSourceBuffer, size_t(ui64BufferSize));

	return true;
}

bool Memory::set(void* pTargetBuffer, const uint64_t ui64BufferSize, const uint8_t ui8Value)
{
	if (ui64BufferSize == 0) { return true; }
	// $$$ TODO take 64bits size into consideration
	memset(pTargetBuffer, ui8Value, size_t(ui64BufferSize));
	return true;
}

bool Memory::compare(const void* pSourceBuffer1, const void* pSourceBuffer2, const uint64_t ui64BufferSize)
{
	if (ui64BufferSize == 0) { return true; }
	// $$$ TODO take 64bits size into consideration
	return memcmp(pSourceBuffer1, pSourceBuffer2, size_t(ui64BufferSize)) == 0;
}

// ________________________________________________________________________________________________________________
//

template <typename T>
bool __bigEndianToHost(const uint8_t* pBuffer, T* pValue)
{
	if (!pBuffer) return false;
	if (!pValue) return false;
	memset(pValue, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		((uint8_t*)pValue)[i] = pBuffer[sizeof(T) - 1 - i];
	}
	return true;
}

template <typename T>
bool __littleEndianToHost(const uint8_t* pBuffer, T* pValue)
{
	if (!pBuffer) return false;
	if (!pValue) return false;
	memset(pValue, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++) { ((uint8_t*)pValue)[i] = pBuffer[i]; }
	return true;
}

template <typename T>
bool __hostToBigEndian(const T& rValue, uint8_t* pBuffer)
{
	if (!pBuffer) return false;
	memset(pBuffer, 0, sizeof(T));
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		pBuffer[i] = ((uint8_t*)&rValue)[sizeof(T) - 1 - i];
	}
	return true;
}

template <typename T>
bool __hostToLittleEndian(const T& rValue, uint8_t* pBuffer)
{
	if (!pBuffer) return false;
	for (unsigned int i = 0; i < sizeof(T); i++)
	{
		pBuffer[i] = uint8_t((rValue >> (i * 8)) & 0xff);
	}
	return true;
}

// ________________________________________________________________________________________________________________
//

bool Memory::hostToLittleEndian(const uint16_t ui16Value, uint8_t* pBuffer)
{
	return __hostToLittleEndian<uint16_t>(ui16Value, pBuffer);
}

bool Memory::hostToLittleEndian(const uint32_t ui32Value, uint8_t* pBuffer)
{
	return __hostToLittleEndian<uint32_t>(ui32Value, pBuffer);
}

bool Memory::hostToLittleEndian(const uint64_t ui64Value, uint8_t* pBuffer)
{
	return __hostToLittleEndian<uint64_t>(ui64Value, pBuffer);
}

bool Memory::hostToLittleEndian(const int16_t i16Value, uint8_t* pBuffer)
{
	return __hostToLittleEndian<int16_t>(i16Value, pBuffer);
}

bool Memory::hostToLittleEndian(const int i32Value, uint8_t* pBuffer)
{
	return __hostToLittleEndian<int>(i32Value, pBuffer);
}

bool Memory::hostToLittleEndian(const int64_t i64Value, uint8_t* pBuffer)
{
	return __hostToLittleEndian<int64_t>(i64Value, pBuffer);
}

bool Memory::hostToLittleEndian(const float f32Value, uint8_t* pBuffer)
{
	uint32_t ui32Value;
	memcpy(&ui32Value, &f32Value, sizeof(ui32Value));
	return hostToLittleEndian(ui32Value, pBuffer);
}

bool Memory::hostToLittleEndian(const double f64Value, uint8_t* pBuffer)
{
	uint64_t ui64Value;
	memcpy(&ui64Value, &f64Value, sizeof(ui64Value));
	return hostToLittleEndian(ui64Value, pBuffer);
}

bool Memory::hostToLittleEndian(const long double f80Value, uint8_t* pBuffer)
{
	// $$$ TODO
	return false;
}


// ________________________________________________________________________________________________________________
//

bool Memory::hostToBigEndian(const uint16_t ui16Value, uint8_t* pBuffer)
{
	return __hostToBigEndian<uint16_t>(ui16Value, pBuffer);
}

bool Memory::hostToBigEndian(const uint32_t ui32Value, uint8_t* pBuffer)
{
	return __hostToBigEndian<uint32_t>(ui32Value, pBuffer);
}

bool Memory::hostToBigEndian(const uint64_t ui64Value, uint8_t* pBuffer)
{
	return __hostToBigEndian<uint64_t>(ui64Value, pBuffer);
}

bool Memory::hostToBigEndian(const int16_t i16Value, uint8_t* pBuffer)
{
	return __hostToBigEndian<int16_t>(i16Value, pBuffer);
}

bool Memory::hostToBigEndian(const int i32Value, uint8_t* pBuffer)
{
	return __hostToBigEndian<int>(i32Value, pBuffer);
}

bool Memory::hostToBigEndian(const int64_t i64Value, uint8_t* pBuffer)
{
	return __hostToBigEndian<int64_t>(i64Value, pBuffer);
}

bool Memory::hostToBigEndian(const float f32Value, uint8_t* pBuffer)
{
	uint32_t ui32Value;
	memcpy(&ui32Value, &f32Value, sizeof(ui32Value));
	return hostToBigEndian(ui32Value, pBuffer);
}

bool Memory::hostToBigEndian(const double f64Value, uint8_t* pBuffer)
{
	uint64_t ui64Value;
	memcpy(&ui64Value, &f64Value, sizeof(ui64Value));
	return hostToBigEndian(ui64Value, pBuffer);
}

bool Memory::hostToBigEndian(const long double f80Value, uint8_t* pBuffer)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::littleEndianToHost(const uint8_t* pBuffer, uint16_t* pValue)
{
	return __littleEndianToHost<uint16_t>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, uint32_t* pValue)
{
	return __littleEndianToHost<uint32_t>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, uint64_t* pValue)
{
	return __littleEndianToHost<uint64_t>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, int16_t* pValue)
{
	return __littleEndianToHost<int16_t>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, int* pValue)
{
	return __littleEndianToHost<int>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, int64_t* pValue)
{
	return __littleEndianToHost<int64_t>(pBuffer, pValue);
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, float* pValue)
{
	uint32_t ui32Value;
	bool b = __littleEndianToHost<uint32_t>(pBuffer, &ui32Value);
	memcpy(pValue, &ui32Value, sizeof(float));
	return b;
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, double* pValue)
{
	uint64_t ui64Value;
	bool b = __littleEndianToHost<uint64_t>(pBuffer, &ui64Value);
	memcpy(pValue, &ui64Value, sizeof(double));
	return b;
}

bool Memory::littleEndianToHost(const uint8_t* pBuffer, long double* pValue)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::bigEndianToHost(const uint8_t* pBuffer, uint16_t* pValue)
{
	return __bigEndianToHost<uint16_t>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, uint32_t* pValue)
{
	return __bigEndianToHost<uint32_t>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, uint64_t* pValue)
{
	return __bigEndianToHost<uint64_t>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, int16_t* pValue)
{
	return __bigEndianToHost<int16_t>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, int* pValue)
{
	return __bigEndianToHost<int>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, int64_t* pValue)
{
	return __bigEndianToHost<int64_t>(pBuffer, pValue);
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, float* pValue)
{
	uint32_t ui32Value;
	bool b = __bigEndianToHost<uint32_t>(pBuffer, &ui32Value);
	memcpy(pValue, &ui32Value, sizeof(float));
	return b;
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, double* pValue)
{
	uint64_t ui64Value;
	bool b = __bigEndianToHost<uint64_t>(pBuffer, &ui64Value);
	memcpy(pValue, &ui64Value, sizeof(double));
	return b;
}

bool Memory::bigEndianToHost(const uint8_t* pBuffer, long double* pValue)
{
	// $$$ TODO
	return false;
}
