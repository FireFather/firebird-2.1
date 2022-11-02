#include <stdbool.h>

#include "firebird.h"
jmp_buf J;

static int MovesInTC;
static int OldMTG;
static int64_t TotalTimeWhite;
static int64_t TotalTimeBlack;
static int AbsoluteFactor = 25;
static int BattleFactor = 100;
static int DesiredMillis = 40;
static int EasyFactor = 15;
static int EasyFactorPonder = 33;
static int NormalFactor = 75;

void ponderhit(void)
{
	if (!DoPonder)
		return;
	PonderHit = true;
	DoPonder = false;
	NewPonderHit = true;

	if (EasyMove)
		HaltSearch();
}

void HaltSearch(void)
{
	Stop = true;

	if (JumpIsSet)
		longjmp(J, 1);
}

void Info(const int64_t x)
{
	uint64_t nps, nodes = 0;

	for (int cpu = 0; cpu < NumThreads; cpu++)
		for (int rp = 0; rp < RPperCPU; rp++)
			nodes += RootPosition[cpu][rp].nodes;

	const uint64_t time = x / 1000;

	if (time == 0)
		nps = 0;
	else
		nps = nodes / time * 1000;

	Send("info time %I64u nodes %I64u nps %I64u\n", time, nodes, nps);
}

void CheckDone(typePos* Position, const int d)
{
	if (!JumpIsSet)
		return;

	const int64_t x = GetClock() - StartClock;

	if (d == Depth)
		HaltSearch();

	if (ExtraInfo)
	{
		if (x - LastMessage > 1000000)
			Info(x);
	}

	if (DoPonder)
		goto End;

	if (DoInfinite)
		goto End;

	if (d >= 1 && d < 8)
		goto End;

	if (x > AbsoluteTime)
		HaltSearch();

	if (d == 0 && !NewPonderHit)
		goto End;
	NewPonderHit = false;

	if (!BadMove && x >= BattleTime)
		HaltSearch();

	if (EasyMove && x >= EasyTime)
		HaltSearch();

	if (!BattleMove && x >= OrdinaryTime && !BadMove)
		HaltSearch();
End:
	while (TryInput())
		Input(Position);
}

static uint32_t FullMove(const typePos* Position, uint32_t x)
{
	const int to = To(x), fr = From(x);

	if (!x)
		return x;

	const int pi = Position->sq[fr];

	if (pi == wEnumK || pi == bEnumK)
	{
		if (to - fr == 2 || fr - to == 2)
			x |= FlagOO;
	}

	if (To(x) != 0 && To(x) == Position->Current->ep && (pi == wEnumP || pi == bEnumP))
		x |= FlagEP;
	return x;
}

static uint32_t numeric_move(const typePos* Position, const char* str)
{
	int x = FullMove(Position, (str[2] - 'a') + ((str[3] - '1') << 3) + ((str[0] - 'a') << 6) + ((str[1] - '1') << 9));

	if (str[4] == 'b')
		x |= FlagPromB;

	if (str[4] == 'n')
		x |= FlagPromN;

	if (str[4] == 'r')
		x |= FlagPromR;

	if (str[4] == 'q')
		x |= FlagPromQ;
	return x;
}

void TimeManager(const int64_t Time, int64_t Increment, int MTG)
{
	if (MTG)
	{
		if (MTG > 25)
			MTG = 25;
		DesiredTime = Time / MTG + Increment;
		AbsoluteTime = Time * MTG / ((MTG << 2) - 3) - MIN(1000000, Time / 10);
		if (MTG == 1)
			AbsoluteTime -= MIN(1000000, AbsoluteTime / 10);
		if (AbsoluteTime < 1000)
			AbsoluteTime = 1000;
	}
	else
	{
		AbsoluteTime = Time * AbsoluteFactor / 100 - 10000;
		if (AbsoluteTime < 1000)
			AbsoluteTime = 1000;
		DesiredTime = Time * DesiredMillis / 1000 + Increment;
	}
	if (DesiredTime > AbsoluteTime)
		DesiredTime = AbsoluteTime;
	if (DesiredTime < 1000)
		DesiredTime = 1000;
	EasyTime = DesiredTime * EasyFactor / 100;
	if (DoPonder)
		EasyTime = DesiredTime * EasyFactorPonder / 100;
	BattleTime = DesiredTime * BattleFactor / 100;
	OrdinaryTime = DesiredTime * NormalFactor / 100;
}

void InitSearch(const typePos* Position, char* str)
{
	int64_t wtime = Infinite, btime = Infinite;
	int winc = 0;
	int binc = 0;
	int MTG = 0;
	int sm = 0;
	Depth = 255;
	AbsoluteTime = DesiredTime = Infinite;
	Stop = false;
	DoInfinite = false;
	DoPonder = false;
	NewPonderHit = false;
	DoSearchMoves = false;
	LastMessage = 0;

	const char* p = strtok(str, " ");

	for (StrTok(p); p != NULL; StrTok(p))
	{
		if (!strcmp(p, "depth"))
		{
			StrTok(p);
			Depth = MAX(1, atoi(p));
		}
		else if (!strcmp(p, "movetime"))
		{
			StrTok(p);
			AbsoluteTime = MAX(1, atoll(p)) * 1000 - 10000;
		}
		else if (!strcmp(p, "wtime"))
		{
			StrTok(p);
			wtime = atoll(p) * 1000;
		}
		else if (!strcmp(p, "winc"))
		{
			StrTok(p);
			winc = atoll(p) * 1000;
		}
		else if (!strcmp(p, "btime"))
		{
			StrTok(p);
			btime = atoll(p) * 1000;
		}
		else if (!strcmp(p, "binc"))
		{
			StrTok(p);
			binc = atoll(p) * 1000;
		}
		else if (!strcmp(p, "movestogo"))
		{
			StrTok(p);
			MTG = atoi(p);
		}

		else if (!strcmp(p, "infinite"))
			DoInfinite = true;

		else if (!strcmp(p, "ponder"))
			DoPonder = true;

		else if (!strcmp(p, "searchmoves"))
			DoSearchMoves = true;

		else if (DoSearchMoves)
			SearchMoves[sm++] = numeric_move(Position, p);

		else
		{
		}
	}

	BattleTime = Infinite;
	OrdinaryTime = Infinite;
	EasyTime = Infinite;

	if (IsNewGame || MTG > OldMTG)
	{
		MovesInTC = MTG;
		TotalTimeWhite = MAX(wtime - 500000, (95 * wtime) / 100);
		TotalTimeBlack = MAX(btime - 500000, (95 * btime) / 100);
	}

	int64_t Time = Position->wtm ? wtime : btime;

	if (Time < 0)
		Time = 0;
	if (Time == Infinite)
		goto End;

	Increment = Position->wtm ? winc : binc;

	TimeManager(Time, Increment, MTG);

End:
	if (AbsoluteTime == Infinite)
		Analysing = true;
	else
		Analysing = false;

	if (DoSearchMoves)
		SearchMoves[sm] = MoveNone;
}
