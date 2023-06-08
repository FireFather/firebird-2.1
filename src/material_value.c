#include "firebird.h"

#define ValueP Value4 (80, 90, 110, 125)
#define ValueN Value4 (265, 280, 320, 355)
#define ValueB Value4 (280, 295, 325, 360)
#define ValueR Value4 (405, 450, 550, 610)
#define ValueQ Value4 (800, 875, 1025, 1150)

#define ValueBP Value4 (35, 40, 50, 55)

#define KnightPawnAdjust Value4 (0, 2, 4, 5)
#define RookPawnAdjust Value4 (5, 4, 2, 0)

#define PhaseMinor (1)
#define PhaseRook (3)
#define PhaseQueen (6)

static int InitFlags(const int wP, const int wN, const int wB, const int wBL, const int wBD, const int wR, const int wQ,
                     const int bP, const int bN, const int bB, const int bBL, const int bBD, const int bR, const int bQ)
{
	uint8_t Flags = (wN || wB || wQ || wR) << 1 | (bN || bB || bQ || bR) << 0;

	if (!wQ && !wR && wB + wN == 1 && wP <= 4 && bP <= 4)
		Flags &= 1;

	if (!bQ && !bR && bB + bN == 1 && wP <= 4 && bP <= 4)
		Flags &= 2;

	if (wQ == 1 && bQ == 1 && !wR && !bR && !wB && !bB && !wN && !bN)
		Flags |= QueenEnding << 2;

	if (wR == 1 && bR == 1 && !wQ && !bQ && !wB && !bB && !wN && !bN)
		Flags |= RookEnding << 2;

	if (wB == 1 && bB == 1 && !wQ && !bQ && !wR && !bR && !wN && !bN)
	{
		if (wBL == 1 && bBD == 1 || wBD == 1 && bBL == 1)
			Flags |= BishopEnding << 2;
		else
			Flags |= OppositeBishopEnding << 2;
		Flags |= (WhiteMinorOnly | BlackMinorOnly) << 2;
	}

	if (wN == 1 && bN == 1 && !wQ && !bQ && !wR && !bR && !wB && !bB)
		Flags |= KnightEnding << 2;

	if (wN == 1 && bB == 1 && !wQ && !bQ && !wR && !bR && !wB && !bN)
		Flags |= BishopKnightEnding << 2;

	if (wB == 1 && bN == 1 && !wQ && !bQ && !wR && !bR && !bB && !wN)
		Flags |= BishopKnightEnding << 2;

	if (wB == 1 && !wQ && !wR && !wN)
		Flags |= WhiteMinorOnly << 2;

	if (bB == 1 && !bQ && !bR && !bN)
		Flags |= BlackMinorOnly << 2;

	if (wN == 1 && !wQ && !wR && !wB)
		Flags |= WhiteMinorOnly << 2;

	if (bN == 1 && !bQ && !bR && !bB)
		Flags |= BlackMinorOnly << 2;

	if (!wN && !wB && !wR && !wQ && !bN && !bB && !bR && !bQ && wP + bP == 1)
		Flags |= PawnEnding << 2;

	if (wN == 1 && wB == 1 && !wR && !wQ && !wP && !bQ && !bR && !bB && !bN && !bP)
		Flags |= BishopKnightMate << 2;

	if (bN == 1 && bB == 1 && !bR && !bQ && !bP && !wQ && !wR && !wB && !wN && !wP)
		Flags |= BishopKnightMate << 2;
	return Flags;
}

static int InitTokens(const int wP, const int wN, const int wB, const int wBL, const int wBD, const int wR, const int wQ,
                      const int bP, const int bN, const int bB, const int bBL, const int bBD, const int bR, const int bQ)
{
	int token = 0x80;

	if (wN == 0 && bN == 0 && wB == 0 && bB == 0 && wR == 0 && bR == 0 && wQ == 1 && bQ == 1)
		token = 0x70 + MAX(wP, bP);

	if (wN == 0 && bN == 0 && wB == 0 && bB == 0 && wQ == 0 && bQ == 0 && wR == 1 && bR == 1)
		token = 0x60 + 2 * MAX(wP, bP);

	if (wN == 0 && bN == 0 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 1 && bB == 1)
	{
		if (wBL == 1 && wBD == 0 && bBL == 0 && bBD == 1 || wBL == 0 && wBD == 1 && bBL == 1 && bBD == 0)
			token = 0x30 + 4 * MAX(wP, bP);
		else
			token = 0x78 + 2 * MAX(wP, bP);
	}

	if (wN == 1 && bN == 1 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 0 && bB == 0)
		token = 0x80 + MAX(wP, bP);

	if (wN == 0 && bN == 0 && wR == 0 && bR == 0 && wQ == 0 && bQ == 0 && wB == 0 && bB == 0)
		token = 0xc0 - 8 * MAX(wP, bP);

	if (wN == 0 && bN == 0 && wB == 1 && bB == 1 && wR == 1 && bR == 1 && wQ == 0 && bQ == 0)
	{
		if (wBL == 1 && wBD == 0 && bBL == 0 && bBD == 1 || wBL == 0 && wBD == 1 && bBL == 1 && bBD == 0)
			token = 0x70 + MAX(wP, bP);
	}
	return token;
}

static int WhiteWeight(int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bBL,
                       int bBD, int bR, int bQ)
{
	int wMinor, bMinor, wPhase, bPhase, wWeight, wValue, bValue;
	wMinor = wB + wN;
	bMinor = bB + bN;
	wPhase = wMinor + 2 * wR + 4 * wQ;
	bPhase = bMinor + 2 * bR + 4 * bQ;
	wValue = 3 * (wB + wN) + 5 * wR + 9 * wQ;
	bValue = 3 * (bB + bN) + 5 * bR + 9 * bQ;
	wWeight = 10;

	if (!wP)
	{
		if (wPhase == 1)
			wWeight = 0;

		if (wPhase == 2)
		{
			if (bPhase == 0)
			{
				if (wN == 2)
				{
					if (bP >= 1)
						wWeight = 3;
					else
						wWeight = 0;
				}
			}

			if (bPhase == 1)
			{
				wWeight = 1;

				if (wB == 2 && bN == 1)
					wWeight = 8;

				if (wR == 1 && bN == 1)
					wWeight = 2;
			}

			if (bPhase == 2)
				wWeight = 1;
		}

		if (wPhase == 3 && wR == 1)
		{
			if (bPhase == 2 && bR == 1)
			{
				if (wN == 1)
					wWeight = 1;

				if (wB == 1)
					wWeight = 1;
			}

			if (bPhase == 2 && bR == 0)
			{
				wWeight = 2;

				if (wB == 1 && bN == 2)
					wWeight = 6;

				if (bN == 1 && (wBL == 1 && bBL == 1 || wBD == 1 && bBD == 1))
					wWeight = 2;

				if (bN == 1 && (wBD == 1 && bBL == 1 || wBL == 1 && bBD == 1))
					wWeight = 7;
			}

			if (bPhase == 3)
				wWeight = 2;
		}

		if (wPhase == 3 && wR == 0)
		{
			if (bPhase == 2 && bR == 1)
			{
				if (wN == 2)
					wWeight = 2;

				if (wB == 2)
					wWeight = 7;
			}

			if (bPhase == 2 && bR == 0)
			{
				wWeight = 2;

				if (wB == 2 && bN == 2)
					wWeight = 4;
			}

			if (bPhase == 3)
				wWeight = 2;
		}

		if (wPhase == 4 && wQ)
		{
			if (bPhase == 2 && bN == 2)
				wWeight = 2;

			if (bPhase == 2 && bN == 1)
				wWeight = 8;

			if (bPhase == 2 && bN == 0)
				wWeight = 7;

			if (bPhase == 3)
				wWeight = 1;

			if (bPhase == 4)
				wWeight = 1;
		}

		if (wPhase == 4 && wR == 2)
		{
			if (bPhase == 2 && bR == 0)
				wWeight = 7;

			if (bPhase == 3)
				wWeight = 2;

			if (bPhase == 4)
				wWeight = 1;
		}

		if (wPhase == 4 && wR == 1)
		{
			if (bPhase == 3 && bR == 1)
				wWeight = 3;

			if (bPhase == 3 && bR == 0)
				wWeight = 2;

			if (bPhase == 4)
				wWeight = 2;
		}

		if (wPhase == 4 && wR == 0 && wQ == 0)
		{
			if (bPhase == 3 && bR == 1)
				wWeight = 4;

			if (bPhase == 3 && bR == 0)
				wWeight = 2;

			if (bPhase == 4 && bQ)
				wWeight = 8;

			if (bPhase == 4 && bQ == 0)
				wWeight = 1;
		}

		if (wPhase == 5 && wQ)
		{
			if (bPhase == 4)
				wWeight = 2;

			if (bPhase == 5)
				wWeight = 1;

			if (bPhase == 4 && bR == 2)
			{
				if (wN)
					wWeight = 3;

				if (wB)
					wWeight = 7;
			}

			if (bPhase == 5)
				wWeight = 1;
		}

		if (wPhase == 5 && wR == 1)
		{
			if (bPhase == 4 && bQ)
				wWeight = 9;

			if (bPhase == 4 && bR == 2)
				wWeight = 7;

			if (bPhase == 4 && bR == 1)
				wWeight = 3;

			if (bPhase == 4 && bQ == 0 && bR == 0)
				wWeight = 1;

			if (bPhase == 5)
				wWeight = 2;
		}

		if (wPhase == 5 && wR == 2)
		{
			if (bPhase == 4 && bQ && wB == 1)
				wWeight = 8;

			if (bPhase == 4 && bQ && wN == 1)
				wWeight = 7;

			if (bPhase == 4 && bR == 2)
				wWeight = 3;

			if (bPhase == 4 && bR == 1)
				wWeight = 2;

			if (bPhase == 4 && bQ == 0 && bR == 0)
				wWeight = 1;

			if (bPhase == 5)
				wWeight = 1;
		}

		if (wPhase == 6 && wQ && wR)
		{
			if (bPhase == 4 && bQ == 0 && bR == 0)
				wWeight = 2;

			if (bPhase == 5 && bQ)
				wWeight = 1;

			if (bPhase == 4 && bR == 1)
				wWeight = 6;

			if (bPhase == 4 && bR == 2)
				wWeight = 3;

			if (bPhase == 5 && bR)
				wWeight = 1;

			if (bPhase == 6)
				wWeight = 1;
		}

		if (wPhase == 6 && wQ && wR == 0)
		{
			if (bPhase == 4 && bQ == 0 && bR == 0)
				wWeight = 5;

			if (bPhase == 5 && bQ)
				wWeight = 2;

			if (bPhase == 5 && bR == 2)
				wWeight = 2;

			if (bPhase == 5 && bR == 1)
				wWeight = 1;

			if (bPhase == 6)
				wWeight = 1;
		}

		if (wPhase == 6 && wQ == 0 && wR == 2)
		{
			if (bPhase == 5 && bQ)
				wWeight = 7;

			if (bPhase == 5 && bR == 1)
				wWeight = 1;

			if (bPhase == 5 && bR == 2)
				wWeight = 2;

			if (bPhase == 6)
				wWeight = 1;
		}

		if (wPhase == 6 && wQ == 0 && wR == 1)
		{
			if (bPhase == 5 && bQ)
				wWeight = 9;

			if (bPhase == 5 && bR == 2)
				wWeight = 3;

			if (bPhase == 5 && bR == 1)
				wWeight = 2;

			if (bPhase == 6)
				wWeight = 1;

			if (bPhase == 6 && bQ)
				wWeight = 2;

			if (bPhase == 6 && bQ && bR)
				wWeight = 4;
		}

		if (wPhase >= 7)
		{
			if (wValue > bValue + 4)
				wWeight = 9;

			if (wValue == bValue + 4)
				wWeight = 7;

			if (wValue == bValue + 3)
				wWeight = 4;

			if (wValue == bValue + 2)
				wWeight = 2;

			if (wValue < bValue + 2)
				wWeight = 1;
		}
	}

	if (wP == 1)
	{
		if (bPhase == 1)
		{
			if (wPhase == 1)
				wWeight = 3;

			if (wPhase == 2 && wN == 2)
			{
				if (bP == 0)
					wWeight = 3;
				else
					wWeight = 5;
			}

			if (wPhase == 2 && wR == 1)
				wWeight = 7;
		}

		if (bPhase == 2 && bR == 1 && wPhase == 2 && wR == 1)
			wWeight = 8;

		if (bPhase == 2 && bR == 0 && wPhase == 2)
			wWeight = 4;

		if (bPhase >= 3 && bMinor > 0 && wPhase == bPhase)
			wWeight = 3;

		if (bPhase >= 3 && bMinor == 0 && wPhase == bPhase)
			wWeight = 5;

		if (bPhase == 4 && bQ == 1 && wPhase == bPhase)
			wWeight = 7;
	}
	return wWeight;
}

static int BlackWeight(int wP, int wN, int wB, int wBL, int wBD, int wR, int wQ, int bP, int bN, int bB, int bBL,
                       int bBD, int bR, int bQ)
{
	int wMinor, bMinor, wPhase, bPhase, bWeight, wValue, bValue;
	wMinor = wB + wN;
	bMinor = bB + bN;
	wPhase = wMinor + 2 * wR + 4 * wQ;
	bPhase = bMinor + 2 * bR + 4 * bQ;
	wValue = 3 * (wB + wN) + 5 * wR + 9 * wQ;
	bValue = 3 * (bB + bN) + 5 * bR + 9 * bQ;
	bWeight = 10;

	if (!bP)
	{
		if (bPhase == 1)
			bWeight = 0;

		if (bPhase == 2)
		{
			if (wPhase == 0)
			{
				if (bN == 2)
				{
					if (bP >= 1)
						bWeight = 3;
					else
						bWeight = 0;
				}
			}

			if (wPhase == 1)
			{
				bWeight = 1;

				if (bB == 2 && wN == 1)
					bWeight = 8;

				if (bR == 1 && wN == 1)
					bWeight = 2;
			}

			if (wPhase == 2)
				bWeight = 1;
		}

		if (bPhase == 3 && bR == 1)
		{
			if (wPhase == 2 && wR == 1)
			{
				if (bN == 1)
					bWeight = 1;

				if (bB == 1)
					bWeight = 1;
			}

			if (wPhase == 2 && wR == 0)
			{
				bWeight = 2;

				if (bB == 1 && wN == 2)
					bWeight = 6;

				if (wN == 1 && (bBL == 1 && wBL == 1 || bBD == 1 && wBD == 1))
					bWeight = 2;

				if (wN == 1 && (bBD == 1 && wBL == 1 || bBL == 1 && wBD == 1))
					bWeight = 7;
			}

			if (wPhase == 3)
				bWeight = 2;
		}

		if (bPhase == 3 && bR == 0)
		{
			if (wPhase == 2 && wR == 1)
			{
				if (bN == 2)
					bWeight = 2;

				if (bB == 2)
					bWeight = 7;
			}

			if (wPhase == 2 && wR == 0)
			{
				bWeight = 2;

				if (bB == 2 && wN == 2)
					bWeight = 4;
			}

			if (wPhase == 3)
				bWeight = 2;
		}

		if (bPhase == 4 && bQ)
		{
			if (wPhase == 2 && wN == 2)
				bWeight = 2;

			if (wPhase == 2 && wN == 1)
				bWeight = 8;

			if (wPhase == 2 && wN == 0)
				bWeight = 7;

			if (wPhase == 3)
				bWeight = 1;

			if (wPhase == 4)
				bWeight = 1;
		}

		if (bPhase == 4 && bR == 2)
		{
			if (wPhase == 2 && wR == 0)
				bWeight = 7;

			if (wPhase == 3)
				bWeight = 2;

			if (wPhase == 4)
				bWeight = 1;
		}

		if (bPhase == 4 && bR == 1)
		{
			if (wPhase == 3 && wR == 1)
				bWeight = 3;

			if (wPhase == 3 && wR == 0)
				bWeight = 2;

			if (wPhase == 4)
				bWeight = 2;
		}

		if (bPhase == 4 && bR == 0 && bQ == 0)
		{
			if (wPhase == 3 && wR == 1)
				bWeight = 4;

			if (wPhase == 3 && wR == 0)
				bWeight = 2;

			if (wPhase == 4 && wQ)
				bWeight = 8;

			if (wPhase == 4 && wQ == 0)
				bWeight = 1;
		}

		if (bPhase == 5 && bQ)
		{
			if (wPhase == 4)
				bWeight = 2;

			if (wPhase == 5)
				bWeight = 1;

			if (wPhase == 4 && wR == 2)
			{
				if (bN)
					bWeight = 3;

				if (bB)
					bWeight = 7;
			}

			if (wPhase == 5)
				bWeight = 1;
		}

		if (bPhase == 5 && bR == 1)
		{
			if (wPhase == 4 && wQ)
				bWeight = 9;

			if (wPhase == 4 && wR == 2)
				bWeight = 7;

			if (wPhase == 4 && wR == 1)
				bWeight = 3;

			if (wPhase == 4 && wQ == 0 && wR == 0)
				bWeight = 1;

			if (wPhase == 5)
				bWeight = 2;
		}

		if (bPhase == 5 && bR == 2)
		{
			if (wPhase == 4 && wQ && bB == 1)
				bWeight = 8;

			if (wPhase == 4 && wQ && bN == 1)
				bWeight = 7;

			if (wPhase == 4 && wR == 2)
				bWeight = 3;

			if (wPhase == 4 && wR == 1)
				bWeight = 2;

			if (wPhase == 4 && wQ == 0 && wR == 0)
				bWeight = 1;

			if (wPhase == 5)
				bWeight = 1;
		}

		if (bPhase == 6 && bQ && bR)
		{
			if (wPhase == 4 && wQ == 0 && wR == 0)
				bWeight = 2;

			if (wPhase == 5 && wQ)
				bWeight = 1;

			if (wPhase == 4 && wR == 1)
				bWeight = 6;

			if (wPhase == 4 && wR == 2)
				bWeight = 3;

			if (wPhase == 5 && wR)
				bWeight = 1;

			if (wPhase == 6)
				bWeight = 1;
		}

		if (bPhase == 6 && bQ && bR == 0)
		{
			if (wPhase == 4 && wQ == 0 && wR == 0)
				bWeight = 5;

			if (wPhase == 5 && wQ)
				bWeight = 2;

			if (wPhase == 5 && wR == 2)
				bWeight = 2;

			if (wPhase == 5 && wR == 1)
				bWeight = 1;

			if (wPhase == 6)
				bWeight = 1;
		}

		if (bPhase == 6 && bQ == 0 && bR == 2)
		{
			if (wPhase == 5 && wQ)
				bWeight = 7;

			if (wPhase == 5 && wR == 1)
				bWeight = 1;

			if (wPhase == 5 && wR == 2)
				bWeight = 2;

			if (wPhase == 6)
				bWeight = 1;
		}

		if (bPhase == 6 && bQ == 0 && bR == 1)
		{
			if (wPhase == 5 && wQ)
				bWeight = 9;

			if (wPhase == 5 && wR == 2)
				bWeight = 3;

			if (wPhase == 5 && wR == 1)
				bWeight = 2;

			if (wPhase == 6)
				bWeight = 1;

			if (wPhase == 6 && wQ)
				bWeight = 2;

			if (wPhase == 6 && wQ && wR)
				bWeight = 4;
		}

		if (bPhase >= 7)
		{
			if (bValue > wValue + 4)
				bWeight = 9;

			if (bValue == wValue + 4)
				bWeight = 7;

			if (bValue == wValue + 3)
				bWeight = 4;

			if (bValue == wValue + 2)
				bWeight = 2;

			if (bValue < wValue + 2)
				bWeight = 1;
		}
	}

	if (bP == 1)
	{
		if (wPhase == 1)
		{
			if (bPhase == 1)
				bWeight = 3;

			if (bPhase == 2 && bN == 2)
			{
				if (wP == 0)
					bWeight = 3;
				else
					bWeight = 5;
			}

			if (bPhase == 2 && bR == 1)
				bWeight = 7;
		}

		if (wPhase == 2 && wR == 1 && bPhase == 2 && bR == 1)
			bWeight = 8;

		if (wPhase == 2 && wR == 0 && bPhase == 2)
			bWeight = 4;

		if (wPhase >= 3 && wMinor > 0 && bPhase == wPhase)
			bWeight = 3;

		if (wPhase >= 3 && wMinor == 0 && bPhase == wPhase)
			bWeight = 5;

		if (wPhase == 4 && wQ == 1 && bPhase == wPhase)
			bWeight = 7;
	}
	return bWeight;
}

static uint64_t ComputeValue(const int wP, const int wN, const int wB, const int wBL, const int wBD, const int wR, const int wQ,
                           const int bP, const int bN, const int bB, const int bBL, const int bBD, const int bR, const int bQ)
{
	uint64_t va = 0;
	const int wMinor = wN + wB;
	const int bMinor = bN + bB;
	va += (wB / 2 - bB / 2) * ValueBP;
	va += (wP - bP) * ValueP;
	va += (wN - bN) * ValueN;
	va += (wR - bR) * ValueR;
	va += (wQ - bQ) * ValueQ;
	va += (wB - bB) * ValueB;

	if (wR == 2)
		va -= Value4(16, 20, 28, 32);

	if (bR == 2)
		va += Value4(16, 20, 28, 32);

	if (wQ + wR >= 2)
		va -= Value4(8, 10, 14, 16);

	if (bQ + bR >= 2)
		va += Value4(8, 10, 14, 16);

	if (wMinor > bMinor)
		va += Value4(20, 15, 10, 5);

	if (bMinor > wMinor)
		va -= Value4(20, 15, 10, 5);
	va -= (wP - 5) * wR * RookPawnAdjust;
	va += (wP - 5) * wN * KnightPawnAdjust;
	va += (bP - 5) * bR * RookPawnAdjust;
	va -= (bP - 5) * bN * KnightPawnAdjust;
	return va;
}

static void CalculateMaterialValue(const int c)
{
	int Value;
	int wt;
	int n = c;
	const int wQ = n % 2;
	n /= 2;
	const int bQ = n % 2;
	n /= 2;
	const int wR = n % 3;
	n /= 3;
	const int bR = n % 3;
	n /= 3;
	const int wBL = n % 2;
	n /= 2;
	const int wBD = n % 2;
	n /= 2;
	const int bBL = n % 2;
	n /= 2;
	const int bBD = n % 2;
	n /= 2;
	const int wN = n % 3;
	n /= 3;
	const int bN = n % 3;
	n /= 3;
	const int wP = n % 9;
	n /= 9;
	const int bP = n % 9;
	const int wB = wBL + wBD;
	const int bB = bBL + bBD;
	uint64_t va = ComputeValue(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
	const int phase = PhaseMinor * (wN + wB + bN + bB) + PhaseRook * (wR + bR) + PhaseQueen * (wQ + bQ);
	int va1 = va & 0xffff;
	int va2 = (va >> 16 & 0xffff) + (va1 > 0x8000);
	va1 = (int16_t)va1;
	int va3 = (va >> 32 & 0xffff) + (va2 > 0x8000);
	va2 = (int16_t)va2;
	int va4 = (va >> 48 & 0xffff) + (va3 > 0x8000);
	va3 = (int16_t)va3;
	va4 = (int16_t)va4;

	if (phase < 8)
	{
		va4 *= 8 - phase;
		va3 *= phase;
		va = va3 + va4;
		Value = (int)va / 8;
	}
	else if (phase < 24)
	{
		va3 *= 24 - phase;
		va2 *= phase - 8;
		va = va2 + va3;
		Value = (int)va / 16;
	}
	else
	{
		va2 *= 32 - phase;
		va1 *= phase - 24;
		va = va1 + va2;
		Value = (int)va / 8;
	}
	const int wWeight = WhiteWeight(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
	const int bWeight = BlackWeight(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);

	if (Value > 0)
		wt = wWeight;
	else
		wt = bWeight;
	Value *= wt;
	Value /= 10;
	Material[c].Value = Value;
	Material[c].token = InitTokens(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
	Material[c].flags = InitFlags(wP, wN, wB, wBL, wBD, wR, wQ, bP, bN, bB, bBL, bBD, bR, bQ);
}

void InitMaterialValue()
{
	for (int c = 0; c < 419904; c++)
		CalculateMaterialValue(c);
}
