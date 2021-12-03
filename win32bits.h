#pragma once
#include "firebird.h"

#if !defined(_WIN64) && !defined(_MSC_VER)
static __inline int LSB( UINT64 x )
    {
    _asm
        {
        bsf eax, dword Ptr x[0]
		jnz f_end
		bsf eax, dword Ptr x[4]
		jz f_end
		add eax, 20h
	f_end:
        }
    }
static __inline int MSB( UINT64 x )
    {
    _asm
        {
        bsr	eax, dword Ptr x[4]
		jz	l_lo
		add eax, 20h
		jmp	l_end
	l_lo: bsr eax, dword Ptr x[0]
	l_end:
        }
    }

static __inline int POPCNT( UINT64 v )
    {
	auto v1 = (unsigned int)(v & 0xFFFFFFFF);
    v1 -= (v1 >> 1) & 0x55555555;
    v1 = (v1 & 0x33333333) + ((v1 >> 2) & 0x33333333);
    v1 = (v1 + (v1 >> 4)) & 0x0F0F0F0F;
	    auto v2 = (unsigned int)(v >> 32);
    v2 -= (v2 >> 1) & 0x55555555;
    v2 = (v2 & 0x33333333) + ((v2 >> 2) & 0x33333333);
    v2 = (v2 + (v2 >> 4)) & 0x0F0F0F0F;
    return ((v1 * 0x01010101) >> 24) + ((v2 * 0x01010101) >> 24);
    }
#endif
