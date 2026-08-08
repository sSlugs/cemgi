#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "gen.h"

//
// used to gen constants
//

int main(void) {
    PawnAttacks();

    return 0;
}

void PawnAttacks(void) {
    for (int turn = 0; turn < 2; turn++) {
	printf("{\n");
	for (int sq = 0; sq < 64; sq++) {
	    if (sq % 4 == 0 && sq != 0) {
		printf("\n");
	    }
	    u64 attacks = 0;
	    u64 base = 1ULL << sq;

	    // if white
	    if (!turn) {
		attacks |= (base << 7) & ~FILE_H;
		attacks |= (base << 9) & ~FILE_A;
	    } else {
		attacks |= (base >> 7) & ~FILE_A;
		attacks |= (base >> 9) & ~FILE_H;
	    }
	    printf("0x%" PRIx64 "ULL, ",attacks);
	}
	printf("},\n");
    }
}

void BishopMasksAndBits(void) {
    // Masks
    
    printf("Bishop masks: \n");
    printf("{\n");
    for (int i = 0; i < 64; i++) {
	u64 attacks = BishopAttacksOnFly(i, 0ULL);
	if (i % 4 == 0 && i != 0) {
	    printf("\n");
	}

	int row = i / 8;
	int col = i % 8;

	if (row == 0 || row == 7) {
	    attacks &= ~RANKS[7 - row];
	} else {
	    attacks &= ~(RANK_1 | RANK_8);
	}

	if (col == 0 || col == 7) {
	    attacks &= ~FILES[7 - col];
	} else {
	    attacks &= ~(FILE_A | FILE_H);
	}

	printf("0x%" PRIx64 "ULL, ",attacks);
    }
    printf("\n}\n");

    // Bits
    
    printf("\n\nBishop relevant bits: \n");

    printf("{\n");
    for (int i = 0; i < 64; i++) {
	u64 attacks = BishopAttacksOnFly(i, 0ULL);
	 
	if (i % 8 == 0 && i != 0) {
	    printf("\n");
	}

	int row = i / 8;
	int col = i % 8;

	if (row == 0 || row == 7) {
	    attacks &= ~RANKS[7 - row];
	} else {
	    attacks &= ~(RANK_1 | RANK_8);
	}

	if (col == 0 || col == 7) {
	    attacks &= ~FILES[7 - col];
	} else {
	    attacks &= ~(FILE_A | FILE_H);
	}

	printf("0x%x, ",PopCountU64(attacks));
    }
    printf("\n}\n");
}

void RookMasksAndBits(void) {
    // Masks
    
    printf("Rook masks: \n");
    printf("{\n");
    for (int i = 0; i < 64; i++) {
	u64 attacks = RookAttacksOnFly(i, 0ULL);
	if (i % 4 == 0 && i != 0) {
	    printf("\n");
	}

	int row = i / 8;
	int col = i % 8;

	if (row == 0 || row == 7) {
	    attacks &= ~RANKS[7 - row];
	} else {
	    attacks &= ~(RANK_1 | RANK_8);
	}

	if (col == 0 || col == 7) {
	    attacks &= ~FILES[7 - col];
	} else {
	    attacks &= ~(FILE_A | FILE_H);
	}

	printf("0x%" PRIx64 "ULL, ",attacks);
    }
    printf("\n}\n");

    // Bits
    
    printf("\n\nRook relevant bits: \n");

    printf("{\n");
    for (int i = 0; i < 64; i++) {
	u64 attacks = RookAttacksOnFly(i, 0ULL);
	 
	if (i % 8 == 0 && i != 0) {
	    printf("\n");
	}

	int row = i / 8;
	int col = i % 8;

	if (row == 0 || row == 7) {
	    attacks &= ~RANKS[7 - row];
	} else {
	    attacks &= ~(RANK_1 | RANK_8);
	}

	if (col == 0 || col == 7) {
	    attacks &= ~FILES[7 - col];
	} else {
	    attacks &= ~(FILE_A | FILE_H);
	}

	printf("0x%x, ",PopCountU64(attacks));
    }
    printf("\n}\n");
}

// King attacks 
void KingAttacks(void) {
    printf("{\n");
    for (int i = 0; i < 64; i++) {
	u64 base = 1ULL << i;

	u64 bb = (base << 8);
	bb |= (base << 7) & ~FILE_H;
	bb |= (base >> 1) & ~FILE_H;
	bb |= (base >> 9) & ~FILE_H;
	bb |= (base >> 8);
	bb |= (base >> 7) & ~FILE_A;
	bb |= (base << 1) & ~FILE_A;
	bb |= (base << 9) & ~FILE_A;

	printf("	0x%" PRIx64 ",\n",bb);
    }
    printf("}\n");
};

// Knight attacks 
void KnightAttacks(void) {
    printf("{\n");
    for (int i = 0; i < 64; i++) {
	u64 base = 1ULL << i;

	u64 bb = (base << 15) & ~FILE_H;
	bb |= (base << 6) & ~(FILE_H | FILE_G);
	bb |= (base >> 10) & ~(FILE_H | FILE_G);
	bb |= (base >> 17) & ~FILE_H;

	bb |= (base >> 15) & ~FILE_A;
	bb |= (base >> 6) & ~(FILE_A | FILE_B);
	bb |= (base << 10) & ~(FILE_A | FILE_B);
	bb |= (base << 17) & ~FILE_A;

	printf("	0x%" PRIx64 ",\n",bb);
    }
    printf("}\n");
};

