#include "bench.h"
#include "globals.h"
#include "movegen/magics.h"
#include "utils/types.h"
#include <stdio.h>
#include <time.h>

#define ITERATIONS 100000000ULL

static volatile u64 benchmark_sink;

static inline u64 elapsed_nanoseconds(struct timespec start, struct timespec end) {
    return (u64)(end.tv_sec - start.tv_sec) * 1000000000ULL + (u64)(end.tv_nsec - start.tv_nsec);
}

static inline void bench_magic(void) {
    const u64 occ = 0xffff00000000ffffULL;

    // Store the result somewhere observable so the compiler
    // cannot simply remove the calculation.
    benchmark_sink ^= RookAttackFromSquare(A1, occ);
}

void bench_magics(void) {
    struct timespec start, end;

    // Optional warm-up: avoids measuring some first-call effects.
    for (u64 i = 0; i < 10000; i++) {
        bench_magic();
    }

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (u64 i = 0; i < ITERATIONS; i++) {
        bench_magic();
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    u64 total_ns = elapsed_nanoseconds(start, end);
    double ns_per_call = (double)total_ns / (double)ITERATIONS;

    printf("Total:       %llu ns\n",
           (unsigned long long)total_ns);

    printf("Per call:    %.3f ns\n",
           ns_per_call);

    // Prevent the compiler from considering benchmark_sink unused.
    printf("Benchmark sink: 0x%llx\n",(unsigned long long)benchmark_sink);
}
