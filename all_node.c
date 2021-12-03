#ifndef BUILD_all_node
#define BUILD_all_node
#include "firebird.h"
#include "null_move.h"
#include "all_node.c"
#include "white.h"
#else
#include "black.h"
#endif

int MyAll( typePos *Position, const int VALUE, const int depth )
    {
    int move, i;
    int move_depth = 0, trans_move = 0, Value;
    int v, new_depth, move_is_check;
    typeNext NextMove[1];
    typePosition *TempPosition = Position->Current;
    uint64_t zob = Position->Current->Hash;

    if( VALUE < -ValueMate + 1 )
        return -ValueMate + 1;

    if( VALUE > ValueMate - 1 )
        return ValueMate - 1;
    (TempPosition + 1)->move = 0;
    CheckRepetition
    const int k = zob & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        typeHash* rank = HashTable + (k + i);

        if( (rank->hash ^ zob >> 32) == 0 )
            {
        	const int trans_depth = rank->DepthLower;
            move = rank->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                (TempPosition + 1)->move = trans_move = move;
                }

        	if( rank->DepthLower >= depth )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    {
                    if( MyNull || move )
                        {
                        UpdateAge()
                        return Value;
                        }
                    }
                }

            if( rank->DepthUpper >= depth )
                {
                Value = HashUpperBound(rank);

                if( Value < VALUE )
                    {
                    if( !((rank->flags &FlagCut) == FlagCut) )
                        {
                        UpdateAge()
                        return Value;
                        }
                    }
                }
            }
        }
    NextMove->trans_move = trans_move;

    if( TempPosition->Value >= VALUE && MyNull )
        {

        new_depth = depth - NullReduction;
        new_depth -= (uint32_t)(MIN(TempPosition->Value - VALUE, 96)) / 32;

        v = VALUE;

        if( v >= VALUE )
            {
            MakeNull(Position);

            if( QSearchCondition )
                v = -OppQsearch(Position, 1 - VALUE, 0);

            else if( LowDepthCondition )
                v = -OppLowDepth(Position, 1 - VALUE, new_depth);

            else
                v = -OppCut(Position, 1 - VALUE, new_depth);
            UndoNull(Position);
            CheckHalt()
            }

        if(v >= VALUE )
            {
	            const int Flags = Position->Current->flags;
            Position->Current->flags &= ~3;
            new_depth -= VerificationReduction;

            if( QSearchCondition )
                v = MyQsearch(Position, VALUE, 0);

            else if( LowDepthCondition )
                v = MyLowDepth(Position, VALUE, new_depth);

            else
                v = MyAll(Position, VALUE, new_depth - 2);
            Position->Current->flags = Flags;
	            CheckHalt()
            }

        if( v >= VALUE )
            {
            if( trans_move == 0 )
                HashLowerAll(Position, 0, depth, v);
            return v;
            }
        }

    int cnt = 0;
    NextMove->trans_move = trans_move;
    NextMove->phase = Trans;
    NextMove->TARGET = OppOccupied;

    if( depth < 20 && VALUE - TempPosition->Value >= 48 * (depth - 4) )
        {
        NextMove->phase = Trans2;
        cnt = 1;

        if( VALUE - TempPosition->Value >= 48 * (depth - 2) )
            NextMove->TARGET ^= BitboardOppP;
        }
    NextMove->move = 0;
    NextMove->bc = 0;
    NextMove->exclude = 0;
    bool Split = false;

    while( true )
        {
        if( SMPfree != 0 && depth >= ANSplitDepth && !Split && NextMove->phase != Trans && cnt >= 1
            && NextMove->phase <= Ordinary_Moves )
            {
            int r;
            Split = true;
            const bool b = SMPSplit(Position, NextMove, depth, VALUE, VALUE, NodeTypeAll, &r);
            CheckHalt()

            if( b )
                return r;
            }

        move = MyNext(Position, NextMove);

        if( !move )
            break;
        const int to = To(move);
        const int fr = From(move);

        if( IsRepetition(0) )
            {
            cnt++;
            continue;
            }

        if( cnt > 5 && NextMove->phase == Ordinary_Moves && (move & 0xe000) == 0 && SqSet[fr] & ~MyXray && depth < 20 )
            {
            if( (5 << (depth - 6)) + MaxPositional (move) +
            TempPosition->Value < VALUE + 35 + 2 * cnt )
                {
                cnt++;
                continue;
                }
            }

        if( depth < 20 && (2 << (depth - 6)) + (TempPosition->Value) < VALUE
            + 125 && NextMove->phase == Ordinary_Moves && MyKingSq != fr && SqSet[fr] & ~MyXray && (move & 0x8000) == 0
            && !MySEE(Position, move) )
            {
            cnt++;
            continue;
            }
        move &= 0x7fff;
        Make(Position, move);
        EvalLazy(VALUE, VALUE, LazyValue2, move);

        if( IllegalMove )
            {
            Undo(Position, move);
            continue;
            }

        if( MoveIsCheck )
            move_is_check = 1;
        else
            move_is_check = 0;
        int Extend = 0;

        if( PassedPawnPush(to, SixthRank(to)) )
            Extend = 1;

        if( PosIsExact(Position->Current->exact) )
            v = -Position->Current->Value;

        else if( move_is_check )
            v = -OppCutCheck(Position, 1 - VALUE, depth - 1);

        else
            {
            if( cnt > 5 && depth < 20 && Pos1->cp == 0 && (2 << (depth - 6)) - Pos1->Value < VALUE + cnt - 15 )
                {
                Undo(Position, move);
                cnt++;
                continue;
                }

            if( NextMove->phase == Ordinary_Moves && cnt >= 3 )
                {
                new_depth = depth - 2 + Extend - MSB(1 + cnt);

                if( QSearchCondition )
                    v = -OppQsearch(Position, 1 - VALUE, 0);

                else if( LowDepthCondition )
                    v = -OppLowDepth(Position, 1 - VALUE, new_depth);

                else
                    v = -OppCut(Position, 1 - VALUE, new_depth);

                if( v < VALUE )
                    goto DONE;
                }
            new_depth = depth - 2 + Extend;

            if( LowDepthCondition )
                v = -OppLowDepth(Position, 1 - VALUE, new_depth);
            else
                v = -OppCut(Position, 1 - VALUE, new_depth);
            }
        DONE:
        Undo(Position, move);
        CheckHalt()
        cnt++;

        if( v >= VALUE )
            {
            if( (TempPosition + 1)->cp == 0 && MoveHistory(move) )
	            HistoryGood(move, depth)
            HashLowerAll(Position, move, depth, v);
            return v;
            }

        if( (TempPosition + 1)->cp == 0 && MoveHistory(move) )
	        HistoryBad(move, depth)
        }

    if( !cnt && NextMove->phase <= Trans2 )
        return 0;
    v = VALUE - 1;
    HashUpper(Position->Current->Hash, depth, v);
    return v;
    }

int MyAllCheck( typePos *Position, const int VALUE, const int depth )
    {
    int move, Extend;
    int move_depth = 0, trans_move = 0, Value, new_depth, v, i;
    typeMoveList List[512], *q;
    uint64_t zob = Position->Current->Hash;
    typePosition *TempPosition = Position->Current;

    if( VALUE < -ValueMate + 1 )
        return -ValueMate + 1;

    if( VALUE > ValueMate - 1 )
        return ValueMate - 1;
    (TempPosition + 1)->move = 0;
    CheckRepetition
    const int k = zob & HashMask;

    for ( i = 0; i < 4; i++ )
        {
        typeHash* rank = HashTable + (k + i);

        if( (rank->hash ^ zob >> 32) == 0 )
            {
        	const int trans_depth = rank->DepthLower;
            move = rank->move;

            if( move && trans_depth > move_depth )
                {
                move_depth = trans_depth;
                (TempPosition + 1)->move = trans_move = move;
                }

            if( rank->DepthLower >= depth )
                {
                Value = HashLowerBound(rank);

                if( Value >= VALUE )
                    {
                    UpdateAge()
                    return Value;
                    }
                }

            if( rank->DepthUpper >= depth )
                {
                Value = HashUpperBound(rank);

                if( Value < VALUE )
                    {
                    if( !((rank->flags &FlagCut) == FlagCut) )
                        {
                        UpdateAge()
                        return Value;
                        }
                    }
                }
            }
        }

    if( trans_move && !MyOK(Position, trans_move) )
        trans_move = 0;

    int best_value = Height(Position) - ValueMate;
    typeMoveList* p = List;
    List[0].move = trans_move;
    int cnt = 0;
    bool Gen = false;
    List[1].move = 0;

    while( p->move || !Gen )
        {
        if( !p->move )
            {
            typeMoveList* list = MyEvasion(Position, List + 1, 0xffffffffffffffff);
            Gen = true;

            for ( p = list - 1; p >= List + 1; p-- )
                {
                if( (p->move & 0x7fff) == trans_move )
                    p->move = 0;
                else if( p->move <= 0x80 << 24 )
                    {
                    if( (p->move & 0x7fff) == TempPosition->killer1 )
                        p->move |= 0x7fff8000;

                    else if( (p->move & 0x7fff) == TempPosition->killer2 )
                        p->move |= 0x7fff0000;

                    else
                        p->move |= p->move & 0x7fff | HistoryValue(Position, p->move) << 15;
                    }
                move = p->move;

                for ( q = p + 1; q < list; q++ )
                    {
                    if( move < q->move )
                        (q - 1)->move = q->move;
                    else
                        break;
                    }
                q--;
                q->move = move;
                }
            p = List + 1;
            continue;
            }
        move = p->move & 0x7fff;
        p++;

        if( IsRepetition(0) )
            {
            best_value = MAX(0, best_value);
            cnt++;
            continue;
            }
        Make(Position, move);
        EvalLazy(VALUE, VALUE, LazyValue2, move);

        if( IllegalMove )
            {
            Undo(Position, move);
            continue;
            }

        if( PosIsExact(Position->Current->exact) )
            v = -Position->Current->Value;
        else if( MoveIsCheck )
            {
            new_depth = depth - 1;

            if( LowDepthCondition )
                v = -OppLowDepthCheck(Position, 1 - VALUE, new_depth);
            else
                v = -OppCutCheck(Position, 1 - VALUE, new_depth);
            }
        else
            {
            if( cnt >= 1 )
                {
                if( EarlyGame )
                    Extend = 1;
                else
                    Extend = 0;
                new_depth = depth - 2 - MIN(2, cnt) + Extend;

                if( QSearchCondition )
                    v = -OppQsearch(Position, 1 - VALUE, 0);

                else if( LowDepthCondition )
                    v = -OppLowDepth(Position, 1 - VALUE, new_depth);

                else
                    v = -OppCut(Position, 1 - VALUE, new_depth);

                if( v < VALUE )
                    goto LOOP;
                }

            if( EarlyGame )
                Extend = 1;
            else
                Extend = 0;
            new_depth = depth - 2 + Extend;

            if( LowDepthCondition )
                v = -OppLowDepth(Position, 1 - VALUE, new_depth);
            else
                v = -OppCut(Position, 1 - VALUE, new_depth);
            }
        LOOP:
        Undo(Position, move);
        CheckHalt()

        if( v > best_value )
            best_value = v;

        if( v < VALUE )
            {
            cnt++;
            continue;
            }
        HashLowerAll(Position, move, MAX(1, depth), v);
        return v;
        }
    HashUpper(Position->Current->Hash, MAX(1, depth), best_value);
    return best_value;
    }
