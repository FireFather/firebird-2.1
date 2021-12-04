#pragma once
#include <nmmintrin.h>

#if defined(_WIN64) && defined(_MSC_VER)
inline DWORD LSB (uint64_t b)
    {
    DWORD index;
    _BitScanForward64(&index, b);
    return (index);
    }
inline DWORD MSB(uint64_t b )
    {
    DWORD index;
    _BitScanReverse64(&index, b);
    return (index);
    }
static __inline uint64_t POPCNT(const uint64_t b)
{
	return (_mm_popcnt_u64(b));
}
#endif
