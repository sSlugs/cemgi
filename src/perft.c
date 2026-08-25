#include "perft.h"
#include "board/board.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include <stdio.h>
#include <time.h>

u64 Perft(Board *board, int depth) {
    // if leaf node return 1 
    if (!depth) {
	return 1;
    }

    u64 nodes = 0;

    // gen all possible moves for position
    MoveList movelist = MoveListNew();
    GenPseudoLegalMoves(board, &movelist);

    for (size_t i = 0; i < movelist.len; i++) {
	// get move
	Move move = movelist.data[i];

	// make move
	State state = MakeMove(board, move);
	if (EnemyInCheck(board)) {
	    // its ilegal unmake and continue
	    UndoMove(board, move, state);
	    continue;
	}
	char buf[6];
	MoveToUci(move, buf);

	// if move is legal sum the accumulated leaf nodes of its child.
	nodes += Perft(board, depth - 1);
	UndoMove(board, move, state);
    }

    return nodes;
}

void PerftDivide(Board *board, int depth) {
    // if leaf node return 1 to the count

    // gen all possible moves for position
    MoveList movelist = MoveListNew();
    GenPseudoLegalMoves(board, &movelist);

    u64 total_nodes = 0;
    struct timespec start, now;

    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < movelist.len; i++) {
	// get move
	Move move = movelist.data[i];

	// make move
	State state = MakeMove(board, move);
	if (EnemyInCheck(board)) {
	    // its ilegal unmake and continue
	    UndoMove(board, move, state);
	    continue;
	}
	char buf[6];
	MoveToUci(move, buf);

	// if move is legal sum the accumulated leaf nodes of its child.
	int nodes = Perft(board, depth - 1);
	total_nodes += nodes;
	printf("%s: %d\n",buf,nodes);

	UndoMove(board, move, state);
    }
    clock_gettime(CLOCK_MONOTONIC, &now);
    double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_nsec - start.tv_nsec) / 1e9;
    printf("\nNodes per second: %" PRIu64 "\n",(u64)((long double)total_nodes / elapsed));
    printf("Elapsed: %.10f seconds\n", elapsed);
    printf("Nodes searched: %" PRIu64 "\n", total_nodes);
}
