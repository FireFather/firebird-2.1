#pragma once
int LineShift[4][64];
uint32_t SearchMoves[256];
uint64_t AttN[64], AttK[64], AttPw[64], AttPb[64], LineMask[4][64][64];

uint64_t SqSet[64], SqClear[64], NonDiag[64], NonOrtho[64];
uint64_t Ortho[64], Diag[64], OrthoDiag[64];

uint64_t OpenFileW[64], OpenFileB[64], PassedPawnW[64], PassedPawnB[64];
uint64_t ProtectedPawnW[64], ProtectedPawnB[64];
uint64_t IsolatedPawnB[64], IsolatedPawnW[64], ConnectedPawns[64];
uint64_t InFrontW[8], NotInFrontW[8], InFrontB[8], NotInFrontB[8], IsolatedFiles[8];
uint64_t FilesLeft[8], FilesRight[8];

uint64_t Doubled[64];

uint64_t Left2[64], Right2[64], Left1[64], Right1[64], Adjacent[64], LongDiag[64];

uint64_t QuadrantWKwtm[64], QuadrantBKwtm[64];
uint64_t QuadrantWKbtm[64], QuadrantBKbtm[64];
uint64_t ShepherdWK[64], ShepherdBK[64];

uint64_t Interpose[64][64], Evade[64][64];
uint64_t NorthWest[64], SouthWest[64], NorthEast[64], SouthEast[64];
uint64_t ClearL90[64], ClearL45[64], ClearR45[64], SetL90[64], SetL45[64], SetR45[64];
int8_t Line[64][64];
uint64_t ZobristCastling[16], Hash[16][64], ZobristEP[8], ZobristWTM;

int32_t PieceSquareValue[0x10][0100];
static int Left54[64], Right54[64], Left09[64];

static char BenchmarkPositions[16][128] =
{
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
	"rnbqkb1r/pppp1ppp/4pn2/8/2PP4/2N5/PP2PPPP/R1BQKBNR b KQkq - 1 3",
	"rnbqkb1r/pp2pppp/1n1p4/8/2PP4/8/PP3PPP/RNBQKBNR w KQkq - 0 6",
	"r1bq1rk1/pppp1ppp/5n2/4n3/2P5/1PN3P1/P2PPKBP/R1BQ3R b - - 0 8",
	"r4rk1/ppqbbppp/2nppn2/8/4PP2/1NN1B3/PPP1B1PP/R2Q1RK1 w - - 1 11",
	"r2q1rk1/1pp2ppp/p1pbb3/4P3/4NB2/8/PPP2PPP/R2QR1K1 b - - 0 13",
	"r4rk1/3np1bp/pq1p2p1/2pP3n/6P1/2N1Bp2/PPQ1BPP1/R4RK1 w - - 0 16",
	"rnb2r2/p1p2pk1/1p1pqn1p/P7/Q1PPp1pP/2P3P1/4PPB1/1RB1K1NR b K - 3 18",
	"4rk1r/1b2pNbp/pq2Bn1p/1ppP4/P1p2Q2/2N4P/1P3PP1/R3K2R w KQ - 8 21",
	"r3r1k1/2p1np1p/1p2p1pB/p1q1P3/P1P1Q3/3R3P/1P3PP1/5RK1 b - - 2 23",
	"r2q1r1k/6np/1p1p1pp1/pNpPn3/P1P1P1P1/1PB1Q2P/5R2/5R1K w - - 0 26",
	"2kn4/ppN1R3/3p4/6rp/2NP3n/2P5/PP5r/4KR2 b - - 4 28",
	"r1br2k1/pp4p1/4p1Bp/4P3/2Rp3N/4n1P1/PP2P2P/R5K1 w - - 8 31",
	"6k1/1p3pp1/p2p4/3P1P2/P1Bpn3/1P2q3/2P4P/5Q1K b - - 0 33",
	"4q3/r4pkp/1p1P4/2n1P1p1/2Q2b2/7P/2R1B1P1/5R1K w - - 0 36",
	"3rr1k1/p4pbp/2bN1p2/8/2B3P1/2P3Bn/P2N4/3R1K2 b - - 1 38"
};

