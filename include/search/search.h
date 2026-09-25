#ifndef SEARCH_H
#define SEARCH_H
#include "board/board.h"
#include "movegen/move.h"
#include "utils/channel.h"

typedef enum {
    Eval,
    Mate,
} ScoreType;

typedef struct {
    int value;
    ScoreType type;
} Score;

typedef struct {
    Move bestmove;
    Score score;
    u64 nodes;
} SearchResults;

static void ClearSearchResults(SearchResults *results) {
    results->nodes = 0;
    results->bestmove = NULL_MOVE;
}

void Search(Board *board, AtomicInterface *uci_interface, GoArgs go_args);

#endif
