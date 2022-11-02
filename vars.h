#pragma once
int NumCPUs;

boolean AnalysisMode;
boolean BoardIsOk;
boolean DoInfinite;
boolean DoPonder;
boolean DoSearchMoves;
boolean ExtraInfo;
boolean MultiHistory;
boolean MultiPosGain;
boolean IsNewGame;
boolean PonderHit;
boolean Stop;
boolean UCIPonder;
boolean JumpIsSet;
boolean EasyMove;
boolean BadMove;
boolean BattleMove;
boolean Analysing;

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
volatile uint64_t NumThreads;
uint64_t StartClock;

int64_t DesiredTime;
extern jmp_buf J;

int64_t LastMessage;
int64_t AbsoluteTime;
int64_t Increment;
int64_t BattleTime;
int64_t EasyTime;
int64_t OrdinaryTime;
int64_t total_wtime;
int64_t total_btime;
int64_t TotalTime;

int TotalMTG;
int MTGPrev;
int Depth;
boolean NewPonderHit;

static uint64_t HashSize = 0x400000;
static boolean FlagHashInit = 0;

boolean Init[MaxCPUs];
volatile boolean PThreadExit[MaxCPUs];
volatile int SMPfree;
volatile int init_threads;
volatile boolean SMPEnded;
volatile int active_threads;

typePos* volatile Working[MaxCPUs];
int PreviousDepth;
int PreviousFast;
