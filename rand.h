#pragma once
static uint64_t randkey = 1;
static uint16_t RAND16()
    {
    randkey = randkey * 8765432181103515245ULL + 1234567891ULL;
    return (randkey >> 32) % 65536;
    }
static uint64_t GetRand()
    {
	return (uint64_t) RAND16() << 48 |
		(uint64_t) RAND16() << 32 |
		(uint64_t) RAND16() << 16 |
		(uint64_t) RAND16() << 0;
    }

