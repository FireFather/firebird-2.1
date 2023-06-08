#ifndef BUILD_TOP_ANALYSIS
#define BUILD_TOP_ANALYSIS
#include "firebird.h"
#define IsCheck   \
 (Position->wtm ? \
   (wBitboardK & Position->Current->bAtt) : (bBitboardK & Position->Current->wAtt))

typeRootMoveList RootMoveList[512];
#include "top_analysis.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyTopAnalysis(typePos* Position)
{
	int i, v;
	int move_depth = 0;
	uint32_t move, Hash_MOVE = 0;
	typeMoveList *ml, ML[512];
	typeRootMoveList* p;
	const typePosition* TempPosition = Position->Current;
	const int PieceValue[16] =
	{
		0, 1, 3, 0, 3, 3, 5, 9, 0, 1, 3, 0, 3, 3, 5, 9
	};
	int sm;

	EVAL(0);

	if (IsCheck)
		ml = MyEvasion(Position, ML, 0xffffffffffffffff);
	else
	{
		typeMoveList* mlx = MyCapture(Position, ML, OppOccupied);
		ml = MyOrdinary(Position, mlx);
		SortOrdinary(ml, mlx, 0, 0, 0);
	}

	const int k = Position->Current->Hash & HashMask;

	for (i = 0; i < 4; i++)
	{
		const typeHash* rank = HashTable + (k + i);

		if ((rank->hash ^ Position->Current->Hash >> 32) == 0)
		{
			const int trans_depth = rank->DepthLower;
			move = rank->move;

			if (IsExact(rank))
			{
				int EXACT_DEPTH = trans_depth;
				uint32_t EXACT_MOVE = move;
				int Value = HashUpperBound(rank);
			}

			if (move && trans_depth > move_depth)
			{
				move_depth = trans_depth;
				Hash_MOVE = move;
			}
		}
	}

	for (i = 0; i < ml - ML; i++)
		RootMoveList[i].move = ML[i].move;
	RootMoveList[ml - ML].move = MoveNone;
	typeRootMoveList* list = RootMoveList + (ml - ML);
	typeRootMoveList* q = RootMoveList;

	for (p = RootMoveList; p < list; p++)
	{
		move = p->move & 0x7fff;
		Make(Position, move);
		EVAL(0);

		if (IllegalMove)
		{
			Undo(Position, move);
			continue;
		}

		if (Analysing && DoSearchMoves)
		{
			sm = 0;

			while (SearchMoves[sm])
			{
				if (SearchMoves[sm] == move)
				{
					q++->move = move & 0x7fff;
					break;
				}
				sm++;
			}
		}
		else
			q++->move = move & 0x7fff;
		Undo(Position, move);
	}
	q->move = 0;
	list = q;

	for (p = RootMoveList; p < list; p++)
	{
		if (Position->sq[To(p->move)])
		{
			const uint32_t to = Position->sq[To(p->move)];
			const uint32_t fr = Position->sq[From(p->move)];
			p->move |= 0xff000000 + ((16 * PieceValue[to] - PieceValue[fr]) << 16);
		}
	}

	for (p = RootMoveList; p < list; p++)
		if (p->move == Hash_MOVE)
			p->move |= 0xffff0000;

	for (p = list - 1; p >= RootMoveList; p--)
	{
		move = p->move;

		for (q = p + 1; q < list; q++)
		{
			if (move < q->move)
				(q - 1)->move = q->move;
			else
				break;
		}
		q--;
		q->move = move;
	}

	int L = -ValueMate;
	int U = ValueMate;

	if (!RootMoveList[0].move)
	{
		if (IsCheck)
		{
			RootScore = L;
		}
		else
		{
			RootScore = 0;
		}
		RootBestMove = 0;
		RootDepth = 0;
		return;
	}

	for (int depth = 2; depth <= 250; depth += 2)
	{
		if (depth >= 14 && RootScore <= 25000 && -25000 <= RootScore && MultiPV == 1)
		{
			int A = 8;
			L = RootScore - A;
			U = RootScore + A;

			if (L < -25000)
				L = -ValueMate;

			if (U > 25000)
				U = ValueMate;
		AGAIN:
			v = MyAnalysis(Position, L, U, depth);

			if (v > L && v < U)
				goto CHECK_DONE;

			if (v <= L)
			{
				L -= A;
				A += A / 2;
				RootScore = L;
				goto AGAIN;
			}
			U += A;
			A += A / 2;
			RootScore = U;
			goto AGAIN;
		}
		MyAnalysis(Position, -ValueMate, ValueMate, depth);
	CHECK_DONE:
		RootPrevious = RootScore;
		CheckDone(Position, depth / 2);
	}
}
