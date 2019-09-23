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

bool Memory::set(void* pTargetBuffer, const uint64_t size, const uint8_t value)
{
	if (size == 0) { return true; }
	// $$$ TODO take 64bits size into consideration
	memset(pTargetBuffer, value, size_t(size));
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
bool __bigEndianToHost(const uint8_t* buffer, T* value)
{
	if (!buffer) { return false; }
	if (!value) { return false; }
	memset(value, 0, sizeof(T));
	for (uint32_t i = 0; i < sizeof(T); i++) { ((uint8_t*)value)[i] = buffer[sizeof(T) - 1 - i]; }
	return true;
}

template <typename T>
bool __littleEndianToHost(const uint8_t* buffer, T* value)
{
	if (!buffer) { return false; }
	if (!value) { return false; }
	memset(value, 0, sizeof(T));
	for (uint32_t i = 0; i < sizeof(T); i++) { ((uint8_t*)value)[i] = buffer[i]; }
	return true;
}

template <typename T>
bool __hostToBigEndian(const T& value, uint8_t* buffer)
{
	if (!buffer) { return false; }
	memset(buffer, 0, sizeof(T));
	for (uint32_t i = 0; i < sizeof(T); i++) { buffer[i] = ((uint8_t*)&value)[sizeof(T) - 1 - i]; }
	return true;
}

template <typename T>
bool __hostToLittleEndian(const T& value, uint8_t* buffer)
{
	if (!buffer) { return false; }
	for (uint32_t i = 0; i < sizeof(T); i++) { buffer[i] = uint8_t((value >> (i * 8)) & 0xff); }
	return true;
}

// ________________________________________________________________________________________________________________
//

bool Memory::hostToLittleEndian(const uint16_t value, uint8_t* buffer) { return __hostToLittleEndian<uint16_t>(value, buffer); }

bool Memory::hostToLittleEndian(const uint32_t value, uint8_t* buffer) { return __hostToLittleEndian<uint32_t>(value, buffer); }

bool Memory::hostToLittleEndian(const uint64_t value, uint8_t* buffer) { return __hostToLittleEndian<uint64_t>(value, buffer); }

bool Memory::hostToLittleEndian(const int16_t value, uint8_t* buffer) { return __hostToLittleEndian<int16_t>(value, buffer); }

bool Memory::hostToLittleEndian(const int value, uint8_t* buffer) { return __hostToLittleEndian<int>(value, buffer); }

bool Memory::hostToLittleEndian(const int64_t value, uint8_t* buffer) { return __hostToLittleEndian<int64_t>(value, buffer); }

bool Memory::hostToLittleEndian(const float value, uint8_t* buffer)
{
	uint32_t tmp;
	memcpy(&tmp, &value, sizeof(tmp));
	return hostToLittleEndian(tmp, buffer);
}

bool Memory::hostToLittleEndian(const double value, uint8_t* buffer)
{
	uint64_t tmp;
	memcpy(&tmp, &value, sizeof(tmp));
	return hostToLittleEndian(tmp, buffer);
}

bool Memory::hostToLittleEndian(const long double /*value*/, uint8_t* /*buffer*/)
{
	// $$$ TODO
	return false;
}


// ________________________________________________________________________________________________________________
//

bool Memory::hostToBigEndian(const uint16_t value, uint8_t* buffer) { return __hostToBigEndian<uint16_t>(value, buffer); }

bool Memory::hostToBigEndian(const uint32_t value, uint8_t* buffer) { return __hostToBigEndian<uint32_t>(value, buffer); }

bool Memory::hostToBigEndian(const uint64_t value, uint8_t* buffer) { return __hostToBigEndian<uint64_t>(value, buffer); }

bool Memory::hostToBigEndian(const int16_t value, uint8_t* buffer) { return __hostToBigEndian<int16_t>(value, buffer); }

bool Memory::hostToBigEndian(const int value, uint8_t* buffer) { return __hostToBigEndian<int>(value, buffer); }

bool Memory::hostToBigEndian(const int64_t value, uint8_t* buffer) { return __hostToBigEndian<int64_t>(value, buffer); }

bool Memory::hostToBigEndian(const float value, uint8_t* buffer)
{
	uint32_t tmp;
	memcpy(&tmp, &value, sizeof(tmp));
	return hostToBigEndian(tmp, buffer);
}

bool Memory::hostToBigEndian(const double value, uint8_t* buffer)
{
	uint64_t tmp;
	memcpy(&tmp, &value, sizeof(tmp));
	return hostToBigEndian(tmp, buffer);
}

bool Memory::hostToBigEndian(const long double /*value*/, uint8_t* /*buffer*/)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::littleEndianToHost(const uint8_t* buffer, uint16_t* value) { return __littleEndianToHost<uint16_t>(buffer, value); }

bool Memory::littleEndianToHost(const uint8_t* buffer, uint32_t* value) { return __littleEndianToHost<uint32_t>(buffer, value); }

bool Memory::littleEndianToHost(const uint8_t* buffer, uint64_t* value) { return __littleEndianToHost<uint64_t>(buffer, value); }

bool Memory::littleEndianToHost(const uint8_t* buffer, int16_t* value) { return __littleEndianToHost<int16_t>(buffer, value); }

bool Memory::littleEndianToHost(const uint8_t* buffer, int* value) { return __littleEndianToHost<int>(buffer, value); }

bool Memory::littleEndianToHost(const uint8_t* buffer, int64_t* value) { return __littleEndianToHost<int64_t>(buffer, value); }

bool Memory::littleEndianToHost(const uint8_t* buffer, float* value)
{
	uint32_t tmp;
	const bool b = __littleEndianToHost<uint32_t>(buffer, &tmp);
	memcpy(value, &tmp, sizeof(float));
	return b;
}

bool Memory::littleEndianToHost(const uint8_t* buffer, double* value)
{
	uint64_t tmp;
	const bool b = __littleEndianToHost<uint64_t>(buffer, &tmp);
	memcpy(value, &tmp, sizeof(double));
	return b;
}

bool Memory::littleEndianToHost(const uint8_t* /*buffer*/, long double* /*value*/)
{
	// $$$ TODO
	return false;
}

// ________________________________________________________________________________________________________________
//

bool Memory::bigEndianToHost(const uint8_t* buffer, uint16_t* value) { return __bigEndianToHost<uint16_t>(buffer, value); }

bool Memory::bigEndianToHost(const uint8_t* buffer, uint32_t* value) { return __bigEndianToHost<uint32_t>(buffer, value); }

bool Memory::bigEndianToHost(const uint8_t* buffer, uint64_t* value) { return __bigEndianToHost<uint64_t>(buffer, value); }

bool Memory::bigEndianToHost(const uint8_t* buffer, int16_t* value) { return __bigEndianToHost<int16_t>(buffer, value); }

bool Memory::bigEndianToHost(const uint8_t* buffer, int* value) { return __bigEndianToHost<int>(buffer, value); }

bool Memory::bigEndianToHost(const uint8_t* buffer, int64_t* value) { return __bigEndianToHost<int64_t>(buffer, value); }

bool Memory::bigEndianToHost(const uint8_t* buffer, float* value)
{
	uint32_t tmp;
	const bool b = __bigEndianToHost<uint32_t>(buffer, &tmp);
	memcpy(value, &tmp, sizeof(float));
	return b;
}

bool Memory::bigEndianToHost(const uint8_t* buffer, double* value)
{
	uint64_t tmp;
	const bool b = __bigEndianToHost<uint64_t>(buffer, &tmp);
	memcpy(value, &tmp, sizeof(double));
	return b;
}

bool Memory::bigEndianToHost(const uint8_t* /*buffer*/, long double* /*value*/)
{
	// $$$ TODO
	return false;
}
