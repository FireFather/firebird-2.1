#include "firebird.h"

static void BlackFromWhite()
{
	for (int sq = A1; sq <= H8; sq++)
	{
		PieceSquareValue[bEnumP][sq] = -PieceSquareValue[wEnumP][H8 - sq];
		PieceSquareValue[bEnumN][sq] = -PieceSquareValue[wEnumN][H8 - sq];
		PieceSquareValue[bEnumBL][sq] = -PieceSquareValue[wEnumBL][H8 - sq];
		PieceSquareValue[bEnumBD][sq] = -PieceSquareValue[wEnumBD][H8 - sq];
		PieceSquareValue[bEnumR][sq] = -PieceSquareValue[wEnumR][H8 - sq];
		PieceSquareValue[bEnumQ][sq] = -PieceSquareValue[wEnumQ][H8 - sq];
		PieceSquareValue[bEnumK][sq] = -PieceSquareValue[wEnumK][H8 - sq];
	}
}

static int ValueOpenP(const int sq)
{
	const int C[8] =
	{
		-20, -8, -2, 5, 5, -2, -8, -20
	};

	const int T[8] =
	{
		0, -3, -2, -1, 1, 2, 3, 0
	};

	const int file = FILE(sq), rank = RANK(sq);

	if (rank == R1 || rank == R8)
		return 0;
	return C[file] + T[rank];
}

static int ValueEndP(const int sq)
{
	const int C[8] =
	{
		-4, -6, -8, -10, -10, -8, -6, -4
	};

	const int T[8] =
	{
		0, -3, -3, -2, -1, 0, 2, 0
	};

	const int file = FILE(sq), rank = RANK(sq);

	if (rank == R1 || rank == R8)
		return 0;
	return C[file] + T[rank];
}

static int ValueOpenN(const int sq)
{
	const int T[8] =
	{
		-32, -10, 6, 15, 21, 19, 10, -11
	};

	const int C[8] =
	{
		-26, -10, 1, 5, 5, 1, -10, -26
	};

	const int rank = RANK(sq), file = FILE(sq);
	return sq == A8 || sq == H8 ? -120 : T[rank] + C[file];
}

static int ValueEndN(const int sq)
{
	const int V[8] =
	{
		2, 1, 0, -1, -2, -4, -7, -10
	};

	const int T[8] =
	{
		-10, -5, -2, 1, 3, 5, 2, -3
	};

	const int C[8] =
	{
		-4, -1, 2, 4, 4, 2, -1, -4
	};

	const int rank = RANK(sq), file = FILE(sq);
	const int d = ABS(file - rank), e = ABS(file + rank - 7);
	return V[d] + V[e] + T[rank] + C[file];
}

static int ValueOpenB(const int sq)
{
	const int V[8] =
	{
		10, 5, 1, -3, -5, -7, -8, -12
	};

	const int W[8] =
	{
		-5, 0, 0, 0, 0, 0, 0, 0
	};

	const int rank = RANK(sq), file = FILE(sq);
	const int d = ABS(file - rank), e = ABS(file + rank - 7);
	return V[d] + V[e] + W[rank];
}

static int ValueEndB(const int sq)
{
	const int V[8] =
	{
		3, 2, 0, 0, -2, -2, -3, -3
	};

	const int rank = RANK(sq), file = FILE(sq);
	const int d = ABS(file - rank), e = ABS(file + rank - 7);
	return V[d] + V[e];
}

static int ValueOpenR(const int sq)
{
	const int C[8] =
	{
		-4, 0, 4, 8, 8, 4, 0, -4
	};

	return C[FILE(sq)];
}

static int ValueEndR(const int sq)
{
	const int T[8] =
	{
		0, 0, 0, 0, 1, 1, 1, -2
	};

	return T[RANK(sq)];
}

static int ValueOpenQ(const int sq)
{
	const int V[8] =
	{
		3, 2, 1, 0, -2, -4, -7, -10
	};

	const int W[8] =
	{
		-2, 0, 1, 2, 2, 1, 0, -2
	};

	const int rank = RANK(sq), file = FILE(sq);
	const int d = ABS(file - rank), e = ABS(file + rank - 7);
	return V[d] + V[e] + W[rank] + W[file] - 5 * (rank == R1);
}

static int ValueEndQ(const int sq)
{
	const int V[8] =
	{
		1, 0, -1, -3, -4, -6, -8, -12
	};

	const int W[8] =
	{
		-2, 0, 1, 2, 2, 1, 0, -2
	};

	const int rank = RANK(sq), file = FILE(sq);
	const int d = ABS(file - rank), e = ABS(file + rank - 7);
	return V[d] + V[e] + W[rank] + W[file];
}

static int ValueOpenK(const int sq)
{
	const int T[8] =
	{
		4, 1, -2, -5, -10, -15, -25, -35
	};

	const int C[8] =
	{
		40, 45, 15, -5, -5, 15, 45, 40
	};

	const int rank = RANK(sq), file = FILE(sq);
	return T[rank] + C[file];
}

static int ValueEndK(const int sq)
{
	const int V[8] =
	{
		2, 0, -2, -5, -8, -12, -20, -30
	};

	const int T[8] =
	{
		-30, -5, 0, 5, 10, 5, 0, -10
	};

	const int C[8] =
	{
		-15, 0, 10, 15, 15, 10, 0, -15
	};

	const int rank = RANK(sq), file = FILE(sq);
	const int d = ABS(file - rank), e = ABS(file + rank - 7);
	return V[d] + V[e] + T[rank] + C[file];
}

void InitStatic()
{
	int sq;

	for (sq = A1; sq <= H8; sq++)
		PieceSquareValue[wEnumP][sq] = Score(ValueOpenP(sq), ValueEndP(sq));

	for (sq = A1; sq <= H8; sq++)
		PieceSquareValue[wEnumN][sq] = Score(ValueOpenN(sq), ValueEndN(sq));

	for (sq = A1; sq <= H8; sq++)
		PieceSquareValue[wEnumBL][sq] = Score(ValueOpenB(sq), ValueEndB(sq));

	for (sq = A1; sq <= H8; sq++)
		PieceSquareValue[wEnumBD][sq] = Score(ValueOpenB(sq), ValueEndB(sq));

	for (sq = A1; sq <= H8; sq++)
		PieceSquareValue[wEnumR][sq] = Score(ValueOpenR(sq), ValueEndR(sq));

	for (sq = A1; sq <= H8; sq++)
		PieceSquareValue[wEnumQ][sq] = Score(ValueOpenQ(sq), ValueEndQ(sq));

	for (sq = A1; sq <= H8; sq++)
		PieceSquareValue[wEnumK][sq] = Score(ValueOpenK(sq), ValueEndK(sq));
	BlackFromWhite();
}
