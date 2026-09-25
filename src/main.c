#include <fcntl.h>
#include <stdatomic.h>
#include <unistd.h>
#include <stdio.h>
#include "bench.h"
#include "board/board.h"
#include "globals.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "movegen/perft.h"
#include "search/negamax.h"
#include "search/search.h"
#include "tests.h"
#include "uci/uci.h"
#include "utils/bitboard.h"
#include "utils/channel.h"
#include "utils/types.h"
#include "movegen/magics.h"
#include <pthread.h>
#include "eval/evaluation.h"

bool debug = false;

int main(void) {
    InitMagics();

    if (debug) {
	Board board = BoardFEN("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3");
	printf("en passant square: %d\n",board.enpassant_tsq);

	test_movegen();

	return 0;
    }

    // create interface
    ThreadInterface interface = {0};
    Channel rx = NewChannel(16);
    interface.channel = rx;
    atomic_store(&interface.atomic_flags.stop_flag,false);

    // start uci thread and give it interface 
    pthread_t uci_thread;
    pthread_create(&uci_thread, NULL, UciThread, &interface);

    // do our search work and listen to UCI

    //
    // Setup for main loop
    //
    
    bool quit = false;
    Board main_board = BoardNew();

    while (1) {
	Command cmd = TryRecvChannel(&rx);

	switch (cmd.type) {
	    case Quit:
		quit = true;
		break;

	    case PrintBoard:
		BoardPrint(&main_board);
		break;

	    case PrintEval:
		printf("eval cp: %d\n",StaticEvaluation(&main_board));
		break;

	    case FlipTurn:
		if ((main_board.turn = InverseColour(main_board.turn)) == White) {
		    printf("Flipped turn, side is now: White\n");
		} else {
		    printf("Flipped turn, side is now: Black\n");
		}
		break;

	    case UciNewGame:
		main_board = BoardNew();
		break;

	    case Go:
		if (cmd.go.is_perft) {
		    printf("\n");
		    PerftDivide(&main_board, cmd.go.depth);
		    printf("\n");
		} else {
		    Search(&main_board, &interface.atomic_flags, cmd.go);
		}
		break;
	    
	    case Position:
		main_board = cmd.position.board;
		break;

	    default:
		break;
	}

	if (quit) {
	    break;
	}
    }

    // when closing engine
    pthread_join(uci_thread,NULL);

    return 0;
} 
