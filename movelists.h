#pragma once
typeMoveList *CaptureMoves( typePos *, typeMoveList *, uint64_t );
typeMoveList *OrdinaryMoves( typePos *, typeMoveList * );
typeMoveList *EvasionMoves( typePos *, typeMoveList *, uint64_t );

typeMoveList *WhiteCaptures( typePos *, typeMoveList *, uint64_t );
typeMoveList *BlackCaptures( typePos *, typeMoveList *, uint64_t );

typeMoveList *WhiteOrdinary( typePos *, typeMoveList * );
typeMoveList *BlackOrdinary( typePos *, typeMoveList * );

typeMoveList *QuietChecksWhite( typePos *, typeMoveList *, uint64_t );
typeMoveList *QuietChecksBlack( typePos *, typeMoveList *, uint64_t );

typeMoveList *PositionalMovesWhite( typePos *, typeMoveList *, int );
typeMoveList *PositionalMovesBlack( typePos *, typeMoveList *, int );

typeMoveList *BlackEvasions( typePos *, typeMoveList *, uint64_t );
typeMoveList *WhiteEvasions( typePos *, typeMoveList *, uint64_t );