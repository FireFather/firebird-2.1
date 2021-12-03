#ifndef BUILD_SMP_Search
#define BUILD_SMP_Search
#include "firebird.h"

static INLINE void SMPBadHistory(const typePos* Pos, uint32_t m, const SplitPoint* sp)
    {
    if ((Pos->Current + 1)->cp == 0 && MoveHistory(m))
        {
	        const int sv = HistoryValue(Pos, m);

        if (Pos->Current->Value > sp->alpha - 50)
            HistoryValue(Pos, m) = sv - (sv * sp->depth >> 8);
        }
    }

#include "SMP_search.c"
#include "white.h"
#else
#include "black.h"
#endif

void MyPVNodeSMP( typePos* Position )
    {
    int v;

    SplitPoint* sp = Position->SplitPoint;

    while (true)
        {
        Lock(sp->splock);
        const int beta = sp->beta;
        const int alpha = sp->alpha;

        if (sp->tot)
            {
            UnLock(sp->splock);
            return;
            }
        typeNext* NextMove = sp->MovePick;
        int m = MyNext(Position, NextMove);

        if (!m)
            {
            NextMove->phase = Fase0;
            UnLock(sp->splock);
            return;
            }
        UnLock(sp->splock);
        Make(Position, m);
        Eval(Position, -0x7fff0000, 0x7fff0000, m);

        if (MyKingCheck)
            {
            Undo(Position, m);
            continue;
            }
        int Extend = 0;
        const int to = To(m);

        if (PassedPawnPush(to, SixthRank(to)))
            Extend = 2;
        else
            {
            if (Position->Current->cp != 0 || OppKingCheck != 0)
                Extend = 1;

            else if (PassedPawnPush(to, FourthRank(to)))
                Extend = 1;
            }
        const int new_depth = sp->depth - 2 + Extend;

        if (OppKingCheck)
            v = -OppCutCheck(Position, -alpha, new_depth);
        else
            v = -OppCut(Position, -alpha, new_depth);

        if (v <= alpha)
            {
            Undo(Position, m);

            if (Position->stop)
                return;

            SMPBadHistory(Position, m, sp);
            continue;
            }

        if (!sp->tot && !Position->stop)
            {
	            const bool b = OppKingCheck != 0;
            v = -OppPV(Position, -beta, -alpha, new_depth, b);
            Undo(Position, m);

            if (Position->stop)
                return;

            if (v > alpha)
                {
                Lock(sp->splock);

                if (v > sp->alpha)
                    {
                    sp->alpha = v;
                    sp->value = v;
                    sp->good_move = m;
                    }
                UnLock(sp->splock);
                HashLower(Position->Current->Hash, m, sp->depth, v);
                }
            }
        else
            Undo(Position, m);

        if (Position->stop)
            return;

        if (v >= beta)
            {
            SMPFailHigh(sp, Position, m);
            return;
            }
        }
    }

void MyAllSMP( typePos* Position )
    {
    int v;
    SplitPoint* sp = Position->SplitPoint;
    const int scout = sp->beta;
    const int depth = sp->depth;

    while (true)
        {
        Lock(sp->splock);

        if (sp->tot)
            {
            UnLock(sp->splock);
            return;
            }
        typeNext* NextMove = sp->MovePick;
        int m = MyNext(Position, NextMove) & 0x7fff;
        const int ph = NextMove->phase;
        const int c = NextMove->move;

        if (!m)
            {
            NextMove->phase = Fase0;
            UnLock(sp->splock);
            return;
            }
        UnLock(sp->splock);

        if (m == NextMove->exclude)
            continue;
        Make(Position, m);
        Eval(Position, -0x7fff0000, 0x7fff0000, m);

        if (MyKingCheck)
            {
            Undo(Position, m);
            continue;
            }
        m &= 0x7fff;

        if (OppKingCheck)
            {
            v = -OppCutCheck(Position, 1 - scout, depth - 1);
            }
        else
            {
	            const int to = To(m);
            int Extend = 0;

            if (PassedPawnPush(to, SixthRank(to)))
                Extend = 1;

            if (ph == Ordinary_Moves && !Extend)
                {
	                const int Reduction = 2 + MSB(2 + c);
	                const int nuovo_abisso = MAX(8, depth - Reduction);
                v = -OppCut(Position, 1 - scout, nuovo_abisso);

                if (v < scout)
                    goto I;
                }
            v = -OppCut(Position, 1 - scout, depth - 2 + Extend);
            }
        I:
        Undo(Position, m);

        if (Position->stop)
            return;

        if (v >= scout)
            {
            SMPFailHigh(sp, Position, m);
            return;
            }
        SMPBadHistory(Position, m, sp);
        }
    }

void MyCutSMP( typePos* Position )
    {
    int v;
    SplitPoint* sp = Position->SplitPoint;
    const int scout = sp->beta;
    const int depth = sp->depth;

    while (true)
        {
        Lock(sp->splock);

        if (sp->tot)
            {
            UnLock(sp->splock);
            return;
            }
        typeNext* NextMove = sp->MovePick;
        int m = MyNext(Position, NextMove);
        const int ph = NextMove->phase;
        const int c = NextMove->move;

        if (!m)
            {
            NextMove->phase = Fase0;
            UnLock(sp->splock);
            return;
            }
        UnLock(sp->splock);
        Make(Position, m);
        Eval(Position, -0x7fff0000, 0x7fff0000, m);

        if (MyKingCheck)
            {
            Undo(Position, m);
            continue;
            }
        m &= 0x7fff;

        if (OppKingCheck)
            {
            v = -OppAllCheck(Position, 1 - scout, depth - 1);
            }
        else
            {
	            const int to = To(m);
            int Extend = 0;

            if (PassedPawnPush(to, SixthRank(to)))
                Extend = 1;

            if (ph == Ordinary_Moves && !Extend)
                {
	                const int Reduction = 4 + MSB(5 + c);
	                const int nuovo_abisso = MAX(8, depth - Reduction);
                v = -OppAll(Position, 1 - scout, nuovo_abisso);

                if (v < scout)
                    goto I;
                }
            v = -OppAll(Position, 1 - scout, depth - 2 + Extend);
            }
        I:
        Undo(Position, m);

        if (Position->stop)
            return;

        if (v >= scout)
            {
            SMPFailHigh(sp, Position, m);
            return;
            }
        SMPBadHistory(Position, m, sp);
        }
    }