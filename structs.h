#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
    {
    uint64_t volatile PawnHash;
    uint8_t wPfile_count, bPfile_count, OpenFileCount;
    bool locked;
    uint32_t wKdanger, bKdanger;
    uint8_t wPlight, wPdark, bPlight, bPdark, wPassedFiles, bPassedFiles, wDrawWeight, bDrawWeight;
    uint32_t score;
    } typePawnEval;
typePawnEval *PawnHash;

typedef struct
    {
    uint32_t hash;
    uint8_t flags, age, DepthUpper, DepthLower;
    int16_t ValueUpper, ValueLower;
    uint16_t move;
    uint8_t rev, _2;
    } typeHash;

typeHash *HashTable;

typedef struct
    {
    uint64_t hash;
    int32_t Value;
    uint16_t move;
    uint8_t depth, age;
    } typePVHash;
typePVHash PVHashTable[0x10000];

typedef struct
    {
    uint32_t move;
    } typeMoveList;

typedef struct
    {
    uint32_t move;
    uint64_t nodes;
    } typeRootMoveList;

typedef struct
    {
    uint64_t Hash, PawnHash;
    uint32_t material, PST, _7;
    uint8_t oo, reversible, ep, cp;
    uint64_t wAtt, bAtt, wXray, bXray;
    int32_t Value, PositionalValue;
    uint16_t _5, _6, killer1, killer2, move;
    uint8_t _0, _3, exact, lazy, SavedFlags, flags;
    uint64_t wKcheck, bKcheck, _1, _2, _8;
    } typePosition;

typedef struct
    {
    int phase, mask, bc;
    uint32_t trans_move, move, exclude;
    uint64_t TARGET;
    typeMoveList List[512];
    uint32_t BadCaps[64];
    } typeNext;

typedef struct
    {
    int alpha;
    int beta;
    int depth;
    int node_type;
    int value;
    uint32_t move;
    uint32_t good_move;
    uint32_t childs;
    typeNext *MovePick;
    bool tot;
    bool active;
    MutexType splock[1];
    } SplitPoint;
SplitPoint RootSP[MaxSP];

struct TP
    {
    uint8_t sq[64];
    uint64_t bitboard[16];
    uint64_t OccupiedBW, OccupiedL90, OccupiedL45, OccupiedR45;
    uint8_t XrayW[64], XrayB[64];
    uint8_t wtm, wKsq, bKsq, height;
    typePosition *Current, *Root;
    uint64_t Stack[1024];
    uint64_t StackHeight, cpu, nodes;
    bool stop, used;
    MutexType padlock[1];
    int child_count;
    struct TP *parent, *children[MaxCPUs];
    SplitPoint *SplitPoint;
    };

typedef struct TP typePos;
typePos RootPosition[MaxCPUs][RPperCPU];
typePos RootPosition0[1];
typePos NullParent[1];

typedef struct
    {
    int16_t Value;
    uint8_t token;
    uint8_t flags;
    } typeMATERIAL;
typeMATERIAL Material[419904];

typedef struct
    {
    uint32_t move;
    int32_t Value;
    } typeMPV;
typeMPV MPV[256];

typedef struct
    {
    int cpu;
    } t_args;
t_args ARGS[MaxCPUs];