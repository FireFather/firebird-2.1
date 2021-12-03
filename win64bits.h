#pragma once
#include <nmmintrin.h>

#if defined(_WIN64) && defined(_MSC_VER)
static int LSB(const UINT64 x )
    {
    int y;
    _BitScanForward64(&y, x);
    return y;
    }
static int MSB(const UINT64 x )
    {
    int y;
    _BitScanReverse64(&y, x);
    return y;
    }
static __inline int POPCNT(const UINT64 b)
{
	return (_mm_popcnt_u64(b));
}
#endif
