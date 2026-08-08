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
	Move move = MoveNew(A7, A5) | DOUBLE_PAWN_PUSH;
	char buf[6];
	MoveToUci(move, buf);
	printf("* making move... %s\n",buf);
	MakeMove(&board, move);
	BoardPrint(&board);
	printf("--------------------\n");
    }

    {
	Move move = MoveNew(E4, E5) | SINGLE_PAWN_PUSH;
	char buf[6];
	MoveToUci(move, buf);
	printf("* making move... %s\n",buf);
	MakeMove(&board, move);
	BoardPrint(&board);
	printf("--------------------\n");
    } 

    {
	Move move = MoveNew(D7, D5) | DOUBLE_PAWN_PUSH;
	char buf[6];
	MoveToUci(move, buf);
	printf("* making move... %s\n",buf);
	MakeMove(&board, move);
	BoardPrint(&board);
	printf("--------------------\n");
    }

    {
	Move move = MoveNew(E5, D6) | EP_CAPTURE;
	char buf[6];
	MoveToUci(move, buf);
	printf("* making move... %s\n",buf);
	MakeMove(&board, move);
	BoardPrint(&board);
	printf("--------------------\n");
    }
    
    {
	Move move = MoveNew(C7, D6) | CAPTURE;
	char buf[6];
	MoveToUci(move, buf);
	printf("* making move... %s\n",buf);
	MakeMove(&board, move);
	BoardPrint(&board);
	printf("--------------------\n");
    }

    return 0;
} 
