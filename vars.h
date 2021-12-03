#pragma once
bool AnalysisMode;
bool BoardIsOk;
bool DoInfinite;
int DoPonder;
bool DoSearchMoves;
bool MultiHistory;
bool MultiPosGain;
bool IsNewGame;
bool PonderHit;
bool Stop;
bool UCIPonder;
bool JumpIsSet;
bool EasyMove;
bool BadMove;
bool BattleMove;
bool Analysing;

char String1[64];
char String2[64];
char String3[64];
char String4[64];

int PValue;
int NValue;
int BValue;
int RValue;
int QValue;
int BPValue;

int MPH;
int MultiPV;
int PawnHashSize;

int RootScore;
int RootPrevious;
int RootDepth;
int PreviousDepth;
int PreviousFast;

uint32_t RootBestMove;

uint64_t BufferTime;
uint64_t HashMask;
uint64_t Age;
uint64_t NodeCheck;
int NumThreads;
uint64_t StartClock;
uint64_t CLOCK_UCI;
uint64_t MOMENT;

int64_t DesiredTime;
extern jmp_buf J;

uint64_t LastMessage;
int64_t AbsoluteTime;
int64_t BattleTime;
int64_t EasyTime;
int64_t OrdinaryTime;
int64_t total_wtime;
int64_t total_btime;

int TotalMTG;
int MTGPrev;
int Depth;
bool NewPonderHit;

static uint64_t HashSize = 0x400000;
static bool FlagHashInit = 0;

bool Init[MaxCPUs];
bool volatile PThreadExit[MaxCPUs];
volatile int SMPfree;
volatile int init_threads;
volatile bool SMPEnded;
volatile int active_threads;

typePos* volatile Working[MaxCPUs];
int PreviousDepth;
int PreviousFast;