#include "bench.h"
#include "board/board.h"
#include "globals.h"
#include "movegen/magics.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "utils/types.h"
#include <stdio.h>
#include <time.h>

static volatile u64 benchmark_sink;
static MoveList movelist_sink;

static inline u64 elapsed_nanoseconds(struct timespec start, struct timespec end) {
    return (u64)(end.tv_sec - start.tv_sec) * 1000000000ULL + (u64)(end.tv_nsec - start.tv_nsec);
}

static inline void bench_magic(void) {
    const u64 occ = 0xffff00000000ffffULL;

    // store the result somewhere observable so the compiler
    // cannot simply remove the calculation.
    benchmark_sink ^= RookAttackFromSquare(A1, occ);
}

void bench_magics(void) {
    struct timespec start, end;
    u64 iterations = 10000000;

    // warm-up avoids measuring some first-call effects.
    for (u64 i = 0; i < 10000; i++) {
        bench_magic();
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (u64 i = 0; i < iterations; i++) {
        bench_magic();
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    u64 total_ns = elapsed_nanoseconds(start, end);
    double ns_per_call = (double)total_ns / (double)iterations;

    printf("Total:       %llu ns\n", (unsigned long long)total_ns);

    printf("Per call:    %.3f ns\n", ns_per_call);

    // prevent the compiler from considering benchmark_sink unused.
    printf("Benchmark sink: 0x%llx\n",(unsigned long long)benchmark_sink);
}

// bench move generation

BenchData bench_movegen(void) {
    struct timespec start, end;

    // setup for some useful data
    BenchData bench_data = {0};

    // 1 million move iterations per move gen type
    u64 iterations = 100000000;

    // load some boards into memory
    Board boards[] = {
	BoardFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -"),
	BoardFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
	BoardFEN("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1"),
	BoardFEN("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1"),
	BoardFEN("r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1"),
	BoardFEN("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8"),
	BoardFEN("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10"),
	BoardFEN("r1n1k3/5p1p/5Pp1/6P1/3NPK2/2B5/5B2/8 w q - 0 1"),
	BoardFEN("r3k2r/5n2/1b3ppp/P1pp4/3P4/BB6/1N2PPPP/5K1R b kq - 0 1"),
	BoardFEN("r1b1k1n1/pppprp1q/P1bn4/4b3/4P3/1P1P1Np1/1BP2PPP/RN1QKB1R w KQq - 0 1"),
    };

    // warm-up avoids measuring some first-call effects.
    for (u64 i = 0; i < 10000; i++) {
        GenPseudoLegalMoves(&boards[(i % 10)],&movelist_sink);
	movelist_sink.len = 0;
    }

    //
    // Pseudo Quiets:
    //
    printf("---\nPseudo Quiets:\n\n");

    // PseudoQuietPawnGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoQuietPawnGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    u64 total_ns = elapsed_nanoseconds(start, end);
    double ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoQuietPawnGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoQuietPawnGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoQuietKnightGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoQuietKnightGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoQuietKnightGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoQuietKnightGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoQuietBishopGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoQuietBishopGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoQuietBishopGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoQuietBishopGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoQuietRookGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoQuietRookGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoQuietRookGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoQuietRookGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoQuietQueenGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoQuietQueenGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoQuietQueenGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoQuietQueenGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoQuietKingGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoQuietKingGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoQuietKingGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoQuietKingGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    //
    // Pseudo Captures:
    //
    
    printf("---\nPseudo Captures:\n\n");

    // PseudoCapturePawnGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoCapturePawnGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoCapturePawnGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoCapturePawnGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoCaptureKnightGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoCaptureKnightGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoCaptureKnightGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoCaptureKnightGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoCaptureBishopGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoCaptureBishopGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoCaptureBishopGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoCaptureBishopGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoCaptureRookGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoCaptureRookGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoCaptureRookGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoCaptureRookGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoCaptureQueenGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoCaptureQueenGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoCaptureQueenGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoCaptureQueenGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // PseudoCaptureKingGen
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        PseudoCaptureKingGen(&boards[(i % 10)],&movelist_sink);
	bench_data.total_moves_generated += movelist_sink.len;
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    bench_data.total_iterations += iterations;
    bench_data.total_ns += total_ns;
    printf("-\nPseudoCaptureKingGen() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("PseudoCaptureKingGen() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    //
    // Main gen:
    //
    printf("---\nMain move gen:\n\n");

    // GenPseudoLegalMoves
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (u64 i = 0; i < iterations; i++) {
        GenPseudoLegalMoves(&boards[(i % 10)],&movelist_sink);
	movelist_sink.len = 0;
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    total_ns = elapsed_nanoseconds(start, end);
    ns_per_call = (double)total_ns / (double)iterations;
    printf("-\nGenPseudoLegalMoves() Total:       %llu ns\n", (unsigned long long)total_ns);
    printf("GenPseudoLegalMoves() ns per call:    %.3f ns\n-\n\n", ns_per_call);

    // GenPseudoLegalMoves
    double ilegal_moves = 0;
    total_ns = 0;
    ns_per_call = 0;
    u64 temp_iterations = 1000000;
    for (u64 i = 0; i < temp_iterations; i++) {
	Board *board = &boards[(i % 10)];
        GenPseudoLegalMoves(board,&movelist_sink);
	clock_gettime(CLOCK_MONOTONIC, &start);

	for (size_t i = 0; i < movelist_sink.len; i++) {
	    Move move = movelist_sink.data[i];

	    State state = MakeMove(board, move);
	    if (EnemyInCheck(board)) {
		ilegal_moves++;	
	    }
	    UndoMove(board, move, state);
	}

	clock_gettime(CLOCK_MONOTONIC, &end);
	movelist_sink.len = 0;
	total_ns += elapsed_nanoseconds(start, end);
    }
    ns_per_call = (double)total_ns / (double)temp_iterations;
    printf("-\nTotal time spent legalizing moves:       %llu ns\n", (unsigned long long)total_ns);
    printf("Average time spent legalizing moves per call of GenPseudoLegalMoves():    %.3f ns\n", ns_per_call);
    printf("Average ilegal moves per call of GenPseudoLegalMoves():    %.3f moves\n-\n\n", ilegal_moves / temp_iterations);

    // display important data at end
    bench_data.average_ns_per_call = (double)bench_data.total_ns / (double)bench_data.total_iterations;
    bench_data.average_moves_per_ns = (double)bench_data.total_moves_generated / (double)bench_data.total_ns;
    printf("\n---\nMovelist sink len: 0x%llx\n",(unsigned long long)movelist_sink.len);
    printf("\nTotal nanoseconds for bench (exlcuding main move gen):    %llu ns\n",(unsigned long long)bench_data.total_ns);
    printf("Average nanoseconds per movegen call (exlcuding main move gen):    %.3f ns\n",bench_data.average_ns_per_call);
    printf("\nTotal Moves generated (exlcuding main move gen):    %llu moves\n",(unsigned long long)bench_data.total_moves_generated);
    printf("Average Moves per ns (exlcuding main move gen):    %.3f moves\n---\n",bench_data.average_moves_per_ns);
    
    return bench_data;
}
