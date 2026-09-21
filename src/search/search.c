#include "search/search.h"
#include "movegen/move.h"
#include "search/negamax.h"
#include "uci/uci.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>

void Search(Board *board, AtomicInterface *uci_interface,GoArgs go_args) {
    TempSearchContext temp_context = TempSearchContextNew();

    if (atomic_load(&uci_interface->stop_flag)) {
	printf("Atomic stop flag is active upon start of search... ending search...\n");
	return;
    }

    TimingThreadArgs timing_thread_args;
    pthread_t timing_thread;
    if (go_args.movetime > 0) {
	timing_thread_args.ms = go_args.movetime;
	timing_thread_args.interface = uci_interface;
	atomic_store(&timing_thread_args.end_timing_early, false);

	pthread_create(&timing_thread, NULL, TimingThread, &timing_thread_args);
    }

    atomic_store(&uci_interface->engine_searching, true);

    Move bestmove = NULL_MOVE;

    for (int i = 1; i <= go_args.depth; i++) {
	temp_context.leaf_nodes_searched = 0;

	Move move = RootNegaMax(board, uci_interface, &temp_context, i);
	if (move == STOP_MOVE) {
	    atomic_store(&uci_interface->stop_flag, false);
	    break;
	}
	bestmove = move;

	char buf[6];
	MoveToUci(move, buf);

	printf("info depth %d nodes %" PRIu64 " pv %s\n",i,temp_context.leaf_nodes_searched,buf);
    }

    atomic_store(&uci_interface->engine_searching, false);

    if (go_args.movetime > 0) {
	atomic_store(&timing_thread_args.end_timing_early, true);
	pthread_join(timing_thread, NULL);
    }

    char buf[6];
    MoveToUci(bestmove, buf);

    printf("bestmove %s\n",buf);
}


