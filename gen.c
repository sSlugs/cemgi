#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include "gen.h"

//
// used to gen constants
//

int main(void) {

    return 0;
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
