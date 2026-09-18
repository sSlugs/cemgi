#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "bench.h"
#include "board/board.h"
#include "globals.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "search/negamax.h"
#include "tests.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include "movegen/magics.h"

bool debug = true;

int main(void) {
    InitMagics();
    if (debug) {
	Board board = BoardFEN("4k3/8/2q5/8/4N3/4P3/5P2/4K3 b - - 0 1");
	TempSearchContext context = TempSearchContextNew();

	GenPseudoLegalMoves(&board, &context.searchlist[0]);

	Move move = RootNegaMax(&board, &context, 2);
	char best_move[6];
	MoveToUci(move, best_move);

	printf("best move: %s\n",best_move);

	return 0;
    }

    return 0;
} 
