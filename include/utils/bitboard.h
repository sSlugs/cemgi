#ifndef BITBOARD_H
#define BITBOARD_H
#include "globals.h"
#include <stdint.h>
#include "utils/types.h"

// sets bit at index
static inline void SetBit(u64 *bb, int index) {
    *bb |= (1ULL << index);
}

// clears bit at index
static inline u64 GetBit(u64 bb, int index) {
    return bb & (1ULL << index);
}

// clears bit at index
static inline void ClearBit(u64 *bb, int index) {
    *bb &= ~(1ULL << index);
}

// pops lowest active bit
static inline void PopLsb(u64 *bb) {
    *bb &= *bb - 1ULL;
}

// returns number of active bits
static inline int PopCountU64(u64 bb) {
    return __builtin_popcountll(bb);
}

// returns the number of 0s until before it finds first active bit. (Ex. 1110_1000 = 3) fast bc it uses cpu instruction
static inline Square LsbIndex(u64 bb) {
    if (bb == 0)
        return NullSquare;

    return (Square)__builtin_ctzll(bb);
}

void PrintBitboard(u64 bb);

#endif
