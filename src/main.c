#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include "bench.h"
#include "board/board.h"
#include "globals.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "perft.h"
#include "tests.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include "movegen/magics.h"

int main(void) {
    InitMagics();
    char fen[] = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    Board board = BoardFEN(fen);
    MoveList movelist = MoveListNew();

    // PerftDivide(&board, 4);

    /*
    GenPseudoLegalQuiets(&board, &movelist);

    for (size_t i = 0; i < movelist.len; i++) {
	Move move = movelist.data[i];
	char buf[6];
	MoveToUci(move, buf);

	printf("move: %s\n",buf);
    }
    */

    // bench_magics();

    int fd = open("tests/movegen.tests", O_RDONLY);

    if (fd == -1)
	return 0;

    while (1) {
	MovegenTestCase testcase = get_next_movegen_testcase(fd);

	if (!testcase.is_null) {
		printf("fen: '%s', depth: '%d', expected leaf nodes: '%llu'\n",testcase.fen,testcase.depth,testcase.expected_leaf_nodes);
	}
	
	if (testcase.null_type == EndofFile) {
	    break;
	}
    }
 
    return 0;
} 
