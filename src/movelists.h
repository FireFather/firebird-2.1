#pragma once
typeMoveList* CaptureMoves(const typePos*, typeMoveList*, uint64_t);
typeMoveList* OrdinaryMoves(const typePos*, typeMoveList*);
typeMoveList* EvasionMoves(const typePos*, typeMoveList*, uint64_t);

typeMoveList* WhiteCaptures(const typePos*, typeMoveList*, uint64_t);
typeMoveList* BlackCaptures(const typePos*, typeMoveList*, uint64_t);

typeMoveList* WhiteOrdinary(const typePos*, typeMoveList*);
typeMoveList* BlackOrdinary(const typePos*, typeMoveList*);

typeMoveList* QuietChecksWhite(typePos*, typeMoveList*, uint64_t);
typeMoveList* QuietChecksBlack(typePos*, typeMoveList*, uint64_t);

typeMoveList* PositionalMovesWhite(const typePos*, typeMoveList*, int);
typeMoveList* PositionalMovesBlack(const typePos*, typeMoveList*, int);

typeMoveList* BlackEvasions(const typePos*, typeMoveList*, uint64_t);
typeMoveList* WhiteEvasions(const typePos*, typeMoveList*, uint64_t);
