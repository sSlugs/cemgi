#ifndef GEN_H
#define GEN_H
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

// typedefs for unsigned ints
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

// FILES
#define FILE_A 0x101010101010101ULL
#define FILE_B 0x202020202020202ULL
#define FILE_C 0x404040404040404ULL
#define FILE_D 0x808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

// RANKS
#define RANK_1 0xFFULL
#define RANK_2 0xFF00ULL
#define RANK_3 0xFF0000ULL
#define RANK_4 0xFF000000ULL
#define RANK_5 0xFF00000000ULL
#define RANK_6 0xFF0000000000ULL
#define RANK_7 0xFF000000000000ULL
#define RANK_8 0xFF00000000000000ULL

const u64 RANKS[8] = {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
};

const u64 FILES[8] = {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
};

void KnightAttacks(void);
void KingAttacks(void);
void SquareMasks(void);
void InverseSquareMasks(void);
void RookMasksAndBits(void);
void BishopMasksAndBits(void);

// functions

static void PrintBitboard(u64 bb) {
    for (int r = 7; r >= 0; r--) {
	for (int c = 0; c < 8; c++) {
	    int i = (r*8) + c;
	    
	    if (bb & (1ULL << i)) {
		printf("1 ");
	    } else {
		printf("0 ");
	    }
	}
	printf("\n");
    }
}


static inline int PopCountU64(u64 bb) {
    return __builtin_popcountll(bb);
}

static u64 RookAttacksOnFly(int square,u64 occupancy) {
    u64 attacks = 0;
    u64 base = 1ULL << square;
    u64 rank_occ = RANKS[square / 8];
    u64 empty = ~occupancy;

    u64 attack;

    // north ray
    attack = (base << 8);
    do {
	attacks |= attack;
	attack <<= 8;
    } while (attack & empty);

    // south ray
    attack = (base >> 8);
    do {
	attacks |= attack;
	attack >>= 8;
    } while (attack & empty);

    // west ray
    attack = (base >> 1) & rank_occ;
    do {
	attacks |= attack & rank_occ;
	attack >>= 1;
    } while (attack & empty & rank_occ);

    // east ray
    attack = (base << 1) & rank_occ;
    do {
	attacks |= attack & rank_occ;
	attack <<= 1;
    } while (attack & empty & rank_occ);
    
    return attacks;
}

static u64 BishopAttacksOnFly(int square, u64 occupancy) {
    u64 attacks = 0;
    u64 base = 1ULL << square;
    u64 empty = ~occupancy;

    u64 attack;

    // north-east ray
	attack = (base << 9) & ~FILE_A;
    do {
	attacks |= attack;
	attack = (attack << 9) & ~FILE_A;
    } while (attack & empty);

    // north-west ray
	attack = (base << 7) & ~FILE_H;
    do {
	attacks |= attack;
	attack = (attack << 7) & ~FILE_H;
    } while (attack & empty);

    // south-east ray
	attack = (base >> 7) & ~FILE_A;
    do {
	attacks |= attack;
	attack = (attack >> 7) & ~FILE_A;
    } while (attack & empty);

    // south-west ray
	attack = (base >> 9) & ~FILE_H;
    do {
	attacks |= attack;
	attack = (attack >> 9) & ~FILE_H;
    } while (attack & empty);

    return attacks;
}


#endif
