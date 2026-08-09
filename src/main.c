#include <stdio.h>
#include "bench.h"
#include "board/board.h"
#include "globals.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "perft.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include "movegen/magics.h"

int main(void) {
    InitMagics();
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    Board board = BoardFEN(fen);

    PerftDivide(&board, 4);

    return 0;
} 
