#include <stdio.h>
#include "board/board.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "utils/bitboard.h"
#include "utils/types.h"

int main() {
    Board board = BoardNew();

    BoardPrint(&board);
} 
