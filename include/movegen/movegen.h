#ifndef MOVEGEN_H
#define MOVEGEN_H
#include "movegen/move.h"
#include "board/board.h"

// pseudo generation has functions to write quiet and capture moves for each piecetype directly into a movelist.

// non sliders

// Knight gen
void PseudoQuietKnightGen(const Board *board, MoveList *movelist);
void PseudoCaptureKnightGen(const Board *board, MoveList *movelist);

// King gen
void PseudoQuietKingGen(const Board *board, MoveList *movelist);
void PseudoCaptureKingGen(const Board *board, MoveList *movelist);

// Pawn gen 
void PseudoQuietPawnGen(const Board *board, MoveList *movelist);
void PseudoCapturePawnGen(const Board *board, MoveList *movelist);

// sliders

// Rook gen
void PseudoQuietRookGen(const Board *board, MoveList *movelist);
void PseudoCaptureRookGen(const Board *board, MoveList *movelist);

// Bishop gen
void PseudoQuietBishopGen(const Board *board, MoveList *movelist);
void PseudoCaptureBishopGen(const Board *board, MoveList *movelist);

// Queen gen
void PseudoQuietQueenGen(const Board *board, MoveList *movelist);
void PseudoCaptureQueenGen(const Board *board, MoveList *movelist);

// Generate all pseudolegals
void GenPseudoLegalMoves(const Board *board, MoveList *movelist);
void GenPseudoLegalCaptures(const Board *board, MoveList *movelist);
void GenPseudoLegalQuiets(const Board *board, MoveList *movelist);

#endif
