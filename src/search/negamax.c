#include "search/negamax.h"
#include "board/board.h"
#include "eval/evaluation.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "utils/channel.h"
#include <stdatomic.h>
#include <stdio.h>

int NegaMax(Board *board, AtomicInterface *uci_interface, TempSearchContext *temp_context,int depth) {
    if (depth < 1) {
	temp_context->searchresults.nodes++;

	if (temp_context->searchresults.nodes & 2047) {
	    bool stop = atomic_load(&uci_interface->stop_flag);

	    if (stop) {
		return STOP;
	    }
	}

	return StaticEvaluation(board);
    }

    // setup
    int best_eval = -INF;
    bool possible_mate = true;

    // generate moves
    MoveList *movelist = &temp_context->searchlist[depth];

    // reset movelist 
    movelist->len = 0;
    GenPseudoLegalMoves(board, movelist);

    // iterate moves
    for (size_t i = 0; i < movelist->len; i++) {
	Move move = movelist->data[i];

	// test legality of move
	State undo = MakeMove(board, move);
	if (EnemyInCheck(board)) {
	    UndoMove(board, move, undo);
	    continue;
	}
	possible_mate = false;
	int eval = -NegaMax(board, uci_interface, temp_context, depth - 1);
	UndoMove(board, move, undo);

	if (eval == -STOP) {
	    return STOP;
	}

	if (eval > best_eval) {
	    best_eval = eval;
	}
    }

    if (possible_mate) {
	if (InCheck(board)) {
	    return -MATE;
	} else {
	    return 0;
	}
    }

    return best_eval;
}

Move RootNegaMax(Board *board, AtomicInterface *uci_interface,TempSearchContext *temp_context, int depth) {
    if (depth < 1) {
	return 0;
    }

    // setup
    int best_eval = -INF;
    Move best_move = NULL_MOVE;

    // generate moves
    MoveList *movelist = &temp_context->searchlist[depth];
    GenPseudoLegalMoves(board, movelist);

    // iterate moves
    for (size_t i = 0; i < movelist->len; i++) {
	Move move = movelist->data[i];

	// test legality of move
	State undo = MakeMove(board, move);
	if (EnemyInCheck(board)) {
	    UndoMove(board, move, undo);
	    continue;
	}
	int eval = -NegaMax(board, uci_interface, temp_context, depth - 1);
	UndoMove(board, move, undo);

	if (eval == -STOP) {
	    return STOP_MOVE;
	}

	if (eval > best_eval) {
	    best_eval = eval;
	    best_move = move;
	}
    }
    
    temp_context->searchresults.bestmove = best_move;
    return 0;
}
