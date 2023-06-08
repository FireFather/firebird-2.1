#pragma once
#ifdef _MSC_VER
#include <nmmintrin.h>

static int LSB(const uint64_t x)
{
	DWORD y;
	_BitScanForward64(&y, x);
	return (int)y;
}

static int MSB(const uint64_t x)
{
	DWORD y;
	_BitScanReverse64(&y, x);
	return (int)y;
}

static INLINE int POPCNT(uint64_t x)
{
	return (int)_mm_popcnt_u64(x);
}
#endif
