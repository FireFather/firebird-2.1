#ifndef BUILD_QSEARCH
#define BUILD_QSEARCH
#include "firebird.h"
#include "qsearch.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyQsearch(typePos* Position, const int VALUE, const int depth)
{
	int Value, i;
	const int k = Position->Current->Hash & HashMask;
	uint32_t move, trans_move = 0, move_depth = 0;
	typeMoveList List[512];
	const typePosition* TempPosition = Position->Current;

	if (VALUE < -ValueMate + 1)
		return -ValueMate + 1;

	if (VALUE > ValueMate - 1)
		return ValueMate - 1;

	CheckRepetition

	for (i = 0; i < 4; i++)
	{
		const typeHash* rank = HashTable + (k + i);

		if ((rank->hash ^ Position->Current->Hash >> 32) == 0)
		{
			if (rank->DepthLower)
			{
				Value = HashLowerBound(rank);

				if (Value >= VALUE)
					return Value;
			}

			if (rank->DepthUpper)
			{
				Value = HashUpperBound(rank);

				if (Value < VALUE)
					return Value;
			}

			const uint32_t trans_depth = rank->DepthLower;
			move = rank->move;

			if (move && trans_depth > move_depth)
			{
				move_depth = trans_depth;
				trans_move = move;
			}
		}
	}

	int best_value = TempPosition->Value + TempoValue2;

	if (best_value >= VALUE)
		return best_value;
	int v = VALUE - PrunePawn;
	uint64_t TARGET = OppOccupied;

	if (v > best_value)
	{
		v = VALUE - PruneMinor;
		TARGET ^= BitboardOppP;

		if (v > best_value)
		{
			TARGET ^= BitboardOppN | BitboardOppB;
			v = VALUE - PruneRook;

			if (v > best_value)
				TARGET ^= BitboardOppR;
		}

		if (BitboardOppP & MyAttacked)
			best_value += PrunePawn;
	}

	const typeMoveList* list = MyCapture(Position, List, TARGET);
	typeMoveList* p = List;

	while (p->move)
	{
		if ((p->move & 0x7fff) == trans_move)
			p->move |= 0xffff0000;
		p++;
	}
	p = List;

	while (p->move)
	{
		move = p->move;
		typeMoveList* q = ++p;

		while (q->move)
		{
			if (move < q->move)
			{
				const uint32_t Temp = q->move;
				q->move = move;
				move = Temp;
			}
			q++;
		}

		if (!EasySEE(move) && (move & 0x7fff) != trans_move && SqSet[From(move)] & ~MyXray && !MySEE(Position, move))
			continue;
		move &= 0x7fff;
		Make(Position, move);
		EvalLazy(VALUE, VALUE, LazyValue, move);

		if (IllegalMove)
		{
			Undo(Position, move);
			continue;
		}

		if (PosIsExact(Position->Current->exact))
			v = -Position->Current->Value;

		else if (MoveIsCheck)
			v = -OppQsearchCheck(Position, 1 - VALUE, depth - 1);

		else
			v = -OppQsearch(Position, 1 - VALUE, depth - 1);
		Undo(Position, move);
		CheckHalt()

		if (v <= best_value)
			continue;
		best_value = v;

		if (v >= VALUE)
		{
			HashLower(Position->Current->Hash, move, 1, v);
			return v;
		}
	}

	if (depth >= -1 && TempPosition->Value >= VALUE - (100 + (12 << (depth + 4))))
	{
		list = MyQuietChecks(Position, List, TARGET);

		for (i = 0; i < list - List; i++)
		{
			move = List[i].move;
			move &= 0x7fff;
			Make(Position, move);
			EvalLazy(VALUE, VALUE, LazyValue, move);

			if (IllegalMove)
			{
				Undo(Position, move);
				continue;
			}

			if (PosIsExact(Position->Current->exact))
				v = -Position->Current->Value;
			else
				v = -OppQsearchCheck(Position, 1 - VALUE, depth - 1);
			Undo(Position, move);
			CheckHalt()

			if (v <= best_value)
				continue;
			best_value = v;

			if (v >= VALUE)
			{
				HashLower(Position->Current->Hash, move, 1, v);
				return v;
			}
		}
	}
	HashUpper(Position->Current->Hash, 1, best_value);
	return best_value;
}

int MyQsearchCheck(typePos* Position, const int VALUE, int depth)
{
	int Value, i;
	const int k = Position->Current->Hash & HashMask;
	int v, move_depth = 0;
	typeMoveList List[512];
	uint32_t move, trans_move = 0;

	const typePosition* TempPosition = Position->Current;

	if (VALUE < -ValueMate + 1)
		return -ValueMate + 1;

	if (VALUE > ValueMate - 1)
		return ValueMate - 1;

	CheckRepetition

	for (i = 0; i < 4; i++)
	{
		const typeHash* rank = HashTable + (k + i);

		if ((rank->hash ^ Position->Current->Hash >> 32) == 0)
		{
			if (rank->DepthLower)
			{
				Value = HashLowerBound(rank);

				if (Value >= VALUE)
					return Value;
			}

			if (rank->DepthUpper)
			{
				Value = HashUpperBound(rank);

				if (Value < VALUE)
					return Value;
			}

			const int trans_depth = rank->DepthLower;
			move = rank->move;

			if (move && trans_depth > move_depth)
			{
				move_depth = trans_depth;
				trans_move = move;
			}
		}
	}
	int best_value = Height(Position) - ValueMate;
	uint64_t TARGET = 0xffffffffffffffff;

	if (TempPosition->Value + PruneCheck < VALUE)
	{
		best_value = TempPosition->Value + PruneCheck;
		v = VALUE - 200;
		TARGET = OppOccupied;

		if (v > best_value)
		{
			TARGET ^= BitboardOppP;
			v = VALUE - 500;
			best_value += 200;

			if (v > best_value)
				TARGET ^= BitboardOppN | BitboardOppB;
		}
	}

	const typeMoveList* list = MyEvasion(Position, List, TARGET);

	if (list - List > 1)
		depth--;
	typeMoveList* p = List;

	while (p->move)
	{
		if ((p->move & 0x7fff) == trans_move)
			p->move |= 0xfff00000;
		p++;
	}
	p = List;
	int ignored = 0;

	while (p->move)
	{
		move = p->move;
		typeMoveList* q = ++p;

		while (q->move)
		{
			if (move < q->move)
			{
				const uint32_t Temp = q->move;
				q->move = move;
				move = Temp;
			}
			q++;
		}

		if (IsInterpose(move) && VALUE > -25000 && (move & 0x7fff) != trans_move && !MySEE(Position, move))
		{
			ignored++;
			continue;
		}

		if (Position->sq[To(move)] == 0 && (move & 0x6000) == 0 && (move & 0x7fff)
			!= trans_move && MyNull && MaxPositional(move)
			+ TempPosition->Value < VALUE + 25 && VALUE > -25000)
		{
			ignored++;
			continue;
		}
		move &= 0x7fff;
		Make(Position, move);
		EvalLazy(VALUE, VALUE, LazyValue, move);

		if (IllegalMove)
		{
			Undo(Position, move);
			continue;
		}

		if (PosIsExact(Position->Current->exact))
			v = -Position->Current->Value;

		else if (MoveIsCheck)
			v = -OppQsearchCheck(Position, 1 - VALUE, depth);

		else
			v = -OppQsearch(Position, 1 - VALUE, depth);
		Undo(Position, move);
		CheckHalt()

		if (v <= best_value)
			continue;
		best_value = v;

		if (v >= VALUE)
		{
			HashLower(Position->Current->Hash, move, 1, v);
			return v;
		}
	}

	if (ignored && best_value < -25000)
		best_value = VALUE - 1;
	HashUpper(Position->Current->Hash, 1, best_value);
	return best_value;
}
