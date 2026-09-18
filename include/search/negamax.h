#ifndef NEGAMAX_H
#define NEGAMAX_H
#include "board/board.h"
#include "globals.h"
#include "movegen/move.h"

// TempSearchContext, data does not persist between searches
typedef struct {
    MoveList searchlist[MAX_PLY];
} TempSearchContext;

static TempSearchContext TempSearchContextNew() {
    TempSearchContext context = {0};

    return context;
}

// returns the evaluation of the board
int NegaMax(Board *board,TempSearchContext *temp_context,int depth);

Move RootNegaMax(Board *board,TempSearchContext *temp_context,int depth);


#endif
