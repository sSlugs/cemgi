#ifndef MAGICS_H
#define MAGICS_H
#include "globals.h"
#include "stdbool.h"
#include "utils/types.h"

#define NULL_BITBOARD 0xffffffffffffffffULL

// make these available so that movegen can use em
extern u64 ROOK_ATTACKS[64][4096];
extern u64 BISHOP_ATTACKS[64][512];

extern const u64 ROOK_MASKS[64];
extern const u64 BISHOP_MASKS[64];

extern const u64 ROOK_MAGICS[64];
extern const u64 BISHOP_MAGICS[64];

extern const int ROOK_RELEVANT_BITS[64];
extern const int BISHOP_RELEVANT_BITS[64];

// initialize magics so movegen works. MUST DO THIS AT START OF PROGRAM!!!
void InitMagics(void);

// generates magic number for square via brute force
u64 GenMagicForSquare(Square square,bool is_rook,bool print);

// generate all magics
void GenAllMagics(void);

// get attack maps at given square with occupancy
static inline u64 RookAttackFromSquare(Square square, u64 occupancy) {
    return ROOK_ATTACKS[square][((occupancy & ROOK_MASKS[square]) * ROOK_MAGICS[square]) >> (64 - ROOK_RELEVANT_BITS[square])];
}

static inline u64 BishopAttackFromSquare(Square square, u64 occupancy) {
    return BISHOP_ATTACKS[square][((occupancy & BISHOP_MASKS[square]) * BISHOP_MAGICS[square]) >> (64 - BISHOP_RELEVANT_BITS[square])];
}

#endif
