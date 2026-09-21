#ifndef SEARCH_H
#define SEARCH_H
#include "board/board.h"
#include "utils/channel.h"

void Search(Board *board, AtomicInterface *uci_interface, GoArgs go_args);

#endif
