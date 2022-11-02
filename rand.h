#pragma once
static uint64_t randkey = 1;

static uint16_t RAND16()
{
	randkey = randkey * 8765432181103515245ULL + 1234567891ULL;
	return (randkey >> 32) % 65536;
}

static uint64_t GetRand()
{
	return (uint64_t)RAND16() << 48 |
		(uint64_t)RAND16() << 32 |
		(uint64_t)RAND16() << 16 |
		(uint64_t)RAND16() << 0;
}

typedef struct
{
	uint64_t RandKey;
	uint8_t pad[56];
} RAND;

static RAND Rand[MaxCPUs];

static uint32_t Random32(const int cpu)
{
	Rand[cpu].RandKey = Rand[cpu].RandKey * 0x7913cc52088a6cfULL + 0x99f2e6bb0313ca0dULL;
	return Rand[cpu].RandKey >> 18 & 0xffffffff;
}

static void InitRandom32(uint64_t x)
{
	for (int cpu = 0; cpu < MaxCPUs; cpu++)
	{
		x = x * 0xb18ec564ff729005ULL + 0x86ee25701b5e244fULL;
		Rand[cpu].RandKey = x;
	}
}
