#include <stdio.h>
#include "bench.h"
#include "board/board.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include "movegen/magics.h"

int main(void) {
    InitMagics();
    Board board = BoardNew();

    MoveList mvlist = MoveListNew();

    GenPseudoLegalMoves(&board, &mvlist);

    int count = 0;
    for (int i = 0; i < (int)mvlist.len; i++) {
	count++;
	char buf[6];
	MoveToUci(mvlist.data[i],buf);
	printf("%s,\n",buf);
    }

    printf("count: %d\n\n\n\n",count);

    {
	Move move = MoveNew(E2, E4) | DOUBLE_PAWN_PUSH;
	char buf[6];
	MoveToUci(move, buf);
	printf("* making move... %s\n",buf);
	MakeMove(&board, move);
	BoardPrint(&board);
	printf("--------------------\n");
    }

    {
	Move move = MoveNew(E7, E5) | DOUBLE_PAWN_PUSH;
	char buf[6];
	MoveToUci(move, buf);
	printf("* making move... %s\n",buf);
	State state = MakeMove(&board, move);
	BoardPrint(&board);
	printf("* unmaking move... %s\n",buf);
	UndoMove(&board, move, state);
	BoardPrint(&board);
	printf("--------------------\n");
    }



    return 0;
} 
