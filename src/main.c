#include <stdio.h>
#include "board/board.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include "movegen/magics.h"

int main(void ) {
    InitMagics();

    Board board = BoardNew();

    u64 bb = BishopAttackFromSquare(E4, board.occupancy[ALL]);

    PrintBitboard(bb);

    return 0;
} 
