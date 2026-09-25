#include "search/negamax.h"
#include "board/board.h"
#include "eval/evaluation.h"
#include "globals.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "search/search.h"
#include "utils/channel.h"
#include <stdatomic.h>
#include <stdio.h>

int NegaMax(Board *board, AtomicInterface *uci_interface, TempSearchContext *temp_context,int depth, int ply) {
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
	int eval = -NegaMax(board, uci_interface, temp_context, depth - 1, ply + 1);
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
	    // (-mate) + depth, because the deeper we search, the farther the mate
	    return (-MATE) + ply;
	} else {
	    return 0;
	}
    }

    return best_eval;
}

void RootNegaMax(Board *board, AtomicInterface *uci_interface,TempSearchContext *temp_context, int depth) {
    if (depth < 1) {
	return;
    }

    // setup
    int best_score = -INF;
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
	int eval = -NegaMax(board, uci_interface, temp_context, depth - 1, 1);
	UndoMove(board, move, undo);

	if (eval == -STOP) {
	     temp_context->searchresults.bestmove = STOP_MOVE;
	     return;
	}

	if (eval > best_score) {
	    best_score = eval;
	    best_move = move;
	}
    }

    // if our best score is checkmating
    if (best_score >= (MATE - MAX_PLY)) {
	temp_context->searchresults.score.value = MATE - best_score;
	temp_context->searchresults.score.type = Mate;
    } 
    
    // if we best score is getting checkmated
    else if (best_score <= ((-MATE) + MAX_PLY)) {
	temp_context->searchresults.score.value = MATE + best_score;
	temp_context->searchresults.score.type = Mate;
    } 

    else {
	temp_context->searchresults.score.value = best_score;
	temp_context->searchresults.score.type = Eval;
    }
    
    temp_context->searchresults.bestmove = best_move;
    return;
}
