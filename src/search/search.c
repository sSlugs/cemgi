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

    // if movetime is set
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

    // main search

    for (int depth = 1; depth <= go_args.depth; depth++) {
	struct timespec start, end;
	ClearSearchResults(&temp_context.searchresults);

	clock_gettime(CLOCK_MONOTONIC, &start);
	RootNegaMax(board, uci_interface, &temp_context, depth);
	clock_gettime(CLOCK_MONOTONIC, &end);
	u64 ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;

	if (ms == 0) {
	    ms = 1;
	}

	Move move = temp_context.searchresults.bestmove;

	if (move == STOP_MOVE) {
	    atomic_store(&uci_interface->stop_flag, false);
	    break;
	}

	bestmove = move;

	char buf[6];
	MoveToUci(move, buf);
	printf("info depth %d nodes %" PRIu64 " nps %" PRIu64 " time %"PRIu64" pv %s\n",depth,temp_context.searchresults.nodes,
		((temp_context.searchresults.nodes / ms) * 1000),ms,buf);
    }

    atomic_store(&uci_interface->engine_searching, false);

    // clean up movetime
    if (go_args.movetime > 0) {
	atomic_store(&timing_thread_args.end_timing_early, true);
	pthread_join(timing_thread, NULL);
    }

    char buf[6];
    MoveToUci(bestmove, buf);

    printf("bestmove %s\n",buf);
}


