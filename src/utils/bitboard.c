#include "utils/bitboard.h"
#include <stdio.h>

void PrintBitboard(u64 bb) {
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

