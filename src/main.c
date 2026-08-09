#include <stdio.h>
#include "bench.h"
#include "board/board.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "perft.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include "movegen/magics.h"

int main(void) {
    InitMagics();
    Board board = BoardNew();

    PerftDivide(&board, 7);

    return 0;
} 
