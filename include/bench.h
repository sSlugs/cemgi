#ifndef BENCH_H
#define BENCH_H
#include "globals.h"

// BenchData, holds information and data about a benchmark
typedef struct {
    u64 total_ns;
    double average_ns_per_call;
    u64 total_moves_generated;
    double average_moves_per_ns;
    u64 total_iterations;
} BenchData;

// bench how long it takes to run a single magic lookup
void bench_magics(void);

// bench how long it takes to run each section of move generation + legalizations
BenchData bench_movegen(void);

#endif
