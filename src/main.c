#include <stdio.h>
#include "bench.h"
#include "board/board.h"
#include "movegen/move.h"
#include "movegen/movegen.h"
#include "utils/bitboard.h"
#include "utils/types.h"
#include "movegen/magics.h"

static u64 BishopAttacksOnFly(Square square, u64 occupancy) {
    u64 attacks = 0;
    u64 base = 1ULL << square;
    u64 empty = ~occupancy;

    u64 attack;

    // north-east ray
    attack = base; 
    do {
	attack = (attack << 9) & ~FILE_A;
	attacks |= attack;
    } while (attack & empty);

    // north-west ray
    attack = base; 
    do {
	attack = (attack << 7) & ~FILE_H;
	attacks |= attack;
    } while (attack & empty);

    // south-east ray
    attack = base; 
    do {
	attack = (attack >> 7) & ~FILE_A;
	attacks |= attack;
    } while (attack & empty);

    // south-west ray
    attack = base; 
    do {
	attack = (attack >> 9) & ~FILE_H;
	attacks |= attack;
    } while (attack & empty);

    return attacks;
}

static u64 RookAttacksOnFly(Square square,u64 occupancy) {
    u64 attacks = 0;
    u64 base = 1ULL << square;
    u64 rank_occ = RANKS[square / 8];
    u64 empty = ~occupancy;
    printf("\n");
    PrintBitboard(empty);
    printf("\n");

    u64 attack;

    // north ray
    attack = base;
    do {
	attack <<= 8;
	attacks |= attack;
    } while (attack & empty);

    // south ray
    attack = base;
    do {
	attack >>= 8;
	attacks |= attack;
    } while (attack & empty);

    // west ray
    attack = base; 
    do {
	attack >>= 1;
	attacks |= attack & rank_occ;
    } while (attack & empty & rank_occ);

    // east ray
    attack = base;
    do {
	attack <<= 1;
	attacks |= attack & rank_occ;
    } while (attack & empty & rank_occ);
    
    return attacks;
}

int main(void ) {
    InitMagics();
    Board board = BoardNew();
    board.occupancy[ALL] = 0xffff18999918ffff;

    u64 bb = RookAttackFromSquare(F5, board.occupancy[ALL]);
    u64 attacks = RookAttacksOnFly(F5,board.occupancy[ALL]);

    PrintBitboard(bb);
    printf("---\n");
    PrintBitboard(attacks);

    return 0;
} 
