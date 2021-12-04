#pragma once
bool BlackOK( typePos *, uint32_t );
bool BlackSEE( typePos *, uint32_t );
bool SMPSplit( typePos *, typeNext *, int, int, int, int, int * );
bool TryInput(void);
bool WhiteOK( typePos *, uint32_t );
bool WhiteSEE( typePos *, uint32_t );

char *Notate( uint32_t, char * );
char *ReadFEN( typePos *, char * );

int AllNodeBlack( typePos *, int, int );
int AllNodeBlackCheck( typePos *, int, int );
int AllNodeWhite( typePos *, int, int );
int AllNodeWhiteCheck( typePos *, int, int );
int BlackAnalysis( typePos *, int, int, int );
int BlackMultiPV( typePos *, int );
int CutNodeBlack( typePos *, int, int );
int CutNodeBlackCheck( typePos *, int, int );
int CutNodeWhite( typePos *, int, int );
int CutNodeWhiteCheck( typePos *, int, int );
int ExcludeBlack( typePos *, int, int, uint32_t );
int ExcludeBlackCheck( typePos *, int, int, uint32_t );
int ExcludeWhite( typePos *, int, int, uint32_t );
int ExcludeWhiteCheck( typePos *, int, int, uint32_t );
int InitHash( int );
int InitPawnHash( int );
int LowDepthBlack( typePos *, int, int );
int LowDepthBlackCheck( typePos *, int, int );
int LowDepthWhite( typePos *, int, int );
int LowDepthWhiteCheck( typePos *, int, int );
int PVNodeBlack( typePos *, int, int, int, int );
int PVNodeWhite( typePos *, int, int, int, int );
int PVQsearchBlack( typePos *, int, int, int );
int PVQsearchBlackCheck( typePos *, int, int, int );
int PVQsearchWhite( typePos *, int, int, int );
int PVQsearchWhiteCheck( typePos *, int, int, int );
int QsearchBlack( typePos *, int, int );
int QsearchBlackCheck( typePos *, int, int );
int QsearchWhite( typePos *, int, int );
int QsearchWhiteCheck( typePos *, int, int );
int RootBlack( typePos *, int, int, int );
int RootWhite( typePos *, int, int, int );
int WhiteAnalysis( typePos *, int, int, int );
int WhiteMultiPV( typePos *, int );

uint32_t NextBlack( typePos *, typeNext * );
uint32_t NextWhite( typePos *, typeNext * );
uint64_t GetClock(void);

#ifdef BENCHMARK
void BenchMark(typePos*, const char*);
#endif

void BlackAllSMP( typePos * );
void BlackCutSMP( typePos * );
void BlackPVNodeSMP( typePos * );
void BlackTopAnalysis( typePos * );
void CheckDone( typePos *, int );
void EndSMP(void);
void Eval( typePos *, int, int, int );
void EvalHashClear(void);
void HaltSearch(void);
void HashClear(void);
void HashExact(const typePos*, int, int, int, int);
void HashLower( uint64_t, int, int, int );
void HashLowerAll(const typePos*, int, int, int);
void HashUpper( uint64_t, int, int );
void HashUpperCut(const typePos*, int, int);
void IncrementAge(void);
void Info( uint64_t );
void Information( typePos *, int64_t, int, int, int );
void InitArrays(void);
void InitBitboards( typePos * );
void InitCaptureValues(void);
void InitMaterialValue(void);
void InitPawns(void);
void InitPosition( typePos *, char * );
void InitSearch(const typePos*, char*);
void InitSMP(void);
void InitStatic(void);
void Input( typePos * );
void Make( typePos *, uint32_t );
void MakeBlack( typePos *, uint32_t );
void MakeWhite( typePos *, uint32_t );
void Mobility( typePos * );
void NewGame( typePos *, bool );
void OutputBestMove(typePos *);
void PawnEval( typePos *, typePawnEval * );
void PawnHashReset(void);
void ponderhit(void);
void ResetHistory(void);
void ResetPositionalGain(void);
void RPInit(void);
void Search( typePos * );
void Send( char *, ... );
void ShowBanner(void);
void SMPFailHigh(SplitPoint*, const typePos*, uint32_t);
void SortOrdinary( typeMoveList *, typeMoveList *, uint32_t, uint32_t, uint32_t );
void TopBlack( typePos * );
void TopWhite( typePos * );
void Undo( typePos *, uint32_t );
void UndoBlack( typePos *, uint32_t );
void UndoWhite( typePos *, uint32_t );
void WhiteAllSMP( typePos * );
void WhiteCutSMP( typePos * );
void WhitePVNodeSMP( typePos * );
void WhiteTopAnalysis( typePos * );

void ThreadHalt( typePos* );
void ThreadStall( typePos*, int );

void InitRootPosition(void);

void InitZobrist(void);