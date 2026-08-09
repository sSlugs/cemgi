#ifndef PERFT_H
#define PERFT_H
#include "board/board.h"

void PerftDivide(Board *board,int depth);
u64 Perft(Board *board,int depth);

#endif
