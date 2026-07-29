#ifndef BITBOARD_H
#define BITBOARD_H
#include "globals.h"
#include <stdint.h>
#include "utils/types.h"

// pops lowest active bit
static inline void PopLsb(u64 *bb) {
    *bb &= *bb - 1ULL;
}

// returns the number of 0s until before it finds first active bit. (Ex. 1110_1000 = 3) fast bc it uses cpu instruction
static inline Square LsbIndex(u64 bb) {
    if (bb == 0)
        return NullSquare;

    return (Square)__builtin_ctzll(bb);
}

void PrintBitboard(u64 bb);

#endif
