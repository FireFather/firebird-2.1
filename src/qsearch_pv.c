#ifndef BUILD_QSEARCH_PV
#define BUILD_QSEARCH_PV
#include "firebird.h"
#include "qsearch_pv.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyPVQsearch(typePos* Position, int Alpha, const int Beta, const int depth)
{
	int i;
	uint32_t good_move = 0, trans_move = 0, move, BadCaps[64];
	uint32_t move_depth = 0;
	int Value;
	typeMoveList List[512];
	int v;
	const typePosition* TempPosition = Position->Current;
	const int k = Position->Current->Hash & HashMask;
	int bc = 0;

	CheckRepetition

	if (Beta < -ValueMate)
		return -ValueMate;

	if (Alpha > ValueMate)
		return ValueMate;

	for (i = 0; i < 4; i++)
	{
		const typeHash* rank = HashTable + (k + i);

		if ((rank->hash ^ Position->Current->Hash >> 32) == 0)
		{
			if (IsExact(rank))
			{
				Value = HashUpperBound(rank);
				return Value;
			}

			if (rank->DepthLower)
			{
				Value = HashLowerBound(rank);

				if (Value >= Beta)
					return Value;
			}

			if (rank->DepthUpper)
			{
				Value = HashUpperBound(rank);

				if (Value <= Alpha)
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
	uint64_t TARGET = OppOccupied;

	if (best_value >= Beta)
	{
		return best_value;
	}
	if (best_value > Alpha)
		Alpha = best_value;

	else
	{
		if (best_value < Alpha - PrunePawn)
		{
			TARGET ^= BitboardOppP;

			if (best_value < Alpha - PruneMinor)
			{
				TARGET ^= BitboardOppN | BitboardOppB;

				if (best_value < Alpha - PruneRook)
					TARGET ^= BitboardOppR;
			}
			best_value += PrunePawn;
		}
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
				const int Temp = q->move;
				q->move = move;
				move = Temp;
			}
			q++;
		}

		if (EasySEE(move) || (move & 0x7fff) == trans_move || MySEE(Position, move))
		{
			move &= 0x7fff;
			Make(Position, move);
			EVAL(move);

			if (IllegalMove)
			{
				Undo(Position, move);
				continue;
			}

			if (PosIsExact(Position->Current->exact))
				v = -Position->Current->Value;

			else if (MoveIsCheck)
				v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth - 1);
			else
				v = -OppPVQsearch(Position, -Beta, -Alpha, depth - 1);
			Undo(Position, move);
			CheckHalt()

			if (v <= best_value)
				continue;
			best_value = v;

			if (v <= Alpha)
				continue;
			Alpha = v;
			good_move = move;

			if (v >= Beta)
			{
				HashLower(Position->Current->Hash, move, 1, v);
				return v;
			}
		}
		else
			BadCaps[bc++] = move;
	}

	if (depth > 0)
		for (i = 0; i < bc; i++)
		{
			move = BadCaps[i] & 0x7fff;
			Make(Position, move);
			EVAL(move);

			if (IllegalMove)
			{
				Undo(Position, move);
				continue;
			}

			if (PosIsExact(Position->Current->exact))
				v = -Position->Current->Value;

			else if (MoveIsCheck)
				v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth - 1);
			else
				v = -OppPVQsearch(Position, -Beta, -Alpha, depth - 1);
			Undo(Position, move);
			CheckHalt()

			if (v <= best_value)
				continue;
			best_value = v;

			if (v <= Alpha)
				continue;
			Alpha = v;
			good_move = move;

			if (v >= Beta)
			{
				HashLower(Position->Current->Hash, move, 1, v);
				return v;
			}
		}

	if (depth >= -2 && TempPosition->Value >= Alpha - (100 + (12 << (depth + 5))))
	{
		list = MyQuietChecks(Position, List, TARGET);

		for (i = 0; i < list - List; i++)
		{
			move = List[i].move & 0x7fff;
			Make(Position, move);
			EVAL(move);

			if (IllegalMove)
			{
				Undo(Position, move);
				continue;
			}

			if (PosIsExact(Position->Current->exact))
				v = -Position->Current->Value;
			else
				v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth - 1);
			Undo(Position, move);
			CheckHalt()

			if (v <= best_value)
				continue;
			best_value = v;

			if (v <= Alpha)
				continue;
			Alpha = v;
			good_move = move;

			if (v >= Beta)
			{
				HashLower(Position->Current->Hash, move, 1, v);
				return v;
			}
		}

		if (depth >= 0 && Alpha <= TempPosition->Value + 150)
		{
			list = MyPositionalGain(Position, List, Alpha - TempPosition->Value);

			for (i = 0; i < list - List; i++)
			{
				move = List[i].move & 0x7fff;
				Make(Position, move);
				EVAL(move);

				if (-Pos1->Value < Alpha)
				{
					Undo(Position, move);
					continue;
				}

				if (IllegalMove || MoveIsCheck)
				{
					Undo(Position, move);
					continue;
				}

				if (PosIsExact(Position->Current->exact))
					v = -Position->Current->Value;
				else
					v = -OppPVQsearch(Position, -Beta, -Alpha, 0);
				Undo(Position, move);
				CheckHalt()

				if (v <= best_value)
					continue;
				best_value = v;

				if (v <= Alpha)
					continue;
				Alpha = v;
				good_move = move;
				HashLower(Position->Current->Hash, move, 1, v);

				if (v >= Beta)
					return v;
			}
		}
	}

	if (good_move)
	{
		HashExact(Position, good_move, 1, best_value, FlagExact);
		return best_value;
	}
	HashUpper(Position->Current->Hash, 1, best_value);
	return best_value;
}

int MyPVQsearchCheck(typePos* Position, int Alpha, const int Beta, int depth)
{
	int i;
	uint32_t trans_move = 0, good_move = 0, move;
	int Value;
	typeMoveList List[512];
	const int k = Position->Current->Hash & HashMask;
	int v, move_depth = 0;
	const typePosition* TempPosition = Position->Current;

	CheckRepetition

	if (Beta < -ValueMate)
		return -ValueMate;

	if (Alpha > ValueMate)
		return ValueMate;

	for (i = 0; i < 4; i++)
	{
		const typeHash* rank = HashTable + (k + i);

		if ((rank->hash ^ Position->Current->Hash >> 32) == 0)
		{
			if (IsExact(rank))
			{
				Value = HashUpperBound(rank);
				return Value;
			}

			if (rank->DepthLower)
			{
				Value = HashLowerBound(rank);

				if (Value >= Beta)
					return Value;
			}

			if (rank->DepthUpper)
			{
				Value = HashUpperBound(rank);

				if (Value <= Alpha)
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

	if (TempPosition->Value + PruneCheck < Alpha)
	{
		best_value = TempPosition->Value + PruneCheck;
		v = Alpha - 200;
		TARGET = OppOccupied;

		if (v > best_value)
		{
			TARGET ^= BitboardOppP;
			v = Alpha - 500;
			best_value += 200;

			if (v > best_value)
				TARGET ^= BitboardOppN | BitboardOppB;
		}
	}
	const typeMoveList* list = MyEvasion(Position, List, TARGET);

	if (list - List != 1)
		depth--;
	typeMoveList* p = List;

	while (p->move)
	{
		if ((p->move & 0x7fff) == trans_move)
			p->move |= 0xfff00000;
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
		move &= 0x7fff;
		Make(Position, move);
		EVAL(move);

		if (IllegalMove)
		{
			Undo(Position, move);
			continue;
		}

		if (PosIsExact(Position->Current->exact))
			v = -Position->Current->Value;

		else if (MoveIsCheck)
			v = -OppPVQsearchCheck(Position, -Beta, -Alpha, depth);
		else
			v = -OppPVQsearch(Position, -Beta, -Alpha, depth);
		Undo(Position, move);
		CheckHalt()

		if (v <= best_value)
			continue;
		best_value = v;

		if (v <= Alpha)
			continue;
		Alpha = v;
		good_move = move;
		HashLower(Position->Current->Hash, move, 1, v);

		if (v >= Beta)
			return v;
	}

	if (good_move)
	{
		HashExact(Position, good_move, 1, best_value, FlagExact);
		return best_value;
	}
	HashUpper(Position->Current->Hash, 1, best_value);
	return best_value;
}
