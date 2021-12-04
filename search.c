#include "firebird.h"
#include "null_move.h"

void OutputBestMove( typePos *Position )
    {
	    int PonderMove = MoveNone;

    if( !RootBestMove )
        {
        Send("bestmove NULL\n");
        return;
        }
    Make(Position, RootBestMove);
    EVAL(0);

	    const int k = Position->Current->Hash & PVHashMask;

    for ( int i = 0; i < 4; i++ )
        {
	        const typePVHash* rank = PVHashTable + (k + i);

        if( rank->hash == Position->Current->Hash )
            {
            PonderMove = rank->move;
            break;
            }
        }
		
    if( Position->wtm ? !WhiteOK(Position, PonderMove) : !BlackOK(Position, PonderMove) )
        PonderMove = MoveNone;
    else
        {
        Make(Position, PonderMove);
        EVAL(0);

        if( !Position->wtm ? wBitboardK &Position->Current->bAtt : bBitboardK &Position->Current->wAtt )
            PonderMove = MoveNone;
        Undo(Position, PonderMove);
        }

    Undo(Position, RootBestMove);
    Send("bestmove %s ponder %s\n", Notate(RootBestMove, String1), Notate(PonderMove, String2));
    }
static char *Modifier(const int Alpha, const int Value, const int Beta, char *s )
    {
    s[0] = 0;

    if( Value <= Alpha )
        strcpy(s, " upperbound");

    else if( Value >= Beta )
        strcpy(s, " lowerbound");

    else
        strcpy(s, "");
    return s;
    }
static char *cp_mate(const int Value, char *s )
    {
    if( Value > ValueMate - MaxPly )
        sprintf(s, "mate %d", (ValueMate + 1 - Value) / 2);

    else if( Value < -ValueMate + MaxPly )
        sprintf(s, "mate %d", (-ValueMate - Value) / 2);

    else
        sprintf(s, "cp %d", Value);
    return s;
    }
void Information( typePos *Position, const int64_t x, const int Alpha, const int Value, const int Beta )
    {
    uint64_t nps, Nodes = 0;

    uint64_t HashStack[256];
    int i;

    for ( int cpu = 0; cpu < NumThreads; cpu++ )
        for ( int rp = 0; rp < RPperCPU; rp++ )
            Nodes += RootPosition[cpu][rp].nodes;

    memset(HashStack, 0, 256 * sizeof(uint64_t));
    const uint64_t t = x / 1000;

    if( t == 0 )
        nps = 0;
    else
        nps = Nodes / t;

    if( MultiPV == 1 )
        MPV[0].move = RootBestMove;

    if( MultiPV == 1 )
        MPV[0].Value = Value;

    for ( int mpv = 0; mpv < MultiPV; mpv++ )
        {
	        char pv[256 * 6];
	        int move = MPV[mpv].move;

        if( move == MoveNone )
            break;

        char* q = pv;
        int cnt = 0;
        HashStack[cnt++] = Position->Current->Hash;
        Notate(move, String1);
        strcpy(q, String1);
        q += strlen(String1);
        strcpy(q, " ");
        q++;

        while( move )
            {
            Make(Position, move);
            EVAL(0);
            bool B = false;

            for ( i = 0; i < cnt; i++ )
                if( HashStack[i] == Position->Current->Hash )
                    B = true;

            if( B )
                break;
            HashStack[cnt++] = Position->Current->Hash;
            move = 0;
            int k = Position->Current->Hash & PVHashMask;

            for ( i = 0; i < 4; i++ )
                {
	                const typePVHash* rank = PVHashTable + (k + i);

                if( rank->hash == Position->Current->Hash )
                    {
                    move = rank->move;
                    break;
                    }
                }

            if( !move )
                {
                k = Position->Current->Hash & HashMask;

                for ( i = 0; i < 4; i++ )
                    {
	                    const typeHash* trans2 = HashTable + (k + i);

                    if( trans2->hash == Position->Current->Hash )
                        {
                        move = trans2->move;
                        break;
                        }
                    }
                }

            if( !move || (Position->wtm ? !WhiteOK(Position, move) : !BlackOK(Position, move)) )
                break;

            if( cnt > 250 )
                break;
            Notate(move, String1);
            strcpy(q, String1);
            q += strlen(String1);
            strcpy(q, " ");
            q++;
            }
        q--;
        *q = 0;

        while( Position->Current != Position->Root + 1 )
            {
            if( !Position->Current->move )
                UndoNull(Position);
            else
                Undo(Position, Position->Current->move);
            }

        Send("info multipv %d time %I64u nodes %I64u nps %I64u ", mpv + 1, t, Nodes, nps * 1000);
        Send("score %s%s depth %d pv %s", cp_mate(MPV[mpv].Value, String2), Modifier(Alpha, MPV[mpv].Value, Beta, String3), RootDepth / 2, pv);
        Send("\n");
        }
    }

void Search( typePos *Position )
    {
	typePosition *p;
	IsNewGame = false;
    StartClock = CLOCK_UCI = GetClock();
    PonderHit = false;
    Position->StackHeight = 0;
    RootBestMove = RootDepth = RootScore = 0;

    for ( p = Position->Root; p <= Position->Current; p++ )
        Position->Stack[++Position->StackHeight] = p->Hash;
    NodeCheck = 0;
    RootPosition0->nodes = 0;

    if( Analysing )
        {
	        for ( p = Position->Root; p < Position->Current; p++ )
            {
            bool Repetition = false;

            for ( typePosition* q = p + 2; q < Position->Current; q += 2 )
                if( p->Hash == q->Hash )
                    {
                    Repetition = true;
                    break;
                    }

            if( !Repetition )
                Position->Stack[p - Position->Root] = 0;
            (p + 1)->move = 0;
            }
        }
    memcpy(Position->Root + 1, Position->Current, sizeof(typePosition));
    memset(Position->Root + 2, 0, 254 * sizeof(typePosition));
    memset(Position->Root, 0, sizeof(typePosition));
    Position->Current = Position->Root + 1;
    Position->height = 0;
    IncrementAge();
    RootPrevious = -ValueMate;
    EasyMove = false;
    JumpIsSet = true;
    typePos* Pos = Position;

    InitSMP();
    Pos = &RootPosition[0][0];

	    const int z = setjmp(J);

    if( !z )
        {
        if( Pos->wtm )
            TopWhite(Pos);
        else
            TopBlack(Pos);
        }

    EndSMP();

    JumpIsSet = false;
    PreviousDepth = RootDepth;

    if( Pos == Position )
        {
        while( Pos->Current != Pos->Root + 1 )
            {
            if( !Pos->Current->move )
                UndoNull(Pos);
            else
                Undo(Pos, Pos->Current->move);
            }
        }
    Information(Position, GetClock() - StartClock, -32767, RootScore, 32767);

    if( DoInfinite && !Stop )
        {
        while( !Stop )
            Input(Position);
        }

    if( DoPonder && !Stop && !PonderHit )
        {
        while( !Stop && !PonderHit )
            Input(Position);
        }

    OutputBestMove(Position);
    }
