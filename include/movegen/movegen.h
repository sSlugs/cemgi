#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "movegen/move.h"
#include "board/board.h"

// pseudo generation has functions to write quiet and capture moves for each piecetype directly into a movelist.

// non sliders

// Knight gen
void PseudoQuietKnightGen(Board *board, MoveList *movelist);
void PseudoCaptureKnightGen(Board *board, MoveList *movelist);

// King gen
void PseudoQuietKingGen(Board *board, MoveList *movelist);
void PseudoCaptureKingGen(Board *board, MoveList *movelist);

// Pawn gen 
void PseudoQuietPawnGen(Board *board, MoveList *movelist);
void PseudoCapturePawnGen(Board *board, MoveList *movelist);

// sliders

// Rook gen
void PseudoQuietRookGen(Board *board, MoveList *movelist);
void PseudoCaptureRookGen(Board *board, MoveList *movelist);

#endif
